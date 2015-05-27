/*_############################################################################
  _## 
  _##  agentx_request.cpp  
  _## 
  _##
  _##  AgentX++ 2
  _##  -------------------------------------
  _##  Copyright (C) 2000-2013 - Frank Fock
  _##  
  _##  Use of this software is subject to the license agreement you received
  _##  with this software and which can be downloaded from 
  _##  http://www.agentpp.com
  _##
  _##  This is licensed software and may not be used in a commercial
  _##  environment, except for evaluation purposes, unless a valid
  _##  license has been purchased.
  _##  
  _##########################################################################*/

#include <agentx_pp/agentx_def.h>

#include <agentx_pp/agentx_request.h>
#include <agent_pp/mib_entry.h>
#include <snmp_pp/log.h>

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

static const char *loggerModuleName = "agentx++.agentx_request";

/*-------------------------- class NoVacm -------------------------------*/

NoVacm::NoVacm(): Vacm() 
{
}

NoVacm::~NoVacm()
{
}

/*---------------------- class AgentXRequest ----------------------------*/

AgentXRequest::AgentXRequest(const AgentXPdu& p): Request()
{
	pdu = new AgentXPdu(p);
	init_from_pdu();
	transaction_id = p.get_transaction_id();
	// process all request as if they are v2c requests
	version = version2c;
#ifdef _SNMPv3
	// TODO: This is a hack to get the AgentX context
	// into AGENT++. An AgentX MIB should support contexts
	// although _SNMPv3 is not supported, but this requires
	// changes to AGENT++.
	pdu->set_context_name(p.get_context());
#endif
	lock();
}

AgentXRequest::~AgentXRequest()
{
	unlock();
}

Oidx AgentXRequest::get_oid(int index)
{
	AgentXPdu* axp = (AgentXPdu*)get_pdu();
	if ((axp->get_agentx_type() == AGENTX_GETNEXT_PDU) ||
	    (axp->get_agentx_type() == AGENTX_GETBULK_PDU)) {
		AgentXSearchRange r(axp->get_range(index));
		if (r.is_lower_included()) 
			return r.get_lower().predecessor();	  
	}
	return Request::get_oid(index);
}

void AgentXRequest::set_oid(const Oidx& o, int i)
{
	AgentXPdu* axp = (AgentXPdu*)get_pdu();
	AgentXSearchRange* r = axp->range(i);
	r->set_include(FALSE);
	Request::set_oid(o, i);
}

void AgentXRequest::trim_request(int count) 
{
	Request::trim_request(count);
	AgentXPdu* p = (AgentXPdu*)get_pdu();
	p->trim_ranges(count);
}

bool AgentXRequest::add_rep_row()
{
	if (repeater == 0) return FALSE;
	int rows = (pdu->get_vb_count() - non_rep) / repeater;
	if (rows == 0) return FALSE;

	Vbx vb;
	for (int i=(rows-1)*repeater+non_rep; i<(rows*repeater)+non_rep; i++) {

		int sz = pdu->get_vb_count();
		pdu->get_vb(vb, i);
		*pdu += vb;

		// add range too
		AgentXSearchRange r(((AgentXPdu*)pdu)->get_range(i));
		((AgentXPdu*)pdu)->add_range(r);

		// check if there was room for another vb
		if (pdu->get_vb_count() == sz) return FALSE;
	}

	size = pdu->get_vb_count();
	outstanding += repeater;
	bool* old_done  = done;
	bool* old_ready = ready;
	done  = new bool[size];
	ready = new bool[size];
	
	int j;
	for (j=0; j<size-repeater; j++) { 
		done[j]  = old_done[j]; 
		ready[j] = old_ready[j];
	}
	for (; j<size; j++) {
		done[j]  = FALSE; 
		ready[j] = FALSE;
	}
	delete old_done;
	delete old_ready;
	return TRUE;
}

void AgentXRequest::lock() {
#ifdef NO_FAST_MUTEXES
	LockRequest r(&mutex);
	lockQueue->acquire(&r);
	r.wait();
#else
	mutex.lock();
#endif
}

void AgentXRequest::unlock() {
#ifdef NO_FAST_MUTEXES
	LockRequest r(&mutex);
	lockQueue->release(&r);
	r.wait();
#else
	mutex.unlock();
#endif
}

/*---------------------- class AgentXRequestList ------------------------*/

