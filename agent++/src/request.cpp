/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - request.cpp  
  _## 
  _##  Copyright (C) 2000-2013  Frank Fock and Jochen Katz (agentpp.com)
  _##  
  _##  Licensed under the Apache License, Version 2.0 (the "License");
  _##  you may not use this file except in compliance with the License.
  _##  You may obtain a copy of the License at
  _##  
  _##      http://www.apache.org/licenses/LICENSE-2.0
  _##  
  _##  Unless required by applicable law or agreed to in writing, software
  _##  distributed under the License is distributed on an "AS IS" BASIS,
  _##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  _##  See the License for the specific language governing permissions and
  _##  limitations under the License.
  _##  
  _##########################################################################*/

#include <libagent.h>

#include <agent_pp/request.h>
#include <agent_pp/snmp_counters.h>
#include <snmp_pp/oid_def.h>

#include <agent_pp/snmp_group.h>

#ifdef _SNMPv3
#include <agent_pp/v3_mib.h>
#include <agent_pp/vacm.h>
#endif
#include <agent_pp/notification_originator.h>
#include <agent_pp/snmp_community_mib.h>
#include <agent_pp/snmp_target_mib.h>
#include <snmp_pp/log.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

static const char *loggerModuleName = "agent++.request";

/*--------------------------- class Request --------------------------*/

#ifdef NO_FAST_MUTEXES
LockQueue* Request::lockQueue = 0;
#endif

Request::Request()
    :
#ifdef _THREADS
      Synchronized(),
#endif
      pdu(0)
    , originalVbs(0)
    , originalSize(0)
    , from()
    , done(0)
    , ready(0)
    , outstanding(0)
    , size(0)
    , non_rep(0)
    , max_rep(0)
    , repeater(0)
    , version()
    , transaction_id(0)
    , locks()
#ifdef _SNMPv3
    , viewName()
    , vacm(0)
#endif
    , target()
{
#ifdef NO_FAST_MUTEXES
	init_lock_queue();    
#endif
}

Request::Request(const Pdux& p, const TargetType &t)
    : 
#ifdef _THREADS
      Synchronized(),
#endif
      pdu(0)
    , originalVbs(0)
    , originalSize(0)
    , from()
    , done(0)
    , ready(0)
    , outstanding(0)
    , size(0)
    , non_rep(0)
    , max_rep(0)
    , repeater(0)
    , version()
    , transaction_id(0)
    , locks()
#ifdef _SNMPv3 
    , viewName()
    , vacm(0)
#endif 
    , target(t)
{
#ifdef NO_FAST_MUTEXES
	init_lock_queue();    
#endif
	pdu = p.clone();
	init_from_pdu();

        version = target.get_version();
        GenAddress f;
        target.get_address(f);
        from = f;
}

void Request::init_from_pdu() 
{
	size		= pdu->get_vb_count();
	done		= new bool[size];
	ready		= new bool[size];
	originalVbs     = new Vbx[size];
	pdu->get_vblist(originalVbs, size);

	for (int i=0; i<size; i++) { 
		done[i] = false; 
		ready[i] = false; 
	}
	//memset(done, FALSE, sizeof(bool)*size);
	//memset(ready, FALSE, sizeof(bool)*size);

	if (pdu->get_type() == sNMP_PDU_GETBULK) {
		non_rep = pdu->get_error_status();
		max_rep = pdu->get_error_index();
		if (non_rep<0) non_rep = 0;
		if (max_rep<0) max_rep = 0;
		if ((max_rep == 0) && (size > non_rep)) {
			trim_request(non_rep);
		}
		repeater = size-non_rep;
	}
	else {
		repeater = 0;
		non_rep = size;
		max_rep = 0;
	}
	// must be placed here because trim() could have decreased size!
	outstanding = size;
	if (pdu->get_type() != sNMP_PDU_RESPONSE) {
		pdu->set_error_status(0);
		pdu->set_error_index(0);
	}
	originalSize = size;

	phase = PHASE_DEFAULT;
}

Request::Request(const Request& other) 
{
#ifdef NO_FAST_MUTEXES
	init_lock_queue();    
#endif
	pdu = other.pdu->clone();
	originalVbs = new Vbx[other.originalSize];
	for (int i=0; i<other.originalSize; i++) {
		originalVbs[i] = other.originalVbs[i];
	}
	from = other.from;
	done = new bool[other.size];
	ready = new bool[other.size];
	for (int j=0; j<other.size; j++) {
		done[j] = other.done[j];
		ready[j] = other.ready[j];
	}
	size = other.size;
	outstanding = other.outstanding;
	non_rep = other.non_rep;
	max_rep = other.max_rep;
	repeater = other.repeater;
	version = other.version;
	transaction_id = other.transaction_id;
#ifdef _SNMPv3
	viewName = other.viewName;
	vacm = other.vacm;
#endif
	target = other.target;
	phase = other.phase;
}

Request::~Request()
{
	delete pdu;
	delete[] done;
	delete[] ready;
	delete[] originalVbs;
	for (int i=0; i<locks.size(); i++) {
		set_unlocked(i);
	}
	locks.clear();
#ifdef NO_FAST_MUTEXES
	// There may be some locks acquired on behalf of this
	// request that may block another SET request that has
	// acquired locks through the lock queue. So we notify
	// the queue here to be sure that the other thread 
	// may proceed.
        TryLockResult lockState;
	if ((lockState = lockQueue->trylock()) != BUSY) {
            lockQueue->notify();
            if (lockState == LOCKED) {
                lockQueue->unlock();
            }
        }
        else {
            LOG_BEGIN(loggerModuleName, WARNING_LOG | 2);
            LOG("Request: Destroyed although lockQueue locked by other thread (tid)");
            LOG(transaction_id);
            LOG_END;            
        }
#endif
}


#ifdef NO_FAST_MUTEXES
void Request::init_lock_queue() 
{
	if (!lockQueue)
		lockQueue = new LockQueue();
}
#endif

int Request::position(const Vbx& vb)
{
	Vbx* vbs = new Vbx[size];
	pdu->get_vblist(vbs, size);

	for (int i=0; i<size; i++) {	
		if (vb.get_oid() == vbs[i].get_oid()) {
			delete[] vbs;
			return i;
		}
	}
	delete[] vbs;
	return -1;
}

int Request::first_pending() const
{
	for (int i=0; i<size; i++) {	
		if (!done[i]) return i;
	}
	return -1;
}

/**
 * Check whether the receiver contains a specified variable binding.
 * Two variable bindings are supposed to be the same, if their oids
 * are equal.
 * 
 * @param vb - A variable binding.
 */
bool Request::contains(const Vbx& vb)
{
	return (position(vb)>=0);
}

/**
 * Check whether the request is finished (all variable bindings
 * have been processed).
 *
 * @return TRUE if the request is complete, FALSE otherwise.
 */ 
bool Request::finished() const
{
	return (outstanding <= 0);
}

/**
 * Check whether a specified variable binding (sub-request) 
 * has been processed.
 *
 * @param i - The index (starting from 0) of the variable binding
 *            to check.
 * @return TRUE if the sub-request is done, FALSE otherwise.
 */   
bool Request::is_done(int i) const
{
	if ((i>=0) && (i<size)) 
		return done[i];
	return FALSE;
}

/**
 * Check whether a specified variable binding (sub-request) 
 * is ready to commit (applies only for SET-Requests).
 *
 * @param i - The index (starting from 0) of the variable binding
 *            to check.
 * @return TRUE if the sub-request is ready, FALSE otherwise.
 */   
bool Request::is_ready(int i) const
{
	if ((i>=0) && (i<size)) 
		return ready[i];
	return FALSE;
}

void Request::set_ready(int i)
{
	if ((i>=0) && (i<size)) ready[i] = TRUE;
}

void Request::unset_ready(int i)
{
	if ((i>=0) && (i<size)) ready[i] = FALSE;
}

void Request::check_exception(int i, Vbx& vbl)
{
	if (vbl.get_exception_status() != 0) {
		if (pdu->get_type() == sNMP_PDU_GETBULK) {
					LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
		LOG("RequestList: finished subrequest (ind)");
		LOG(i);
		LOG_END;
if ((i < non_rep) || (i < originalSize))
				vbl.set_oid(originalVbs[i].get_oid());
			/*			else {
				vbl.set_oid(originalVbs[((i-non_rep)%repeater)+
						       non_rep].get_oid());
			}
			*/
		} 
		else {
			// an error occured->make sure 
		     	//original oid is returned
			vbl.set_oid(originalVbs[i].get_oid());
		}
	}
}

void Request::finish(int i)
{
	if ((i>=0) && (i<size)) {
		if (!done[i]) outstanding--;
		done[i] = TRUE;
		LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
		LOG("RequestList: finished subrequest (ind)");
		LOG(i);
		LOG_END;
	}
}
	  
void Request::finish(int i, const Vbx& vb)
{
	if ((i>=0) && (i<size)) {
	  
		Vbx vbl(vb);
		if (!done[i]) outstanding--;
		check_exception(i, vbl);
		done[i] = TRUE;
		pdu->set_vb(vbl, i);

		LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
		LOG("RequestList: finished subrequest (ind)(oid)(val)(syn)");
		LOG(i);
		LOG(vbl.get_printable_oid());
		LOG(vbl.get_printable_value());
		LOG(vbl.get_syntax());
		LOG_END;
	}
}

void Request::error(int index, int error)
{
	outstanding = 0;
	// error index is one based, whereas subrequest index is zero based 
	pdu->set_error_index(index+1);
	pdu->set_error_status(error);
	if (pdu->get_type() == sNMP_PDU_GETBULK) {
		pdu->set_vblist(originalVbs, originalSize);
		pdu->set_error_status(SNMP_ERROR_GENERAL_VB_ERR);
	}
	else {
	  if (((index>=0) && (index<size)) && (index<originalSize)) {	
		// restore original variable binding
		pdu->set_vb(originalVbs[index], index);
	  }
	}
}

#ifdef _SNMPv3

void Request::init_vacm(Vacm* v, const OctetStr& vname)
{
	vacm = v;
	viewName = vname;
}

void Request::vacmError(int index, int error)
{
	outstanding = 0;
        switch (error) {
          case VACM_noSuchView:
          case VACM_noAccessEntry:
	  case VACM_noGroupName: {
            pdu->set_error_status(SNMP_ERROR_AUTH_ERR);
	    pdu->set_error_index(0);
            LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
            LOG("Request: SNMPv3 VACM auth failure:");
            LOG(vacm->getErrorMsg(error));
            LOG_END;
            break;
	  }
          case VACM_notInView: {
            pdu->set_error_status(SNMP_ERROR_NO_ACCESS);
	    pdu->set_error_index(index+1);
            LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
            LOG("Request: SNMPv3 VACM no access:");
            LOG(vacm->getErrorMsg(error));
            LOG_END;
            break;
          }
          case VACM_otherError: {
            pdu->set_error_status(SNMP_ERROR_GENERAL_VB_ERR);
	    pdu->set_error_index(index+1);
            LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
            LOG("Request: SNMPv3 VACM genError:");
            LOG(vacm->getErrorMsg(error));
            LOG_END;
            break;
          }
          default: {
            pdu->set_error_status(SNMP_ERROR_GENERAL_VB_ERR);
	    pdu->set_error_index(index+1);
            LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
            LOG("Request: SNMPv3 VACM ERROR in Request::vacmError:");
            LOG(vacm->getErrorMsg(error));
            LOG_END;
            break;
          }
        }
}