AgentXRequestList::AgentXRequestList(): RequestList()
{
	agentx = 0;
#ifdef _SNMPv3
	vacm = new NoVacm();
#endif
}

AgentXRequestList::AgentXRequestList(AgentXSlave* slave): RequestList()
{
	agentx = slave;
#ifdef _SNMPv3
	vacm = new NoVacm();
#endif
}

AgentXRequestList::~AgentXRequestList() 
{
	// vacm is deleted by ~RequestList()
}

void AgentXRequestList::answer(Request* req) TS_SYNCHRONIZED(
{
	// CAUTION: Make a copy of PDU here because when we answer
	// the request, the response could be so fast back to our
	// master, that the following request could be processed
	// before we have finished here. In the case of a COMMIT
	// or CLEANUP following a PREPARE_SET this could cause a seg
	// fault because the request were are dealing with here is deleted
	// by the main thread by calling AgentXRequestList::receive!
	//
	AgentXPdu* pdu = new AgentXPdu(*((AgentXPdu*)req->get_pdu()));

	bool remove = TRUE;
	// check if we need request for further processing
	switch (pdu->get_agentx_type()) {
	case AGENTX_TESTSET_PDU:
	case AGENTX_COMMITSET_PDU:
	  remove = FALSE;
	  break;
	} 
	if (remove)
		requests->remove(req);

	pdu->set_agentx_type(AGENTX_RESPONSE_PDU);	

	if (!remove) {
	  // If we do not get a CLEANUP from the master
	  // we have to remove the pending request by ourselves.
	  ((AgentXRequest*)req)->get_agentx_pdu()->
	    set_timestamp(agentx->compute_timeout(AGENTX_DEFAULT_TIMEOUT));
	}

	int status = agentx->send(*pdu);

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 4);
	LOG("RequestListAgentX: request answered (id)(status)(tid)(err)(removed)(sz)");
	LOG(pdu->get_request_id());
	LOG(status);
	LOG(pdu->get_transaction_id());
	LOG(pdu->get_error_status());
	LOG(remove);
	LOG(pdu->get_vb_count());
	LOG_END;

	delete pdu;
})


Request* AgentXRequestList::receive(int sec)
{
	int status = AGENTX_OK;
	AgentXPdu* pdu = agentx->receive(sec, status);
	if (!pdu) return 0;

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	LOG("AgentXRequestList: request received (context)(tid)(pid)(siz)(type)(err)(status)");
        LOG(pdu->get_context().get_printable());
	LOG(pdu->get_transaction_id());
	LOG(pdu->get_packet_id());
	LOG(pdu->get_vb_count());
	LOG(pdu->get_agentx_type());
	LOG(pdu->get_error_status());
	LOG(status);
	LOG_END;
	
	if (status == AGENTX_OK) {

		Array<MibEntry> locks;
		switch (pdu->get_agentx_type()) {
		case AGENTX_GET_PDU:
		case AGENTX_GETNEXT_PDU:
		case AGENTX_GETBULK_PDU:
		  // for each search range create an vb
		  // with the lower bound as oid and null as value 
		  pdu->build_vbs_from_ranges();
		  break;
		case AGENTX_COMMITSET_PDU:
		case AGENTX_CLEANUPSET_PDU:
		case AGENTX_UNDOSET_PDU:
		  AgentXRequest* r = 
		    (AgentXRequest*)
		    find_request_on_id(pdu->get_transaction_id());
		  if (!r) {
			// pdu does not follow a testset pdu -> ignore
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			LOG("AgentXRequestList: commit, cleanup, or undo request does not follow a test set request (pid)(tid)(type)");
			LOG(pdu->get_packet_id());
			LOG(pdu->get_transaction_id());
			LOG(pdu->get_agentx_type());
			LOG_END;
			delete pdu;
			return 0;
		  }
		  // Acquire lock for the existing request, because
		  // it may be still in the queue, when the master
		  // timed out that request. We are blocking here
		  // but this case should be rare. 
		  r->lock();
		  pdu->set_vblist(r->originalVbs, r->originalSize);
		  // copy locks
		  for (int i=0; i<r->originalSize; i++) {
			locks.add(r->get_locked(i));
		  }
		  r->locks.clear();
		  // is done by destructor: r->unlock();
		  delete requests->remove(r);
		}		
		AgentXRequest* req = new AgentXRequest(*pdu);
		// paste locks
		if (locks.size()>0) {
			for (int i=0; i<locks.size(); i++) {
				req->locks.add(locks.getNth(i));
			}
		}
		locks.clear();
		delete pdu;
		// if request is to be ignored req will be 
		// deleted by add_request 
		return add_request(req);

	} // end "if (status == AGENTX_OK)" begin "else"
	else {
		delete pdu;
	}	
	return 0;
}