#endif

#ifdef _SNMPv3
/**
 * Return the security_name/community string of the receiver request.
 *
 * @param s - An OctetStr to hold the returned security_name 
 *            Note: the SNMPv1/v2c community is mapped to 
 *                  the security_name.
 */ 
void Request::get_security_name(OctetStr &s)
{
  target.get_security_name(s);
}
#endif
/**
 * Return the variable binding of the specified sub-request.
 *
 * @param index - The index of the sub-request that failed.
 * @return A variable binding.
 */ 
Vbx Request::get_value(int i)
{
	return (*pdu)[i];
}

Vbx* Request::search_value(const Oidx& oid) const
{
	for (int i=0; i<size; i++) {
		Oidx o;
		Vbx vb;
		pdu->get_vb(vb, i);
		vb.get_oid(o);	
		if (o == oid)
			return new Vbx((*pdu)[i]);
	}
	return 0;
}

/**
 * Return the syntax of the specified sub-request (variable binding).
 *
 * @param index - An index of a sub-request.
 * @return A SMI syntax.
 */ 
SnmpInt32 Request::get_syntax(int i)
{
	return (*pdu)[i].get_syntax();
}
	
/**
 * Return the object identifier of the specified receiver�s 
 * sub-request. 
 * 
 * @note This method is not "const" because AgentX++ overwrites
 * non-const.
 *
 * @param index - An index of a sub-request. 
 * @return An object identifier.
 */
Oidx Request::get_oid(int i)
{
	Oidx retval;
	Vbx vb;
	pdu->get_vb(vb, i);
	vb.get_oid(retval);	
	return retval;
}

/**
 * Set the object identifier of a specified sub-request.
 *
 * @param oid - An object identifier.
 * @param index - An index of a sub-request (starting from 0).
 */  
void Request::set_oid(const Oidx& o, int i)
{
	Vbx vb;
	pdu->get_vb(vb, i);
	vb.set_oid(o);
	pdu->set_vb(vb, i);
}
	
/**
 * Add a repetition row to the GETBULK request PDU.
 *
 * @return TRUE if there was enough room in the response PDU for
 *         another repetition, FALSE otherwise.
 */ 