void AgentXRequestList::remove(Request* req) 
TS_SYNCHRONIZED(
{
	if (req) {
		requests->remove(req);
		delete req;
	}
})

Request* AgentXRequestList::add_request(Request* req) 
TS_SYNCHRONIZED(
{
	// request id == transaction id
	AgentXPdu* pdu = (AgentXPdu*)req->get_pdu();
	unsigned long rid = pdu->get_transaction_id();
	// ignore request, if request_id is already known 
	if (find_request_on_id(rid) != 0) {
		// ignore request
		delete req;
		return 0;
	}

	/* Uncomment this if you need to timeout set requests
	   (this is needed if the master does not send UNDOSET
	   requests when it timed out a COMMIT SET request)
	*/
        timeout_set_requests();

	switch (pdu->get_agentx_type()) {
	case AGENTX_TESTSET_PDU:
	case AGENTX_CLEANUPSET_PDU:
	case AGENTX_COMMITSET_PDU:
	case AGENTX_UNDOSET_PDU:
	case AGENTX_GET_PDU:
	case AGENTX_GETNEXT_PDU:
	case AGENTX_GETBULK_PDU:
	  requests->add(req);
	  break;
	}
	return req;
})

void AgentXRequestList::terminate_set_requests()
{
	ListCursor<Request> cur;
	for (cur.init(requests); cur.get();) {
		AgentXRequest* r = (AgentXRequest*)cur.get();
		if ((r->get_agentx_type() == AGENTX_TESTSET_PDU) ||
		    (r->get_agentx_type() == AGENTX_COMMITSET_PDU)) {

			AgentXPdu* pdu = r->get_agentx_pdu();
			LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
			LOG("AgentXRequestList: terminating SET request (context)(tid)(pid)(siz)(type)");
			LOG(pdu->get_context().get_printable());
			LOG(pdu->get_transaction_id());
			LOG(pdu->get_packet_id());
			LOG(pdu->get_vb_count());
			LOG(pdu->get_agentx_type());
			LOG_END;
			// call cleanup set request to free resources
			for (int i=0; i<r->subrequests(); i++) {
				MibEntry* e = r->get_locked(i);
				if (e) {
					e->cleanup_set_request(r, i);
				}
			}
			cur.next();
			requests->remove(r);
			delete r;
		}
		else
			cur.next();
	}
}

void AgentXRequestList::timeout_set_requests()
{
#ifdef AGENTX_SUBAGENT_TIMEOUT_SET_REQUESTS
	ListCursor<Request> cur;
	for (cur.init(requests); cur.get();) {
		AgentXRequest* r = (AgentXRequest*)cur.get();
		if ((r->get_agentx_type() == AGENTX_TESTSET_PDU) ||
		    (r->get_agentx_type() == AGENTX_COMMITSET_PDU)) {
			time_t ct;
			time(&ct);
			if ((r->get_agentx_pdu()->get_timestamp() > 0) &&
			    (r->get_agentx_pdu()->get_timestamp() < ct)) {

				AgentXPdu* pdu = r->get_agentx_pdu();
				LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
				LOG("AgentXRequestList: SET request timed out (context)(tid)(pid)(siz)(type)(timeout)(curtime)");
			        LOG(pdu->get_context().get_printable());
				LOG(pdu->get_transaction_id());
				LOG(pdu->get_packet_id());
				LOG(pdu->get_vb_count());
				LOG(pdu->get_agentx_type());
				LOG(pdu->get_error_status());
				LOG(pdu->get_timestamp());
				LOG(ct);
				LOG_END;
				// call cleanup set request to free resources
				for (int i=0; i<r->subrequests(); i++) {
					MibEntry* e = r->get_locked(i);
					if (e) {
						e->cleanup_set_request(r, i);
					}
				}
				cur.next();
				requests->remove(r);
				delete r;
			}
			else
				cur.next();
		}
		else
			cur.next();
	}
#endif
}

#ifdef AGENTPP_NAMESPACE
}
#endif