bool Request::add_rep_row()
{
	if (repeater == 0) return FALSE;
	int rows = (pdu->get_vb_count() - non_rep) / repeater;
	if (rows == 0) return FALSE;
	
	if (pdu->get_asn1_length() >= get_max_response_length())
		return FALSE;

	Vbx vb;
	for (int i=(rows-1)*repeater+non_rep; i<(rows*repeater)+non_rep; i++) {

		pdu->get_vb(vb, i);
		*pdu += vb;
		// check if there was room for another vb
		// obsolete: if (pdu->get_vb_count() == sz) return FALSE;
	}

	if (pdu->get_asn1_length() > get_max_response_length()) {
		for (int i=0; i<repeater; i++)
			pdu->trim();
		return FALSE;	
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
	delete [] old_done;
	delete [] old_ready;
	return TRUE;
}

bool Request::init_rep_row(int row)
{
	int start = non_rep + row * repeater;
	int end   = start + repeater;
	if ((row < 1) || (end > size)) return FALSE;
	for (int i=start; i<end; i++) {
		if ((!is_done(i)) && (!is_ready(i))) {
			set_oid(get_oid(i-repeater), i);
		}
	}
	return TRUE;
}

void Request::trim_request(int count) 
{
	if (pdu->trim(pdu->get_vb_count()-count))
		size = pdu->get_vb_count();
}

/**
 * Increment the number of variable bindings to be processed by one.
 */
void Request::inc_outstanding()
{
	outstanding++;
}	

/**
 * Decrement the number of variable bindings to be processed by one
 */
void Request::dec_outstanding()
{
	outstanding--;
}	

void Request::no_outstanding()
{
	outstanding = 0;
}	

MibEntry* Request::get_locked(int i) 
{
	if ((i>=0) && (i<locks.size()))
		return locks.getNth(i);
	return 0;
}

int Request::lock_index(MibEntry* entry) 
{
	for (int i=0; i<locks.size(); i++) {
		MibEntry* l = locks.getNth(i);
		if (l == entry)
			return i;
		else if ((l) && (l->type() == AGENTPP_TABLE) &&
			 (((MibTable*)l)->has_listeners())) {

			ListCursor<MibTable>* cur = 
			    ((MibTable*)l)->get_listeners();
			for (;cur->get();cur->next()) {
				if (cur->get() == entry)
				{
					delete cur;
					return i;
				}
			}
			delete cur;
		}
	}
	return -1;
} 

void Request::set_locked(int i, MibEntry* entry) 
{
	if ((i<0) || (i>=size)) return;
	while (i>=locks.size()) {
		locks.add(0);
	}
	if (lock_index(entry) < 0) {
#ifdef NO_FAST_MUTEXES
		LockRequest r(entry);
		lockQueue->acquire(&r);
		r.wait();
#else
		entry->start_synch();
#endif
		// acquire locks for all listeners of a
		// table object. This makes sure that all
		// such locks can be get without causing deadlocks 
		if (entry->type() == AGENTPP_TABLE) {
		    ListCursor<MibTable>* cur = 
			((MibTable*)entry)->get_listeners();
		    for (;cur->get();cur->next()) {
			if (lock_index(cur->get()) < 0) {
#ifdef NO_FAST_MUTEXES
			    LockRequest r(cur->get());
			    lockQueue->acquire(&r);
			    r.wait();
#else
			    cur->get()->start_synch();
#endif		
			}	
		    }
		    delete cur;
		}
	}
	locks.overwriteNth(i, entry);
}

void Request::set_unlocked(int i)
{
	if ((i<0) || (i>=size)) return;
	MibEntry* entry = locks.getNth(i);
	if (entry) {
		locks.clear(i);
		if (locks.index(entry) < 0) {
			// release locks for all listeners of a
			// table object. 
			if (entry->type() == AGENTPP_TABLE) {
			    ListCursor<MibTable>* cur = 
				((MibTable*)entry)->get_listeners();
			    for (;cur->get();cur->next()) {
				if (lock_index(cur->get()) < 0) {
#ifdef NO_FAST_MUTEXES
				    LockRequest r(cur->get());
				    lockQueue->release(&r);
				    r.wait();
#else
				    cur->get()->end_synch();
#endif			
				}
			    }
			    delete cur;
			}
#ifdef NO_FAST_MUTEXES
			LockRequest r(entry);
			lockQueue->release(&r);
			r.wait();
#else
			entry->end_synch();	
#endif
		}
	}
}

void Request::trim_bulk_response()
{
	if (get_type() != sNMP_PDU_GETBULK)
		return;
	int nonrep = get_non_rep();
	int maxrep = get_max_rep();
	int rep = get_rep();
	int j=0;
	int end = 0;
	for (int i=nonrep; i<pdu->get_vb_count(); i++) {
		j = (i-nonrep)/rep;
		if (j > maxrep) {
			trim_request(nonrep+maxrep*rep);
			break;
		}
		if ((*pdu)[i].get_syntax() == 
		    sNMP_SYNTAX_ENDOFMIBVIEW) {
			end++;
			if ((j==0) && (i<originalSize)) {
				(*pdu)[i].set_oid(originalVbs[i].get_oid());
			}
			else {
				(*pdu)[i].set_oid(Request::get_oid(i-rep));
			}
		}
		if (end >= rep) {
			trim_request(i+1);
			break;
		}
	} 
}

int Request::get_max_response_length() 
{ 
#ifdef _SNMPv3
	if (version < version3) 
		return MAX_SNMP_PACKET;
	return (pdu->get_maxsize_scopedpdu()>=MAX_SNMP_PACKET)
		  ? MAX_SNMP_PACKET : pdu->get_maxsize_scopedpdu(); 
#else
	return MAX_SNMP_PACKET;
#endif
}


/*------------------------- class RequestList --------------------------*/

RequestList::RequestList()
    : ThreadManager()
    , requests( new List<Request> )
    , snmp(0)
#ifdef _SNMPv3
    , vacm(0)
    , v3mp(0)
#endif
    , write_community(new OctetStr(DEFAULT_WRITE_COMMUNITY))
    , read_community(new OctetStr(DEFAULT_READ_COMMUNITY))
    , next_transaction_id(0)
    , sourceAddressValidation(false)
{
}

RequestList::~RequestList() TS_SYNCHRONIZED(
{	
	delete requests;
	if (write_community)
		delete write_community;
	if (read_community)
		delete read_community;
})

void RequestList::set_address_validation(bool srcValidation)
{
	sourceAddressValidation = srcValidation;
} 


Request* RequestList::get_request(unsigned long rid)
{
	ListCursor<Request> cur;
	for (cur.init(requests); cur.get(); cur.next()) {
		if (cur.get()->get_transaction_id() == rid) {
			return cur.get();
		}
	}
	return 0;
}
	
Request* RequestList::find_request_on_id(unsigned long rid)
{
	ListCursor<Request> cur;
	for (cur.init(requests); cur.get(); cur.next()) {
		if (cur.get()->get_pdu()->get_request_id() == rid) {
			return cur.get();
		}
	}
	return 0;
}

unsigned long RequestList::get_request_id(const Vbx& vb) TS_SYNCHRONIZED(
{
        ListCursor<Request> cur;
	for (cur.init(requests); cur.get(); cur.next()) {
		if (cur.get()->contains(vb)) {
			return cur.get()->get_transaction_id();
		}
        }
        return 0;
})

bool RequestList::done(unsigned long rid, int index, const Vbx& vb) 
TS_SYNCHRONIZED(
{
	Request* req = get_request(rid);
	if (req) {
		req->finish(index, vb);
		if (req->finished()) 
			return TRUE;
	}
	else {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("RequestList: done: can't find request id");
		LOG(rid);
		LOG_END;
	}
	return FALSE;
})

void RequestList::error(unsigned long rid, int index, int err) 
TS_SYNCHRONIZED(
{
	Request* req = get_request(rid);
	if (req) {
		req->error(index, err);
	}
	else {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("RequestList: done: can't find request id");
		LOG(rid);
		LOG_END;
	}
})

/**
 * Check whether a given community is acceptable for the specified
 * PDU type.
 *
 * @param pdutype - A PDU type (e.g., sNMP_PDU_SET, 
 *                  sNMP_PDU_GET, etc.)
 * @param community - A v1 or v2c community string.
 * @return TRUE if the given community is ok, FALSE otherwise.
 */ 
bool RequestList::community_ok(int pdutype, const OctetStr& community)
{
#ifdef SNMPv3
	return TRUE;
#else
	switch (pdutype) {
	case sNMP_PDU_SET:
		if (*write_community == community)
			return TRUE;
		else {
			if (*read_community == community) { 
				MibIIsnmpCounters::incInBadCommunityUses();
			}
			return FALSE;
		}
	}
	if ((*read_community == community) || 
	    (*write_community == community))  
		return TRUE;
	
	return FALSE;
#endif
}

void RequestList::remove_request(Request* req)
{
    ThreadSynchronize _ts_synchronize(*this);

    if (!req) return;

    requests->remove(req);

#ifdef _SNMPv3	
    if (req->get_security_model() == version3)
    {
	v3MP::I->delete_from_cache(req->get_pdu()->get_request_id(),
				   req->get_pdu()->get_message_id(), false);
    }
#endif

    delete req;
}
	
#ifdef _SNMPv3	
void RequestList::report(Request* req)
{
	ThreadSynchronize sync(*this); // synchronize this method

	Pdux* pdu = req->get_pdu();

	MibIIsnmpCounters::incOutPkts();

	pdu->set_error_status(0);
	pdu->set_error_index(0);
	pdu->set_type(sNMP_PDU_RESPONSE);	

	requests->remove(req);
	
	int status;
	status = snmp->report(*pdu, req->target);

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 4);
	LOG("RequestList: sent report (rid)(tid)(to)(err)(send)(sz)");
	LOG(pdu->get_request_id());
	LOG(req->get_transaction_id());
	LOG(req->from.get_printable());
	LOG(pdu->get_error_status());
	LOG(status);
	LOG(req->get_pdu()->get_vb_count());
	LOG_END;
}
#endif

void RequestList::null_vbs(Request* req) 
{
    Pdux* pdu = req->get_pdu();
    for (int i=0; ((i<req->subrequests()) && 
		   (i<pdu->get_vb_count())); i++) {
	Vbx  null;
	null.set_oid(req->get_oid(i));
	(*pdu)[i] = null;	
    }
}

void RequestList::answer(Request* req) 
{
	ThreadSynchronize sync(*this); // synchronize this method

	Pdux* pdu = req->get_pdu();
	// assure backward compatibility to SNMPv1
	if (req->version == version1) { 
		switch (pdu->get_error_status()) {
		case SNMP_ERROR_NOT_WRITEABLE: 
		case SNMP_ERROR_NO_ACCESS:
		case SNMP_ERROR_NO_CREATION:
		case SNMP_ERROR_INCONSIS_NAME:
		case SNMP_ERROR_AUTH_ERR:
		    pdu->set_error_status(SNMP_ERROR_NO_SUCH_NAME);
		    break;
		case SNMP_ERROR_RESOURCE_UNAVAIL:
		case SNMP_ERROR_COMITFAIL:
		case SNMP_ERROR_UNDO_FAIL:
		    pdu->set_error_status(SNMP_ERROR_GENERAL_VB_ERR);
		    break;
		case SNMP_ERROR_WRONG_VALUE:
		case SNMP_ERROR_WRONG_LENGTH:
		case  SNMP_ERROR_INCONSIST_VAL:
		case SNMP_ERROR_WRONG_TYPE:
		    pdu->set_error_status(SNMP_ERROR_BAD_VALUE);
		    break;
		}
	}   
	for (int i=0; ((i<req->subrequests()) && 
		       (i<pdu->get_vb_count())); i++) {
		Vbx vb((*pdu)[i]);
		switch (vb.get_exception_status()) {
		case sNMP_SYNTAX_NOSUCHOBJECT:
		case sNMP_SYNTAX_NOSUCHINSTANCE: 
		case sNMP_SYNTAX_ENDOFMIBVIEW: {

			if (req->version == version1) {
			    Vbx  null;
			    null.set_oid(req->get_oid(i));
			    (*pdu)[i] = null;
			    pdu->set_error_status(SNMP_ERROR_NO_SUCH_NAME);
			    pdu->set_error_index(i+1);
			}			  
			break;
		}
		}		
		switch (vb.get_exception_status()) {
		case sNMP_SYNTAX_NOSUCHOBJECT: {

			if (req->version >= version2c)
				MibIIsnmpCounters::incOutNoSuchNames();
			break;
		}
		case sNMP_SYNTAX_NOSUCHINSTANCE: {

			if (req->version >= version2c)
				MibIIsnmpCounters::incOutBadValues();
			break;
		}
		case sNMP_SYNTAX_ENDOFMIBVIEW: {
			break;
		}
		default:
			if (pdu->get_type() == sNMP_PDU_SET) 
				MibIIsnmpCounters::incInTotalSetVars();
			else 
				MibIIsnmpCounters::incInTotalReqVars();
		}
	}

	if ((req->get_error_status() != SNMP_ERROR_SUCCESS) &&
	    (pdu->get_type() != sNMP_PDU_SET)) {
	    null_vbs(req);
	}

	MibIIsnmpCounters::incOutPkts();

	switch (pdu->get_type()) {
	case sNMP_PDU_GET: {
		MibIIsnmpCounters::incInGetRequests();
		break;
	}
	case sNMP_PDU_GETBULK:
	case sNMP_PDU_GETNEXT: {
		MibIIsnmpCounters::incInGetNexts();
		break;
	}
	case sNMP_PDU_SET: {
		MibIIsnmpCounters::incInSetRequests();
		break;
	}
	case sNMP_PDU_V1TRAP:
	case sNMP_PDU_TRAP: {
		MibIIsnmpCounters::incInTraps();
		requests->remove(req);
		return; // do not answer traps
	}
	}

	// check message length
	if (pdu->get_asn1_length() > req->get_max_response_length()) {

		LOG_BEGIN(loggerModuleName, WARNING_LOG | 2);
		LOG("RequestList: response tooBig, truncating it (rid)(tid)(to)(size)(limit)");
		LOG(pdu->get_request_id());
		LOG(req->get_transaction_id());
		LOG(req->from.get_printable());
		LOG(pdu->get_asn1_length());
		LOG(req->get_max_response_length());
		LOG_END;
		
		if (pdu->get_type() == sNMP_PDU_GETBULK) {
		  do {
			pdu->trim(req->get_rep());
		  } 
		  while ((pdu->get_vb_count() > req->get_non_rep()) &&
		         (pdu->get_asn1_length() > 
			  req->get_max_response_length()));
		}
		if (pdu->get_asn1_length() > req->get_max_response_length()) {
			pdu->trim(pdu->get_vb_count());
			pdu->set_error_status(SNMP_ERROR_TOO_BIG);
		}
	}

	switch (pdu->get_error_status()) {
	case SNMP_ERROR_TOO_BIG: {
		MibIIsnmpCounters::incOutTooBigs();
		break;
	}
	case SNMP_ERROR_NO_SUCH_NAME: {
		MibIIsnmpCounters::incOutNoSuchNames();
		break;
	}
	case SNMP_ERROR_BAD_VALUE: {
		MibIIsnmpCounters::incOutBadValues();
		break;
	}
	default: {
		if (pdu->get_error_status() != SNMP_ERROR_SUCCESS) 
			MibIIsnmpCounters::incOutGenErrs();
		break;
	}
	}

	int ptype = pdu->get_type();
	pdu->set_type(sNMP_PDU_RESPONSE);	

	requests->remove(req);
	
#ifdef _SNMPv3
	int status = snmp->send (*pdu, &(req->target));
#else
	int status = snmp->send (*pdu, req->from, 
				 req->target.get_version(), 
				 req->target.get_readcommunity());
#endif
	if (status == SNMP_ERROR_TOO_BIG) {
		
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 3);
		LOG("RequestList: response tooBig (rid)(tid)(to)");
		LOG(pdu->get_request_id());
		LOG(req->get_transaction_id());
		LOG(req->from.get_printable());
		LOG_END;
		
		if (ptype == sNMP_PDU_GETBULK) {
		  do {
			pdu->trim(req->get_rep());
#ifdef _SNMPv3
			status = snmp->send (*pdu, &(req->target));
#else
			status = snmp->send (*pdu, req->from, 
					     req->target.get_version(), 
					     req->target.get_readcommunity());
#endif			
		  } while ((status == SNMP_ERROR_TOO_BIG) &&
			   (pdu->get_vb_count() >= 
			    req->get_non_rep() + 2*req->get_rep()));
		}
		if (status == SNMP_ERROR_TOO_BIG) {
			pdu->set_vblist(0, 0);
			pdu->set_error_status(SNMP_ERROR_TOO_BIG);
			pdu->set_error_index(0);
#ifdef _SNMPv3
			status = snmp->send (*pdu, &(req->target));
#else
			status = snmp->send (*pdu, req->from, 
					     req->target.get_version(), 
					     req->target.get_readcommunity());
#endif
			MibIIsnmpCounters::incOutTooBigs();
		}
	}
	MibIIsnmpCounters::incOutGetResponses();

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	LOG("RequestList: request answered (rid)(tid)(to)(err)(send)(sz)");
	LOG(pdu->get_request_id());
	LOG(req->get_transaction_id());
	LOG(req->from.get_printable());
	LOG(pdu->get_error_status());
	LOG(status);
	LOG(req->get_pdu()->get_vb_count());
	LOG_END;
}


Request* RequestList::receive(int sec)
{
#ifdef _SNMPv3
	if (vacm == 0) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
		LOG("RequestList: SNMPv3 support enabled, but VACM not initialized: ");
		LOG_END;
		return 0; // not executed if logging disabled
	}
	if (v3mp == 0) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
		LOG("RequestList: SNMPv3 support enabled, but v3MP not initialized: ");
		LOG_END;
		return 0; // not executed if logging disabled
	}
#endif		
	struct timeval* tvptr = 0;
	if (sec >= 0) {
		tvptr = new (struct timeval);

		tvptr->tv_sec  = sec;    // wait up to sec seconds
		tvptr->tv_usec = 0;
	}

	Pdux         pdu;
#ifdef _SNMPv3
	UTarget target;
	int status = snmp->receive(tvptr, pdu, target);
#else
	UdpAddress   from;
	snmp_version version = version1;
	OctetStr     community;

	int status = snmp->receive(tvptr, pdu, from, version, community);
	CTarget target(from);	
	target.set_version(version);
	target.set_readcommunity(community);
	target.set_writecommunity(community);	
#endif
	if (tvptr) delete tvptr;

	if (status != SNMP_CLASS_TL_FAILED)
	        // do not increment incoming packets for timeouts on select
		MibIIsnmpCounters::incInPkts();

	if ((status == SNMP_CLASS_SUCCESS) ||
	    (status == SNMP_ERROR_TOO_BIG)) {
                GenAddress tmp_addr;
		snmp_version version;
		// security_name replaces community!
		OctetStr        security_name;
#ifdef _SNMPv3
		int             security_model;
		int             security_level;
		OctetStr        context_engine_id;
		OctetStr        context_name;
#endif		
		version = target.get_version();
		target.get_address(tmp_addr);
                UdpAddress from(tmp_addr);

#ifdef _SNMPv3
		target.get_security_name(security_name);
                security_model = target.get_security_model();
                security_level = pdu.get_security_level();
                pdu.get_context_engine_id(context_engine_id);
                pdu.get_context_name(context_name);
#else
		security_name = community;
#endif		
		LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
		LOG("RequestList: request received (id)(siz)(fro)(ver)(com)(type)");
		LOG(pdu.get_request_id());
		LOG(pdu.get_vb_count());
		LOG(from.get_printable());
                switch (version) { 
                  case version1:  { LOG("SNMPv1"); break; }
                  case version2c: { LOG("SNMPv2c"); break; }
#ifdef _SNMPv3
                  case version3:  { LOG("SNMPv3"); break; }
#endif
                  default:        { LOG("unknown"); break; }
                }
		LOG(security_name.get_printable());
		LOG(pdu.get_type());
		LOG_END;
#ifdef _SNMPv3
		if (version== version3) {
		  LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
		  LOG("RequestList: request received: (secmod)(seclev)(cid)(cname): ");
		  LOG(security_model);
		  LOG(security_level);
		  LOG(context_engine_id.get_printable());
		  LOG(context_name.get_printable());
                  LOG_END;
                  
		  if (security_model != 3) {
		    LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		    LOG("Request: Unknown security model");
		    LOG(security_model);
		    LOG_END;
		    v3mp->inc_stats_unknown_security_models();
		    v3mp->inc_stats_invalid_msgs();
		    return 0;
		  }
		}
		else {
		  
		  if (snmpCommunityEntry::instance) {

		    OctetStr transport_tag;
		    bool found = snmpCommunityEntry::instance->
		      get_v3_info(security_name, context_engine_id, 
				  context_name, transport_tag);
		    
		    LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
		    LOG("RequestList: received v1/v2c request (FOUND)(community)(cid)(cname)(filter_tag): ");
		    LOG((found)?"TRUE":"FALSE");
		    LOG(security_name.get_printable());
		    LOG(context_engine_id.get_printable());
		    LOG(context_name.get_printable());
		    LOG(transport_tag.get_printable());
		    LOG_END;
		    
		    if (!found) {
			LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
			LOG("RequestList: v1/v2c bad community (comm)(rid): ");
			LOG(security_name.get_printable());
			LOG(pdu.get_request_id());
			LOG_END;

			authenticationFailure(context_name,
					      target.get_address(),
					      0);

			MibIIsnmpCounters::incInBadCommunityNames();
			
			return 0;
		    }

		    pdu.set_context_engine_id(context_engine_id);
		    pdu.set_context_name(context_name);

		    if (version == version2c)
		      security_model = SNMP_SECURITY_MODEL_V2;
		    else
		      security_model = SNMP_SECURITY_MODEL_V1;

		    security_level = SNMP_SECURITY_LEVEL_NOAUTH_NOPRIV;

		    if ((sourceAddressValidation) &&
			(snmpTargetAddrEntry::instance) &&
			(snmpTargetAddrExtEntry::instance)) {
		      if (!snmpTargetAddrExtEntry::instance->
			  passes_filter(transport_tag, target)) {

			LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
			LOG("RequestList: unauthorized v1/v2c request (from)(rid): ");
			LOG(tmp_addr.get_printable());
			LOG(pdu.get_request_id());
			LOG_END;
			
			authenticationFailure(context_name, 
					      target.get_address(),
					      0);

			return 0;
		      }
		    }
		  }
		  
		} // end version3
		if (status == SNMP_ERROR_TOO_BIG) {

			v3mp->inc_stats_invalid_msgs();

			MibIIsnmpCounters::incInTooBigs();
			
			LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
			LOG("RequestList: too big SNMP PDU received (rid): ");
			LOG(pdu.get_request_id());
			LOG_END;

			pdu.set_vblist(0, 0);
			pdu.set_error_status(SNMP_ERROR_TOO_BIG);
			pdu.set_error_index(0);
			
			pdu.set_type(sNMP_PDU_RESPONSE);
				
			MibIIsnmpCounters::incOutPkts();
				
			snmp->send (pdu, &target);
			return 0;
		}
		// check for a proxy application
#ifdef _PROXY_FORWARDER
		if ((pdu.get_context_engine_id().len() > 0) &&
		    (pdu.get_context_engine_id() !=
		     v3mp->get_local_engine_id())) {

			LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
			LOG("RequestList: proxy request detected (contextEngineID)(rid): ");
			LOG(pdu.get_context_engine_id().get_printable());
			LOG(pdu.get_request_id());
			LOG_END;

			Request* req = new Request(pdu, target);
			return add_request(req);
		}
#endif	// _PROXY_FORWARDER	    
		int		  viewType;
		OctetStr	  viewName;
		
		// set access mode 
		switch (pdu.get_type()) {
		case sNMP_PDU_GET:
		case sNMP_PDU_GETNEXT:
		case sNMP_PDU_GETBULK: { viewType = mibView_read; break; }
		case sNMP_PDU_SET: { viewType = mibView_write; break; }
		default: {
			// sNMP_PDU_V1TRAP 
			// sNMP_PDU_RESPONSE
			// sNMP_PDU_INFORM 
			// sNMP_PDU_TRAP
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			LOG("Request: Don't know, how to handle PduType: ");
			LOG(pdu.get_type());
			LOG_END;
			v3mp->inc_stats_invalid_msgs();
			v3mp->inc_stats_unknown_pdu_handlers();
			MibIIsnmpCounters::incInASNParseErrs();
			return 0;
		}
		}
		// initialize viewName;
		int vacmErrorCode = 
		  vacm->getViewName(security_model, security_name,
				    security_level, viewType, 
				    context_name, viewName);
		Vbx v; 
		Oidx o;
		// check first VB of PDU 
		int i=0;
		pdu.get_vb(v,i);
		v.get_oid(o);
		// access control
		if (vacmErrorCode == VACM_viewFound) {
			vacmErrorCode = vacm->isAccessAllowed(viewName, o);
		}
		switch (vacmErrorCode) {
		case VACM_noSuchView:
		case VACM_noAccessEntry:
		case VACM_noGroupName: {
			// these errors can be handled here
			if (version == version3) {
				pdu.set_error_status(SNMP_ERROR_AUTH_ERR);
				pdu.set_error_index(0);
				pdu.set_type(sNMP_PDU_RESPONSE);
				
				LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
				LOG("RequestList: SNMPv3 auth failure:");
				LOG(vacm->getErrorMsg(vacmErrorCode));
				LOG_END;
			
				MibIIsnmpCounters::incOutPkts();
				
				snmp->send (pdu, &target);
			}
			else {
				MibIIsnmpCounters::incInBadCommunityNames();
			}

			authenticationFailure(context_name,
					      target.get_address(),
					      vacmErrorCode);
 
			return 0;
		}
		case VACM_otherError: {
		  
			pdu.set_error_status(SNMP_ERROR_GENERAL_VB_ERR);
			pdu.set_error_index(i+1);
			pdu.set_type(sNMP_PDU_RESPONSE);
			
			LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
			LOG("RequestList: SNMPv3 general error: (VBindex)");
                        LOG(i+1);
                        LOG(vacmErrorCode);
			LOG_END;

			MibIIsnmpCounters::incOutPkts();

			snmp->send(pdu, &target);
                        return 0;
		}
		case VACM_noSuchContext: {
                        // delete all vbs and set first vb to oid of counter
			// of unknown contextes
                        vacm->incUnknownContexts();
                        for (int j = 0; j<pdu.get_vb_count(); j++) 
                          pdu.delete_vb(0);
                        Vbx newvb = Vbx(oidSnmpUnknownContexts);
			newvb.set_value(vacm->getUnknownContexts());
                        pdu +=newvb;
                        pdu.set_type(sNMP_PDU_RESPONSE);
			// error index of -1 will be handled by req->answer()
			pdu.set_error_status(0);
			pdu.set_error_index(0);
                        
			LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
			LOG("RequestList: SNMPv3 (noSuchContext)(from)");
			LOG(context_name.get_printable());
			GenAddress genAddr;
			target.get_address(genAddr);
			LOG(genAddr.get_printable());
			LOG_END;
			
			snmp->report(pdu, target);
			return 0;
                }
		} //switch
			
#else // #ifdef _SNMPv3
                // access for GETNEXT and GETBULK can�t be checked here
         	// community_ok increments inBadCommunityUses
		if (!community_ok(pdu.get_type(), security_name)) {
			LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
			LOG("RequestList: add request: auth failure");
			LOG(from.get_printable());
			LOG(security_name.get_printable());
			LOG_END;

			authenticationFailure("",
					      target.get_address(),
					      0);

			MibIIsnmpCounters::incInBadCommunityNames();
			return 0;
		}
		else
#endif // _SNMPv3
		{
			Request* req = new Request(pdu, target);
#ifdef _SNMPv3
			// set vacm and initialize viewName
			req->init_vacm(vacm, viewName);
#endif
			// if request is to be ignored req will be 
			// deleted by add_request 
			return add_request(req);
		}
	} // end "if (status == SNMP_CLASS_SUCCESS)" begin "else"
	else {
		switch (status) {
		case SNMP_CLASS_TL_FAILED: {
			// just select timeout
			break;
		}
#ifdef _SNMPv3
		case SNMPv3_MP_PARSE_ERROR:
		case SNMP_CLASS_ERROR: 
		case SNMP_CLASS_ASN1ERROR: {
			MibIIsnmpCounters::incInASNParseErrs();
			break;
		}
		case SNMP_CLASS_BADVERSION: {
			MibIIsnmpCounters::incInBadVersions();
			break;
		}
                case SNMPv3_MP_UNKNOWN_PDU_HANDLERS: {
			v3mp->inc_stats_unknown_pdu_handlers(); 
			break;
		}
                case SNMPv3_MP_UNSUPPORTED_SECURITY_MODEL: {
			authenticationFailure("", target.get_address(),
				  SNMPv3_MP_UNSUPPORTED_SECURITY_MODEL);
			break;
                }
		case SNMPv3_MP_NOT_IN_TIME_WINDOW: {
			v3mp->inc_stats_invalid_msgs();
			authenticationFailure("", target.get_address(),
					      SNMPv3_MP_NOT_IN_TIME_WINDOW);
			break;
		}
		case SNMPv3_MP_DOUBLED_MESSAGE:
		case SNMPv3_MP_INVALID_MESSAGE:
		case SNMPv3_MP_UNKNOWN_MSGID: {
			v3mp->inc_stats_invalid_msgs();
			break;
		}
		case SNMPv3_MP_INVALID_ENGINEID:
			// RFC 3414 � 3.2 (3b),4: 
			// snmpInvalidMsgs must not be incremented
			break;
		case SNMPv3_USM_AUTHENTICATION_ERROR:
		case SNMPv3_USM_AUTHENTICATION_FAILURE: {
			authenticationFailure("",
					      target.get_address(),
					      status);
			break;
		}
		case SNMPv3_MP_MAX_ERROR:
#endif
/* this case is handled above
		case SNMP_ERROR_TOO_BIG: {
		}
*/
		default:
			MibIIsnmpCounters::incInASNParseErrs();
			break;		  
		}
	}	
	return 0;
}

Request* RequestList::add_request(Request* req)
TS_SYNCHRONIZED(
{
	unsigned long rid = req->get_pdu()->get_request_id();
	
	Request* dupl;
	// ignore request, if request_id is already known 
	if (((dupl = find_request_on_id(rid)) == 0) || 
	    (strcmp(dupl->from.get_printable(), 
		    req->from.get_printable()) != 0)) {
		
		req->set_transaction_id(next_transaction_id++);
		requests->add(req);
		return req;
	}
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 4);
	LOG("RequestList: add request: ignored");
	LOG(req->from.get_printable());
	LOG(dupl->from.get_printable());
	LOG(rid);
	LOG_END;

	delete req;
	return 0;
})

#ifndef _SNMPv3

void RequestList::set_read_community(const OctetStr& rc) 
{
	if (read_community) delete read_community;
	read_community = new OctetStr(rc);
}

void RequestList::set_write_community(const OctetStr& wc) 
{
	if (write_community) delete write_community;
	write_community = new OctetStr(wc);
}

#endif

void RequestList::authenticationFailure(const OctetStr& context,
					const GenAddress& sourceAddress,
					int status)
{
#ifdef _SNMPv3
	if (status == SNMPv3_MP_NOT_IN_TIME_WINDOW) {
		// do not report any time window failures by default
		return;
	}
#endif
	if ((snmpEnableAuthenTraps::instance) && 
	    (snmpEnableAuthenTraps::instance->
	     get_state() == 1)) {
		NotificationOriginator no;
		Vbx vbs[1];
		authenticationFailureOid authOid;
		no.generate(vbs, 0, authOid, "", context);
	}
}

#ifdef AGENTPP_NAMESPACE
}
#endif







