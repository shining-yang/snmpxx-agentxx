/*_############################################################################
  _## 
  _##  agentx_master.cpp  
  _## 
  _##
  _##  AgentX++ 2
  _##  -------------------------------------
  _##  Copyright (C) 2000-2014 - Frank Fock
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

#include <errno.h>
#include <signal.h>
#include <agentx_pp/agentx_master.h>
#include <agentx_pp/agentx_mib.h>
#include <agentx_pp/agentx_threads.h>
#include <agentx_pp/agentx_index.h>
#include <agent_pp/system_group.h>
#include <snmp_pp/log.h>

#ifdef WIN32
#include <io.h>
#endif 

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

static const char *loggerModuleName = "agentx++.agentx_master";


void AgentXResponseTask::run()
{
	mib->process_ax_response(*task, timely);
}

void AgentXTask::run()
{
	switch (task->get_agentx_type()) {
	    case AGENTX_REGISTER_PDU: {
		mib->process_ax_registration(*task);
		break;
	    }
	    case AGENTX_UNREGISTER_PDU: {
		mib->process_ax_unregistration(*task);
		break;
	    }
	}
}

AgentXCloseTask::~AgentXCloseTask() 
{
	if (peer)
		delete peer;
	if (closePdu)
		delete closePdu;
}

void AgentXCloseTask::run()
{
	if (peer) {
		mib->remove_peer(*peer);
		mib->agentx->lock_socket();
		AgentXPeer* p = mib->agentx->remove_peer(*peer);
		if (p) delete p;
		mib->agentx->unlock_socket();
		// actually close socket
#ifdef WIN32
		_close(peer->sd);
#else
		close(peer->sd);
#endif
	}
	if (closePdu) {
	        mib->remove_indexes(closePdu->get_session_id()); 
		if (agentxSessionEntry::instance)
			agentxSessionEntry::instance->
			  remove(closePdu->get_peer()->mibID,
				 closePdu->get_session_id());
		mib->remove_entries(closePdu->get_peer()->mibID, 
				    closePdu->get_session_id());
		mib->openSessions.lock();
		AgentXSession* s = 
		  mib->openSessions.get_session(closePdu->get_session_id());
		AgentXSession session;
		mib->openSessions.unlock();
		AgentXPdu* response = mib->create_response(*closePdu, session);
		if (!s) {
			if (response)
				response->set_error_status(AGENTX_NOT_OPEN);
		}
		if (response) { 
			mib->answer(*response);
			delete response;
            if (s) {
                s = mib->openSessions.remove(closePdu->get_session_id());
                if (s) {
                    delete s;
                }
            }
		}	  
	}
}

/*-------------------------- class AgentXThread -----------------------*/

void AgentXThread::run()
{
	AgentXMaster* agentx = master->get_agentx();
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
	LOG("AgentX Master Agent starting");
	LOG_END;
	while (!agentx->quit()) {
#ifdef AX_UNIX_SOCKET
		if (agentx->get_connect_mode() & AX_USE_UNIX_SOCKET) {
			if (!agentx->bind_unix()) {
				LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
				LOG("MasterAgentX: could not bind UNIX port. Aborting.");
				LOG_END;
#ifdef _NO_LOGGING
				raise(SIGTERM);
#endif
			}
		}
#endif
#ifdef AX_TCP_SOCKET
		if (agentx->get_connect_mode() & AX_USE_TCP_SOCKET) {
			if (!agentx->bind_tcp()) {
				LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
				LOG("MasterAgentX: could not bind TCP port. Aborting.");
				LOG_END;
#ifdef _NO_LOGGING
				raise(SIGTERM);
#endif
			}
		}		  
#endif
		while (!agentx->quit()) {
			master->process_agentx();
		}
	}
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
	LOG("AgentX Master Agent ended");
	LOG_END;
}

/*-------------------------- class MasterAgentXMib -----------------------*/


MasterAgentXMib::MasterAgentXMib(): Mib()
{
	agentx = 0;
	add(new agentx_mib());
	agentxSessionEntry::instance->set_master(this);
	axThread = 0;
	axRegThread = 0;
	axBulkThread = 0;
	axSetThread = 0;
	autoContext = FALSE;
}

MasterAgentXMib::MasterAgentXMib(const OctetStr& c): Mib(c)
{
	agentx = 0;
	add(new agentx_mib());
	agentxSessionEntry::instance->set_master(this);
	axThread = 0;
	axRegThread = 0;
	axBulkThread = 0;
	axSetThread = 0;
	autoContext = FALSE;
}

MasterAgentXMib::~MasterAgentXMib()
{
	// close thread pool for SNMP requests first
	// then close sessions and finally the agentX thread
	if (threadPool) {
		delete threadPool;    
		threadPool = 0;
	}
    if (agentx) {
        close_all_sessions();
        agentx->stopit = TRUE;
    }
	if (axThread) {
		delete axThread;
	}
	if (axRegThread) {
		delete axRegThread;
	}
	if (axBulkThread) 
		delete axBulkThread;
	if (axSetThread) 
		delete axSetThread;
}

void MasterAgentXMib::set_agentx(AgentXMaster* master) 
{
	agentx = master;
}

void MasterAgentXMib::set_session_admin_status_writable(bool writable)
{
	agentxSessionAdminStatus::set_writable(writable);
}

bool MasterAgentXMib::init()
{
	if (!axThread)
		axThread = new ThreadPool(1);
	if (!axRegThread)
		axRegThread = new QueuedThreadPool(1);
	if (!axBulkThread)
		axBulkThread = new QueuedThreadPool(2);
	if (!axSetThread)
		axSetThread = new QueuedThreadPool(1);
    
	axRegThread->start();
	axBulkThread->start();
	axSetThread->start();

	Mib::init();
	if (!agentx) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
		LOG("MasterAgentXMib: AgentX protocol services not set, aborting");
		LOG_END;
#ifdef _NO_LOGGING
		raise(SIGTERM);
#endif
		// not reached
		return false;
	}
	axThread->execute(new AgentXThread(this));
	return true;
}

void MasterAgentXMib::dispatch_ax_unregistration(AgentXPdu& request)
{
	AgentXTask* mt = 
	  new AgentXTask(this, new AgentXPdu(request));
	axRegThread->execute(mt);
}

void MasterAgentXMib::dispatch_ax_registration(AgentXPdu& request)
{
	AgentXTask* mt = 
	  new AgentXTask(this, new AgentXPdu(request));
	axRegThread->execute(mt);
}

void MasterAgentXMib::dispatch_ax_close(AgentXPeer* peer, AgentXPdu* closePdu)
{
	AgentXCloseTask* axCloseThread = 
	  new AgentXCloseTask(this, peer, closePdu);
	axRegThread->execute(axCloseThread);
}
	  

void MasterAgentXMib::dispatch_ax_response(AgentXPdu& response, 
					   bool timely)
{
	// synchronize requestList
	ThreadSynchronize* s = new ThreadSynchronize(*requestList);
	unsigned long tid = response.get_transaction_id();
	Request* request = requestList->get_request(tid);
	
	if (!request) {
		AgentXQueue* axQueue = agentx->get_queue();
		axQueue->lock();
		AgentXPdu* req = axQueue->find(response.get_packet_id());
		if (req) {
		    axQueue->remove(req);
		}
		axQueue->unlock();

		LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
		LOG("MasterAgentXMib: late response (tid)");
		LOG(tid);
		LOG_END;
		delete s;
		return;
	}
	AgentXResponseTask* mt = 
	  new AgentXResponseTask(this, new AgentXPdu(response), timely);
	if (request->get_type() == sNMP_PDU_SET) {
		delete s; // unlock requestList
		axSetThread->execute(mt);
	}
	else {
		delete s; // unlock requestList
		axBulkThread->execute(mt);
	}
}


MibEntry* MasterAgentXMib::add(MibEntry* entry)
{
	MibEntryPtr p = Mib::add(entry);
	if ((p) && (AgentXNode::is_agentx(*p))) {
		// added successfully AgentXNode
		((AgentXNode*)p)->set_back_reference(this);
	}
	return p;
}

MibEntry* MasterAgentXMib::add(const OctetStr& c, MibEntry* entry)
{
	MibEntryPtr p = Mib::add(c, entry);
	if ((p) && (AgentXNode::is_agentx(*p))) {
		// added successfully AgentXNode
		((AgentXNode*)p)->set_back_reference(this);
	}
	return p;
}

bool MasterAgentXMib::add_ax_agent_caps(const OctetStr& context, 
					   const Oidx& sysORID,
					   const OctetStr& sysORDescr,
					   AgentXSession& session)
{
	bool ok = Mib::add_agent_caps(context, sysORID, sysORDescr);
	if (ok) {
		session.add_agent_caps(context, sysORID);
	}
	return ok;
}

void MasterAgentXMib::remove_ax_agent_caps(const OctetStr& context, 
					   const Oidx& sysORID,
					   AgentXSession& session)
{
	Mib::remove_agent_caps(context, sysORID);
	session.remove_agent_caps(context, sysORID);
}

void MasterAgentXMib::add_get_subrequest(u_int sid, 
					 unsigned long tid, 
					 const AgentXSearchRange& range,
					 u_char timeout,
					 u_char type,
					 const OctetStr& context,
					 int repeater,
					 int repetitions)
{
    if (!agentx || agentx->stopit) {
        LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
        LOG("MasterAgentXMib: not adding get subrequest because AgentX is down (sid)(tid)");
        LOG(sid);
        LOG(tid);
        LOG_END;
        return;
    }
	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 3);
	LOG("MasterAgentXMib: adding get subrequest (sid)(tid)(lower)(upper)(incl)(timeout)(type)(context)(repeater)(repetitions)");
	LOG(sid);
	LOG(tid);
	LOG(Oidx(range.get_lower()).get_printable());
	LOG(Oidx(range.get_upper()).get_printable());
	LOG(range.is_lower_included());
	LOG(timeout);
    LOG(type);
	LOG(OctetStr(context).get_printable());
	LOG(repeater);
	LOG(repetitions);
	LOG_END;
    
	agentx->lock_queue();
	// look for any pending (not yet sent) requests for the same
    // session and transaction ID
	AgentXPdu* pending = agentx->get_queue()->find(sid, tid, TRUE);
	if (pending) {
		// AgentX request already created
		// type must be the same, because tid is unique
		if (type == AGENTX_GETBULK_PDU) {
		  if (repeater) {
		    ListCursor<AgentXSearchRange> cur(pending->get_ranges());
		    for (; cur.get(); cur.next()) {
			  LOG_BEGIN(loggerModuleName, DEBUG_LOG | 7);
			  LOG("MasterAgentXMib: matching bulk subrequest (sid)(tid)(ref)(match)");
			  LOG(sid);
			  LOG(tid);
			  LOG(range.get_reference());
			  LOG(cur.get()->get_reference());
			  LOG_END;
			
			  if (((range.get_reference() -
				cur.get()->get_reference()) % repeater) == 0) {
			    // just repetition -> ignore
			    agentx->unlock_queue();
			    return;
			  }
		    }
		  }
		  else 
			pending->inc_non_repeaters();
		}
		pending->add_range(range);
		if (timeout > pending->get_timeout()) {
			pending->set_timeout(timeout);
			// do not set timestamp here, will be done later
		}
		agentx->unlock_queue();
	}
	// only prepare and send new request if there are no pending
    // requests left  
	else if (!agentx->get_queue()->find(sid, tid, FALSE)) {
		agentx->unlock_queue();

		AgentXSession session;
		if (!openSessions.get_session(sid, session)) {
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 2);
			LOG("MasterAgentXMib: session lookup failed (sid)");
			LOG(sid);
			LOG_END;
			return; // let the request timeout
		}
		pending = new AgentXPdu(session.get_byte_order(), type);
		pending->set_session_id(sid);
		pending->set_transaction_id(tid);
		pending->add_range(range);
		pending->set_context(context);
		pending->set_timeout(timeout);
		// do not set timestamp here, will be done later

		if (type == AGENTX_GETBULK_PDU) {
			if (repeater)
				pending->set_max_repetitions(repetitions);
			else
				pending->inc_non_repeaters();
		}
        if (!agentx->stopit) {
            agentx->get_queue()->add(pending); // synchronized

            LOG_BEGIN(loggerModuleName, EVENT_LOG | 4);
            LOG("MasterAgentXMib: queued AgentX subrequest (sid)(tid)(type)(timeout)");
            LOG(sid);
            LOG(tid);
            LOG(type);
            LOG(timeout);
            LOG_END;
        }
        else {
            delete pending;
            LOG_BEGIN(loggerModuleName, WARNING_LOG | 4);
            LOG("MasterAgentXMib: AgentX subrequest not queued because AgentX shutdown pending (sid)(tid)(type)");
            LOG(sid);
            LOG(tid);
            LOG(type);
            LOG_END;            
        }
	}
	else {
		agentx->unlock_queue();
		
		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 2);
		LOG("MasterAgentXMib: did not prepare new request, because there is still one in queue (sid)(tid)");
		LOG(sid);
		LOG(tid);
		LOG_END;
	}
}

void MasterAgentXMib::add_set_subrequest(u_int sid, unsigned long tid,
					 const AgentXSearchRange& range,
					 const Vbx& vb, 
					 u_char timeout,
					 u_char type,
					 const OctetStr& context)
{
	agentx->lock_queue();
	// look for any pending (not yet sent) requests for the same
        // session and transaction ID
	AgentXPdu* pending = agentx->get_queue()->find(sid, tid, TRUE);
	if (pending) {
		// AgentX request already created
		// type must be the same, because tid is unique
		*pending += vb;
		pending->add_range(range);
		if (timeout > pending->get_timeout()) {
			pending->set_timeout(timeout);
			// do not set timestamp here, will be done later
		}
		agentx->unlock_queue();
	}
	else {
		agentx->unlock_queue();

		AgentXSession session;
		if (!openSessions.get_session(sid, session)) {
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 2);
			LOG("MasterAgentXMib: session lookup failed (sid)");
			LOG(sid);
			LOG_END;
			return; // let the request timeout
		}
		pending = new AgentXPdu(session.get_byte_order(), type);
		pending->add_range(range);
		pending->set_timeout(session.get_timeout());
		pending->set_session_id(sid);
		pending->set_transaction_id(tid);
		pending->set_context(context);
		pending->set_timeout(timeout);
		// do not set timestamp here, will be done later
		*pending += vb;
        if (!agentx->stopit) {
            agentx->get_queue()->add(pending); // synchronized

            LOG_BEGIN(loggerModuleName, EVENT_LOG | 4);
            LOG("MasterAgentXMib: queued AgentX subrequest (sid)(tid)(type)");
            LOG(sid);
            LOG(tid);
            LOG(type);
            LOG_END;
        }
        else {
            delete pending;
            LOG_BEGIN(loggerModuleName, WARNING_LOG | 4);
            LOG("MasterAgentXMib: AgentX subrequest not queued because AgentX shutdown pending (sid)(tid)(type)");
            LOG(sid);
            LOG(tid);
            LOG(type);
            LOG_END;            
        }        
	}
}


void MasterAgentXMib::process_set_request(Request* req)
{
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	LOG("MasterAgentXMib: processing set request (tid)(phase)(oid)...");
	LOG(req->get_transaction_id());
	LOG(req->phase);	
	for (int j=0; j<req->subrequests(); j++) 
		LOG(req->get_oid(j).get_printable());
	LOG_END;

	switch (req->phase) {
	case PHASE_DEFAULT: {
		req->phase = PHASE_PREPARE;
		if (process_prepare_set_request(req) != SNMP_ERROR_SUCCESS) {
			agentx->get_queue()->cancel(req->get_transaction_id());
		}
		if (agentx->get_queue()->
		    pending(req->get_transaction_id())) 
			break;
	}
	case PHASE_PREPARE: {
		if (req->get_error_status() == SNMP_ERROR_SUCCESS) {
			if (!agentx->get_queue()->
			    pending(req->get_transaction_id())) {
				// prepare phase successfully finished
				req->phase = PHASE_COMMIT;
				process_commit_set_request(req);
				// do not break -> if no AgentX subrequest
				// process directly UNDO
				req->inc_outstanding(); // wait for AgentX resp
			}
		}
		else {
			req->phase = PHASE_CLEANUP;
			process_cleanup_set_request(req);
			req->no_outstanding();
			break;
		}
	}
	case PHASE_COMMIT: {
		if (!agentx->get_queue()->
		    pending(req->get_transaction_id())) {
			if (req->get_error_status() != SNMP_ERROR_SUCCESS) {
				// commit failed
				req->phase = PHASE_UNDO;
				process_undo_set_request(req);
			}
			else {
				req->phase = PHASE_CLEANUP;
				process_cleanup_set_request(req);
				req->no_outstanding();
				break;
			}
		}
		else break;
	}
	case PHASE_UNDO: {
		if (!agentx->get_queue()->
		    pending(req->get_transaction_id())) {
			req->no_outstanding();
		}
		// wait for undoset response
		else if (req->finished()) req->inc_outstanding();
		break;
	}
	case PHASE_CLEANUP:
		req->no_outstanding();
		break;
	}
}


bool MasterAgentXMib::cancel_pending_ax(unsigned long tid)
{
	bool count = false;
	agentx->lock_queue();
	
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
	LOG("MasterAgentXMib: canceling AgentX subrequests (tid)");
	LOG(tid);
	LOG_END;

	OrderedListCursor<AgentXPdu> cur(agentx->get_queue()->elements());
	for (; cur.get(); cur.next()) {
		AgentXPdu* pdu = cur.get();
		if ((pdu->get_transaction_id() == tid) &&
		    (pdu->get_timestamp() == 0)) {
			count = true;
			// not yet sent
			AgentXSession session;
			if (!openSessions.get_session(pdu->get_session_id(),
						      session)) continue;  
				//let request time out
			
			time_t t = AgentX::compute_timeout(pdu->get_timeout());
			pdu->set_timestamp(t);
			pdu->set_time(sysUpTime::get());

      			agentx->send_agentx(session.get_peer().sd, *pdu);
		}
	}	
	agentx->unlock_queue();
	return count;
}

bool MasterAgentXMib::send_pending_ax(unsigned long tid)
{
	int count = 0;
	agentx->lock_queue();
	
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
	LOG("MasterAgentXMib: sending AgentX subrequests (tid)");
	LOG(tid);
	LOG_END;

	OrderedListCursor<AgentXPdu> cur(agentx->get_queue()->elements());
	for (; cur.get(); ) {
		AgentXPdu* pdu = cur.get();
		if ((pdu->get_transaction_id() == tid) &&
		    (pdu->get_timestamp() == 0)) {
			count++;
			// not yet sent
			AgentXSession session;
			if (!openSessions.get_session(pdu->get_session_id(),
						      session)) continue;  
				//let request time out
			
			time_t t = AgentX::compute_timeout(pdu->get_timeout());
			pdu->set_timestamp(t);

			agentx->send_agentx(session.get_peer().sd, *pdu);
			// fire off cleanup -> do not wait for response 
			if (pdu->get_agentx_type() == AGENTX_CLEANUPSET_PDU) {
				cur.next();
				agentx->get_queue()->remove(pdu);
				continue;
			}
		}
		cur.next();
	}	
	agentx->unlock_queue();

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
	LOG("MasterAgentXMib: sent AgentX subrequests (tid)(count)");
	LOG(tid);
	LOG(count);
	LOG_END;

	return (count > 0);
} 



void MasterAgentXMib::finalize(Request* req) 
{
	if (req->finished()) {
		if (req->get_type() != sNMP_PDU_SET) {
			agentx->get_queue()->cancel(req->get_transaction_id());
		}
		else 
			send_pending_ax(req->get_transaction_id());
		Mib::finalize(req);
	}
	else if ((!send_pending_ax(req->get_transaction_id())) &&
		 (!agentx->get_queue()->pending(req->get_transaction_id()))) {
		Mib::finalize(req);
	}
	else {
		if (req->trylock() != Synchronized::BUSY) {
            req->notify();
            req->unlock();
        }
	}
}

void MasterAgentXMib::process_agentx() 
{
	fd_set         readFDS;
	int            nfds;
	struct timeval timeout;
	
	time_t t;
	time(&t);
	if ((t = timeout_requests(t)) == 0) {
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
	}
	else if (t > 0) {
		timeout.tv_sec = t;
		timeout.tv_usec = 0;
	}
    else {
        return;
    }

	nfds = 0;
	int maxfd = agentx->set_file_descriptors(&readFDS);

	if ((nfds = select(maxfd, &readFDS, NULL, NULL, &timeout)) < 0) {
	  switch (errno) {
	    case EBADF: {
		if (!agentx->check_peer_closed(&readFDS)) {
			LOG_BEGIN(loggerModuleName, WARNING_LOG | 2);
			LOG("AgentX: select() returned EBADF but no peers have disconnected" );
			LOG_END;
			return;
		} 
		else {
			LOG_BEGIN(loggerModuleName, WARNING_LOG | 2);
			LOG("AgentX: peer disconnected without sending close PDU");
			LOG_END;
			return;
		}
	    } 
	  case EINTR:
	    // ignore
	    return;
	  default: {
			LOG_BEGIN(loggerModuleName, WARNING_LOG | 4);
			LOG("AgentX: unknown select() error (errno)");
			LOG(errno);
			LOG_END;
			return;
	    }
	  }
	}
	else if (nfds == 0) {
		// timeout without request
	}
	else {
	  //if (agentx->quit()) return;
	  LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
	  LOG("MasterAgentX: received something on socket");
	  LOG_END;

	  if (nfds > 0)  process_ax_master_request(&readFDS, &nfds);
	  if (nfds > 0)  agentx->connect_request(&readFDS, &nfds);
	  if (nfds > 0) {
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 3);
		LOG("MasterAgentX: unknown socket descriptor ready");
		LOG_END;
	  }
	}
}

AgentXMaster* MasterAgentXMib::get_agentx()
{
	return agentx;
}

void MasterAgentXMib::remove_peer(const AgentXPeer& peer) 
{
	if (agentxConnectionEntry::instance) {
		agentxConnectionEntry::instance->remove(peer.mibID);
	}
	// remove registrations
	List<AgentXSession>* list = 
	  openSessions.remove_sessions_of_peer(peer);
	ListCursor<AgentXSession> s;
	for (s.init(list); s.get(); s.next()) {
		remove_agent_caps_by_session(s.get());
		remove_indexes(s.get()->get_id());
		if (agentxSessionEntry::instance)
		  agentxSessionEntry::instance->remove(peer.mibID, 
						       s.get()->get_id());
		remove_entries(peer.mibID, s.get()->get_id());
	}
	// delete sessions 
	delete list;
}

void MasterAgentXMib::process_ax_master_request(fd_set *fds, int* nfds)
{
	agentx->lock_peers();
	ListCursor<AgentXPeer> cur = agentx->peers();
	while ((*nfds > 0) && (cur.get())) {
		if (FD_ISSET(cur.get()->sd, fds)) {

		  LOG_BEGIN(loggerModuleName, DEBUG_LOG | 5);
		  LOG("MasterAgentXMib: reading AgentX packet");
		  LOG_END;
		  (*nfds)--;
		  AgentXPdu req;
		  int status;
		  if ((status = agentx->receive_agentx(cur.get()->sd, req)) != 
		      AGENTX_SUCCESS) {

			LOG_BEGIN(loggerModuleName, WARNING_LOG | 3);
			LOG("MasterAgentXMib: lost connection (sd)(err)");
			LOG(cur.get()->sd);
			LOG(status);
			LOG_END;
			
			// delete peer from list
			cur.get()->closing = TRUE;
			AgentXPeer* victim = new AgentXPeer(*cur.get());
			cur.next();
			dispatch_ax_close(victim, 0);			
			continue;
		  }
		  // set the request's peer
		  req.set_peer(cur.get());

		  LOG_BEGIN(loggerModuleName, DEBUG_LOG | 4);
		  LOG("MasterAgentXMib: processing AgentX packet from (sd)(type)");
		  LOG(cur.get()->sd);
		  LOG(req.get_agentx_type());
		  LOG_END;

		  // process it!
		  process_ax_request(req);
		}
		cur.next();
	}
	agentx->unlock_peers();
}
		
AgentXPdu* MasterAgentXMib::create_response(AgentXPdu& req, 
					    AgentXSession& session) 
{
	AgentXPdu* response = new AgentXPdu();

	response->set_agentx_type(AGENTX_RESPONSE_PDU);
	// TODO: set the sysUpTime depending on the request`s context
	response->set_time(sysUpTime::get());
	response->set_packet_id(req.get_packet_id());
	response->set_transaction_id(req.get_transaction_id());
	response->set_peer(req.get_peer());
	response->set_session_id(req.get_session_id());

	if (!(openSessions.get_session(req.get_session_id(), session))) {
		response->set_error_status(AGENTX_NOT_OPEN);
		answer(*response);
		delete response;
		return 0;
	}
	/* We support non default contexts
	else if (req.non_default_context()) {
		response->set_error_status(AGENTX_UNSUPPORTED_CONTEXT);
		answer(*response);
		delete response;
		return 0;
	}	
	*/
	// TODO: lock_mib() here, if contexts are removed at runtime 
	else if ((!autoContext) && (!get_context(req.get_context()))) {
		response->set_error_status(AGENTX_UNSUPPORTED_CONTEXT);
		answer(*response);
		delete response;
		return 0;
	}
	return response;
}	


void MasterAgentXMib::process_ax_request(AgentXPdu& req) 
{
	switch (req.get_agentx_type()) {
	case AGENTX_OPEN_PDU: {
		process_ax_open(req);
		break;
	}
	case AGENTX_REGISTER_PDU: {
		dispatch_ax_registration(req);
		break;
	}
	case AGENTX_UNREGISTER_PDU: {
		dispatch_ax_unregistration(req);
		break;
	}
	case AGENTX_NOTIFY_PDU: {
		process_ax_notify(req);
		break;
	}
	case AGENTX_PING_PDU: {
		AgentXSession session;
		AgentXPdu* response = create_response(req, session);
		if (response) {
			response->set_error_status(SNMP_ERROR_SUCCESS);
			answer(*response);
			delete response;
		}
		break;
	}
	case AGENTX_CLOSE_PDU: {
		dispatch_ax_close(0, new AgentXPdu(req));
		break;
	}
	case AGENTX_ADDAGENTCAPS_PDU: {
		process_ax_addagentcaps(req);
		break;
	}
	case AGENTX_REMOVEAGENTCAPS_PDU: {
		process_ax_removeagentcaps(req);
		break;
	}
	case AGENTX_RESPONSE_PDU: {		
		dispatch_ax_response(req, TRUE);
		break;
	}
	case AGENTX_INDEXALLOCATE_PDU: {
		process_ax_indexallocate(req);
		break;
	}
	case AGENTX_INDEXDEALLOCATE_PDU: {
		process_ax_indexdeallocate(req);
		break;
	}
	default: {
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
		LOG("MasterAgentXMib: PDU (type) not implemented");
		LOG(req.get_agentx_type());
		LOG_END;
	}
	}
}

void MasterAgentXMib::answer(AgentXPdu& resp) 
{
	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 3);
	LOG("MasterAgentXMib: sending response (tid)(peer)(err)");
	LOG(resp.get_transaction_id());
	LOG(resp.get_peer()->sd);
	LOG(resp.get_error_status());
	LOG_END;
	int status = agentx->send_agentx(resp.get_peer()->sd ,resp);
	if (status != AGENTX_SUCCESS) {
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
		LOG("MasterAgentXMib: could not send response (tid)(peer)");
		LOG(resp.get_transaction_id());
		LOG(resp.get_peer()->sd);
		LOG_END;
	}
	else {

		LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
		LOG("MasterAgentXMib: sent response (tid)(peer)(uptime)");
		LOG(resp.get_transaction_id());
		LOG(resp.get_peer()->sd);
		LOG(resp.get_time());
		LOG_END;
	}	
}

void MasterAgentXMib::process_ax_open(AgentXPdu& req) 
{
	u_int sid = 
	  openSessions.make_new_session(req.network_byte_order(),
					req.get_timeout(),
					req.get_id(),
					req.get_descr(),
					*req.get_peer());
	// feed AgentX MIB
	if (agentxSessionEntry::instance)
		agentxSessionEntry::instance->add(req.get_peer()->mibID,
						  sid, req.get_id(),
						  req.get_descr(),
						  1,
						  req.get_timeout());
	req.set_session_id(sid);
	AgentXSession session;
	AgentXPdu* response = create_response(req, session);	
	req.get_peer()->nSessions++;

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 5);
	LOG("MasterAgentXMib: opened new session (id)(peer)(sessionCount)");
	LOG(sid);
	LOG(req.get_peer()->sd);
	LOG(req.get_peer()->nSessions);
	LOG_END;

	answer(*response);
	delete response;
}

void MasterAgentXMib::process_ax_registration(AgentXPdu& req)
{
	AgentXSession session;
	AgentXPdu* response = create_response(req, session);
	if (!response) return;
	// create new reg entry
	u_char timeout = req.get_timeout();
	if (timeout == 0) {
		// use session timeout
		timeout = session.get_timeout();
		if (timeout == 0) 
			timeout = AGENTX_DEFAULT_TIMEOUT;
	}
	AgentXRegEntry* newReg = 
	  new AgentXRegEntry(session.get_id(), req.get_region(), 
			     req.get_priority(),
			     req.get_region().get_lower().len(),
			     req.get_context(),
			     timeout);
	// check for duplicate registration
	if (is_dupl_reg(*newReg)) {
		response->set_error_status(AGENTX_DUPLICATE_REGISTRATION);
		answer(*response);
		delete response;
		delete newReg;
		return;
	}
	else if (has_conflict(*newReg)) {
		response->set_error_status(AGENTX_DUPLICATE_REGISTRATION);
		answer(*response);
		delete response;
		delete newReg;
		return;
	}
	response->set_error_status(AGENTX_OK);
	answer(*response);
	delete response;
	add_entry(newReg);

	if (agentxRegistrationEntry::instance)
		newReg->mibID = 
		  agentxRegistrationEntry::instance->
		  add(req.get_peer()->mibID, req.get_session_id(),
		      newReg->context,
		      newReg->region.get_lower(), newReg->region.get_subid(),
		      newReg->region.get_upper_bound(), newReg->priority,
		      newReg->timeout, newReg->region.is_single());	
}

void MasterAgentXMib::process_ax_unregistration(AgentXPdu& req)
{
	AgentXSession session;
	AgentXPdu* response = create_response(req, session);
	if (!response) return;
	// create new reg entry
	AgentXRegEntry* newReg = 
	  new AgentXRegEntry(session.get_id(), req.get_region(), 
			     req.get_priority(),
			     req.get_region().get_lower().len(),
			     req.get_context(), 0);
	
	if (!remove_entry(session.get_peer().mibID, *newReg)) {
		response->set_error_status(AGENTX_UNKNOWN_REGISTRATION);
	}
	else {
		response->set_error_status(AGENTX_OK);
	}
	answer(*response);
	delete response;
	delete newReg;
}

void MasterAgentXMib::process_ax_notify(AgentXPdu& req)
{
	int sz = req.get_vb_count();
	Vbx* vbs = new Vbx[sz];
	req.get_vblist(vbs, sz);

	AgentXSession session;
	AgentXPdu* response = create_response(req, session);	
	if (!response) {
		delete[] vbs;
		return;
	}
	response->set_vblist(vbs, sz);

	int payload = 1; // min. trap oid
	Oidx trapoid;
	TimeTicks timestamp(sysUpTime::get());

	if (sz >= 1) {
		if (vbs[0].get_oid() == SNMP_SYSUPTIME_OID) {
			payload++;
			if ((sz < 2) || (vbs[1].get_oid() != SNMP_TRAP_OID)) {
			  response->set_error_status(AGENTX_PROCESSING_ERROR);
			  response->set_error_index(2);
			}
			else {
				vbs[0].get_value(timestamp);
				vbs[1].get_value(trapoid);
			}
		}
		else if (vbs[0].get_oid() != SNMP_TRAP_OID) {
			  response->set_error_status(AGENTX_PROCESSING_ERROR);
			  response->set_error_index(1);
		}
		else {
		  vbs[0].get_value(trapoid);
		}
	}

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
	LOG("MasterAgentXMib: notify from (sid)(oid)(vbs)(timestamp)(err)");
	LOG(req.get_session_id());
	LOG(trapoid.get_printable());
	LOG(sz-payload);
	LOG(timestamp.get_printable());
	LOG(response->get_error_status());
	LOG_END;

	if (response->get_error_status() == AGENTX_OK) {
		notify(req.get_context(), trapoid, 
		       vbs+payload, sz-payload, 
		       (unsigned int)timestamp);
	}

	answer(*response);
	delete response;	
	delete[] vbs;
}

void MasterAgentXMib::process_ax_addagentcaps(AgentXPdu& req)
{
	openSessions.lock();
	AgentXSession* s = openSessions.get_session(req.get_session_id());
	if (!s) {
	    openSessions.unlock();
	    LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
	    LOG("MasterAgentXMib: cannot add agent caps - no session (sid)");
	    LOG(req.get_session_id());
	    LOG_END;

	    // send response
	    AgentXSession session;
	    AgentXPdu* response = create_response(req, session);
	    if (response) { 
		// this part should not be executed in any case!
		answer(*response);
		delete response;
	    }	    
	    return;
	}
	Vbx vb;
	req.get_vb(vb, 0);	
	OctetStr descr;
	vb.get_value(descr);

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
	LOG("MasterAgentXMib: adding agent caps from (sid)(id)(descr)");
	LOG(req.get_session_id());
	LOG(vb.get_oid().get_printable());
	LOG(descr.get_printable());
	LOG_END;
	
	// TODO: check returning an error?
	add_ax_agent_caps(req.get_context(), vb.get_oid(), descr, *s);
	s = 0;
	openSessions.unlock();
	
	// send response
	AgentXSession session;
	AgentXPdu* response = create_response(req, session);
	if (response) {
		answer(*response);
		delete response;
	}
}

void MasterAgentXMib::process_ax_removeagentcaps(AgentXPdu& req)
{
	openSessions.lock();
	AgentXSession* s = openSessions.get_session(req.get_session_id());
	if (!s) {
	    openSessions.unlock();
	    LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
	    LOG("MasterAgentXMib: cannot remove agent caps - no session (sid)");
	    LOG(req.get_session_id());
	    LOG_END;

	    // send response
	    AgentXSession session;
	    AgentXPdu* response = create_response(req, session);
	    if (response) { 
            // this part should not be executed in any case!
            answer(*response);
            delete response;
	    }	    
	    return;
	}

	Vbx vb;
	req.get_vb(vb, 0);	


	LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
	LOG("MasterAgentXMib: removing agent caps from (sid)(id)");
	LOG(req.get_session_id());
	LOG(vb.get_oid().get_printable());
	LOG_END;
	
	remove_ax_agent_caps(req.get_context(), vb.get_oid(), *s);
	s = 0;
	openSessions.unlock();
	
	// send response
	AgentXSession session;
	AgentXPdu* response = create_response(req, session);
	if (response) {
		answer(*response);
		delete response;
	}
}

AgentXIndexDB* MasterAgentXMib::get_index_database(const OctetStr& c)
{
	Oidx context(Oidx::from_string(c, FALSE));
	AgentXIndexDB* db = indexDatabase.find(&context);
	if ((!db) && (get_context(c))) {
		db = indexDatabase.add(new AgentXIndexDB(context));
	}
	else if ((db) && (!get_context(c))) {
		indexDatabase.remove(&context);
		db = 0;
	}
	return db;
}

void MasterAgentXMib::process_ax_indexallocate(AgentXPdu& req)
{
	int sz = req.get_vb_count();
	Vbx* vbs = new Vbx[sz];
	req.get_vblist(vbs, sz);

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	LOG("MasterAgentXMib: index allocation (sid)(tid)[(oid)(value)...]");
	LOG(req.get_session_id());
	LOG(req.get_transaction_id());
	for (int j=0; j<sz; j++) {
	  LOG(vbs[j].get_printable_oid());
	  LOG(vbs[j].get_printable_value());
	}
	LOG_END;

	AgentXSession session;
	AgentXPdu* response = create_response(req, session);	
	if (!response)
	{
	  delete[] vbs;
	  return;
	}

	// get index db
	AgentXIndexDB* db = get_index_database(req.get_context());
	if (!db) {
		response->set_error_status(AGENTX_UNSUPPORTED_CONTEXT);
		response->set_vblist(vbs, sz);
		answer(*response);
		delete response;
		delete[] vbs;
		return;
	}

	// prepare allocation
	int status = AGENTX_OK;
	u_int sid = req.get_session_id();
	int i=0;
	for (i=0; (status == AGENTX_OK) && (i<sz); i++) {
		if (req.get_flags() & AGENTX_NEW_INDEX) {
			status = db->new_index(sid, vbs[i], TRUE);
		}
		else if (req.get_flags() & AGENTX_ANY_INDEX) {
			status = db->any_index(sid, vbs[i], TRUE);
		}
		else {
			status = db->allocate(sid, vbs[i], TRUE);
		}
	}
	if (status == AGENTX_OK) {
		// commit allocation
	  for (i=0; (status == AGENTX_OK) && (i<sz); i++) {
		if (req.get_flags() & AGENTX_NEW_INDEX) {
			status = db->new_index(sid, vbs[i], FALSE);
		}
		else if (req.get_flags() & AGENTX_ANY_INDEX) {
			status = db->any_index(sid, vbs[i], FALSE);
		}
		else {
			status = db->allocate(sid, vbs[i], FALSE);
		}
	  }
	  response->set_vblist(vbs, sz);	  
	  if (status != AGENTX_OK) 
		response->set_error_index(i);
	  response->set_error_status(status);
	}
	else {
		response->set_vblist(vbs, sz);
		response->set_error_index(i);
		response->set_error_status(status);
	}
	// send response
	answer(*response);
	delete response;
	delete[] vbs;
}

void MasterAgentXMib::process_ax_indexdeallocate(AgentXPdu& req)
{
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	LOG("MasterAgentXMib: index deallocation (sid)(tid)(sz)");
	LOG(req.get_session_id());
	LOG(req.get_transaction_id());
	LOG(req.get_vb_count());
	LOG_END;

	int sz = req.get_vb_count();
	Vbx* vbs = new Vbx[sz];
	req.get_vblist(vbs, sz);

	AgentXSession session;
	AgentXPdu* response = create_response(req, session);	
	if (!response) {
		delete[] vbs;
		return;
	}
	// get index db
	AgentXIndexDB* db = get_index_database(req.get_context());
	if (!db) {
		response->set_error_status(AGENTX_UNSUPPORTED_CONTEXT);
		response->set_vblist(vbs, sz);
		answer(*response);
		delete response;
		delete[] vbs;
		return;
	}

	// test deallocation
	int status = AGENTX_OK;
	u_int sid = req.get_session_id();
	int i=0;
	for (i=0; (status == AGENTX_OK) && (i<sz); i++) {
		status = db->release(sid, vbs[i], TRUE);
	}
	if (status == AGENTX_OK) {
	  // commit deallocation
	  for (i=0; (i<sz); i++) {
		db->release(sid, vbs[i], FALSE);
	  }
	  response->set_error_status(status);
	  response->set_vblist(vbs, sz);	  
	}
	else {
		response->set_error_index(i-1);
		response->set_error_status(status);
		response->set_vblist(vbs, sz);
	}
	// send response
	answer(*response);
	delete response;
	delete[] vbs;
}

void MasterAgentXMib::process_ax_response(AgentXPdu& response, bool timely)
{
    if (!agentx || agentx->stopit) {
        LOG_BEGIN(loggerModuleName, EVENT_LOG | 4);
        LOG("MasterAgentXMib: processing response aborted because AgentX protocol is not available (sd)(sid)(tid)(pid)(status)(vbs)");
        LOG(response.get_peer() ? response.get_peer()->sd : -1);
        LOG(response.get_session_id());
        LOG(response.get_transaction_id());
        LOG(response.get_packet_id());
        LOG(response.get_error_status());
        LOG(response.get_vb_count());
        LOG_END;        
        return;
    }
	agentx->lock_queue();
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 4);
	LOG("MasterAgentXMib: processing response (sd)(sid)(tid)(pid)(status)(vbs)");
	LOG(response.get_peer() ? response.get_peer()->sd : -1);
	LOG(response.get_session_id());
	LOG(response.get_transaction_id());
	LOG(response.get_packet_id());
	LOG(response.get_error_status());
	LOG(response.get_vb_count());
	LOG_END;
	
	AgentXPdu* ax_req = 
	    agentx->get_queue()->find(response.get_packet_id());
	if (!ax_req) {
		agentx->unlock_queue();

		LOG_BEGIN(loggerModuleName, INFO_LOG | 2);
		LOG("MasterAgentXMib: late response (sd)(sid)(tid)(pid)");
		LOG(response.get_peer() ? response.get_peer()->sd : -1);
		LOG(response.get_session_id());
		LOG(response.get_transaction_id());
		LOG(response.get_packet_id());
		LOG_END;
		return;
	}
	if (timely) {
		// timely response so reset session's timeout counter
		openSessions.lock();
		AgentXSession* session = 
		  openSessions.get_session(response.get_session_id());
		if (session) session->reset_timeouts();
		openSessions.unlock();
	}
	if (((ax_req->get_agentx_type() == AGENTX_GET_PDU) ||
	     (ax_req->get_agentx_type() == AGENTX_GETNEXT_PDU)) &&
	    (ax_req->get_range_count() != response.get_vb_count()) &&
	    (response.get_error_status() !=  SNMP_ERROR_GENERAL_VB_ERR)) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("MasterAgentXMib: invalid AgentX response (sd)(sid)(tid)(req-vbs)(resp-vbs)");
		LOG(response.get_peer() ? response.get_peer()->sd : -1);
		LOG(response.get_session_id());
		LOG(response.get_transaction_id());
		LOG(ax_req->get_range_count());
		LOG(response.get_vb_count());
		LOG_END;
		agentx->get_queue()->remove(ax_req);
		agentx->unlock_queue();
		
		// synchronize requestList
		Lock* s = new Lock(*requestList);
		Request* request = 
		  requestList->get_request(response.get_transaction_id());
		while (request) {
			if (request->trylock() == Synchronized::BUSY) {
				s->wait(10);
                request = requestList->get_request(response.get_transaction_id());
			}
			else {
				break;
			}
		}
		delete s;
		if (request) {
			request->error(request->first_pending(), 
				       SNMP_ERROR_GENERAL_VB_ERR);
			Mib::finalize(request);
			// request is unlocked and no longer valid
		}
		return;
	}
	AgentXPdu search(*ax_req);
	// we have found the response so delete it from queue
	agentx->get_queue()->remove(ax_req);
	agentx->unlock_queue();
	
	int reqtype = search.get_agentx_type();

	int* refs = new int[search.get_range_count()];
	ListCursor<AgentXSearchRange> cur = search.get_ranges();
	for (int i=0; cur.get(); cur.next(), i++) {
		refs[i] = cur.get()->get_reference();
	}

    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 7);
	LOG("MasterAgentXMib: synchronizing with request list (tid)");
	LOG(response.get_transaction_id());
	LOG_END;

	// synchronize requestList
	Lock* s = new Lock(*requestList);
	Request* request = 
	  requestList->get_request(response.get_transaction_id());
	while (request) {
	    if (request->trylock() == Synchronized::BUSY) {
            s->wait(10);
            request = requestList->get_request(response.get_transaction_id());
	    }
	    else {
            LOG_BEGIN(loggerModuleName, DEBUG_LOG | 8);
            LOG("MasterAgentXMib: trylock request succeeded (tid)(req-id)");
            LOG(response.get_transaction_id());
            LOG(request->get_request_id());
            LOG_END;
            break;
	    }
	}
	delete s; // unlock requestList before reprocessing 

	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
	LOG("MasterAgentXMib: processing response (sd)(sid)(tid)(pid)(reqtype)");
	LOG(response.get_peer() ? response.get_peer()->sd : -1);
	LOG(response.get_session_id());
	LOG(response.get_transaction_id());
	LOG(response.get_packet_id());
	LOG(reqtype);
	LOG_END;

	if (!request) {
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 0);
		LOG("MasterAgentXMib: no SNMP request for (tid)");
		LOG(response.get_session_id());
		LOG_END;
		delete[] refs;
		return;
	}
	// now we need to make sure that the request is
	if ((reqtype == AGENTX_TESTSET_PDU) ||
	    (reqtype == AGENTX_COMMITSET_PDU) ||
	    (reqtype == AGENTX_UNDOSET_PDU) ||
	    (reqtype == AGENTX_CLEANUPSET_PDU)) {
		process_ax_set_response(response, search, refs, request);
		delete[] refs;
	}
	else {
	  int bindings = response.get_vb_count();
	  if (response.get_error_status() != SNMP_ERROR_SUCCESS) {
		Vbx vb;
		int errind = response.get_error_index();
		if ((errind>0) && (errind<=bindings)) {
			response.get_vb(vb, errind-1);
			request->error(refs[errind-1], 
				       response.get_error_status());
		}
		else {
			request->error(request->first_pending(), 
				       SNMP_ERROR_GENERAL_VB_ERR);
		}
	  }
	  else {
		// handle GETBULK or a AgentX GET on a GETBULK request
		if ((reqtype == AGENTX_GETBULK_PDU) ||
		    ((reqtype == AGENTX_GET_PDU) &&
		     (request->get_type() == sNMP_PDU_GETBULK))) {
			if (process_ax_getbulk_response(response, search, 
							refs, request)) {
				// request was reprocessed ->
				// pointer to request may be invalid!!!
				delete[] refs;
				return;
			}
		}
		// handle GETNEXT or a AgentX GET on a GETNEXT/GETBULK request
		else if ((reqtype == AGENTX_GETNEXT_PDU) ||
			 ((reqtype == AGENTX_GET_PDU) &&
			  (request->get_type() == sNMP_PDU_GETNEXT))) {
			if (process_ax_search_response(response, search,
						       refs, request)) {
				// request was reprocessed ->
				// pointer to request may be invalid!!!
				delete[] refs;
				return;
			}
		}
		else if (bindings > 0) {
			Vbx* vbs = new Vbx[bindings];
			response.get_vblist(vbs, bindings);
			for (int i=0; i<bindings; i++) {
				LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
				LOG("MasterAgentXMib: matching vb (sid)(tid)(oid)(value)(syntax)");
				LOG(response.get_session_id());
				LOG(response.get_transaction_id());
				LOG(vbs[i].get_printable_oid());
				LOG(vbs[i].get_printable_value());
				LOG(vbs[i].get_syntax());
				LOG_END;
				request->finish(refs[i], vbs[i]);
			}

			delete[] vbs;
		}
	  }
	  delete[] refs;
	  if (request->finished()) {
		Mib::finalize(request);
	  }
	  else {
	      request->unlock();
	  }
	}
}

void MasterAgentXMib::process_ax_set_response(AgentXPdu& response,
					      const AgentXPdu& search,
					      int* refs,
					      Request* request)
{
	if (response.get_error_status() != SNMP_ERROR_SUCCESS) {
		Vbx vb;
		int errind = response.get_error_index();
		if ((errind>0) && (errind<=search.get_vb_count())) {
			request->error(refs[errind-1], 
				       response.get_error_status());
		}
		else {
			request->error(request->first_pending(), 
				       SNMP_ERROR_GENERAL_VB_ERR);
		}
	}
	else if (search.get_agentx_type() == AGENTX_TESTSET_PDU) {
		for (int i=0; i<search.get_vb_count(); i++) {
			request->set_ready(refs[i]);
		}
	}
	else if (search.get_agentx_type() == AGENTX_COMMITSET_PDU) {
		for (int i=0; i<search.get_vb_count(); i++) {
			request->finish(refs[i]);
		}
	}
	do_process_request(request);
}


void MasterAgentXMib::do_process_request(Request* req) 
{
	req->trylock();
	Mib::do_process_request(req);
}


bool MasterAgentXMib::process_ax_search_response(AgentXPdu& response,
						    const AgentXPdu& search,
						    int* refs,
						    Request* request)
{
	int bindings = response.get_vb_count();
	Vbx* vbs = new Vbx[bindings];
	response.get_vblist(vbs, bindings);
	
	// check for errors
	if (response.get_error_status() != AGENTX_OK) {
		int errind = response.get_error_index()-1;
		if ((errind >= 0) && (errind < bindings)) 
			request->error(refs[errind],
				       response.get_error_status());
		else
			request->error(refs[0], response.get_error_status());
		delete[] vbs;
		return FALSE;
	}
	bool reprocess = FALSE;
	for (int i=0; i<bindings; i++) {
		SmiUINT32 syn = vbs[i].get_syntax();
		if ((syn == sNMP_SYNTAX_NOSUCHOBJECT) ||
		    (syn == sNMP_SYNTAX_NOSUCHINSTANCE)) {
			// Response is a failed AgentX GET request on a 
			// GETNEXT/GETBULK SNMP request
			request->set_oid(search.get_range(i).get_lower(), 
					 refs[i]);
			reprocess = TRUE;
		}
		else if (syn == sNMP_SYNTAX_ENDOFMIBVIEW) {
			// need to reprocess
			AgentXSearchRange range = search.get_range(i);
			if (range.is_unbounded()) {
				// subrequest is finished
				request->finish(refs[i], vbs[i]);
			}
			else {
			  request->set_oid(range.get_upper().predecessor(), 
					   refs[i]);
			  reprocess = TRUE;
			}
		}
		else if (!search.get_range(i).includes(vbs[i].get_oid())) {

			AgentXSearchRange r(search.get_range(i));
			LOG_BEGIN(loggerModuleName, WARNING_LOG | 2);
			LOG("MasterAgentXMib: search response not in range (resp)(lower)(upper)(incl)");
			LOG(vbs[i].get_oid().get_printable());
			LOG(r.get_lower().get_printable());
			LOG(r.get_upper().get_printable());
			LOG(r.is_lower_included());
			LOG_END;		
			
			if (r.is_unbounded()) {
				// subrequest is finished
				request->finish(refs[i], vbs[i]);
			}
			else {
				request->set_oid(r.get_upper().predecessor(), 
						 refs[i]);
				reprocess = TRUE;
			}			
		}
#ifdef _SNMPv3
		else if (requestList->get_vacm()->
			 isAccessAllowed(request->get_view_name(), 
					 vbs[i].get_oid()) != 
			 VACM_accessAllowed) {
			// need to reprocess
			request->set_oid(vbs[i].get_oid(), refs[i]);

			LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
			LOG("MasterAgentXMib: access not allowed, skipping (oid)");
			LOG(vbs[i].get_printable_oid());
			LOG_END;

			reprocess = TRUE;
		}
#endif
		else {
			LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
			LOG("MasterAgentXMib: matching vb (sid)(tid)(oid)(value)(syntax)");
			LOG(response.get_session_id());
			LOG(response.get_transaction_id());
			LOG(vbs[i].get_printable_oid());
			LOG(vbs[i].get_printable_value());
			LOG(vbs[i].get_syntax());
			LOG_END;

			request->finish(refs[i], vbs[i]);
		}
	}
	delete[] vbs;
	if (reprocess) {
		LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
		LOG("MasterAgentXMib: reprocessing request (tid)");
		LOG(request->get_transaction_id());
		LOG_END;
        request->unlock();
		do_process_request(request);
	}
	return reprocess;
}

bool MasterAgentXMib::process_ax_getbulk_response(AgentXPdu& response,
						     const AgentXPdu& search,
						     int* refs,
						     Request* request)
{
	int bindings = response.get_vb_count();
	// repeaters
	int repeaters = search.get_range_count() - search.get_non_repeaters();
	if (bindings - search.get_non_repeaters() > 
	    search.get_max_repetitions() * repeaters) {
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
		LOG("MasterAgentXMib: bulk response with more rep. than max.rep.(rep)(maxrep)");
		LOG(bindings-search.get_non_repeaters());
		LOG(search.get_max_repetitions());
		LOG_END;
		bindings = search.get_max_repetitions() * repeaters 
		           + search.get_non_repeaters();
	}
	Vbx* vbs = new Vbx[bindings];
	response.get_vblist(vbs, bindings);
	
	bool reprocess = FALSE;
	// check for errors
	if (response.get_error_status() != AGENTX_OK) {
		int errind = response.get_error_index()-1;
		if ((errind >= 0) && (errind < bindings)) 
			request->error(refs[errind],
				       response.get_error_status());
		else
			request->error(refs[0], response.get_error_status());
		delete[] vbs;
		return FALSE;
	}
	// check for empty response (returned by for example by NET-SNMP 5.1
	if (bindings == 0) {
#ifdef NET_SNMP_WORKAROUNDS
		for (int i=0; i<search.get_range_count(); i++) {
			AgentXSearchRange range = search.get_range(i);
			if (range.is_unbounded()) {
				// subrequest is finished
				Vbx vb(request->get_oid(refs[i]));
				vb.set_syntax(sNMP_SYNTAX_ENDOFMIBVIEW);
				request->finish(refs[i], vb);
			}
			else {
				request->set_oid(search.get_range(i).
						 get_upper().predecessor(), 
						 refs[i]);
				reprocess = TRUE;
			}
		}
#else
		request->error(refs[0], SNMP_ERROR_GENERAL_VB_ERR);
		delete[] vbs;
		return FALSE;		
#endif
	}
	// non repeaters
	int i=0;
	for (; (i<bindings) && (i<(int)search.get_non_repeaters()); i++) {
		SmiUINT32 syn = vbs[i].get_syntax();
		if ((syn == sNMP_SYNTAX_NOSUCHOBJECT) ||
		    (syn == sNMP_SYNTAX_NOSUCHINSTANCE)) {
			// Response is a failed AgentX GET request on a 
			// GETNEXT/GETBULK SNMP request
			request->set_oid(search.get_range(i).get_lower(), 
					 refs[i]);
			reprocess = TRUE;
		}
		else if (syn == sNMP_SYNTAX_ENDOFMIBVIEW) {
			// need to reprocess
			AgentXSearchRange range = search.get_range(i);
			if (range.is_unbounded()) {
				// subrequest is finished
				request->finish(refs[i], vbs[i]);
			}
			else {
				request->set_oid(search.get_range(i).
						 get_upper().predecessor(), 
						 refs[i]);
				reprocess = TRUE;
			}
		}
#ifdef _SNMPv3
		else if (requestList->get_vacm()->
			 isAccessAllowed(request->get_view_name(), 
					 vbs[i].get_oid()) != 
			 VACM_accessAllowed) {
			// need to reprocess
			request->set_oid(vbs[i].get_oid(), refs[i]);
			reprocess = TRUE;
		}
#endif
		else {
			LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
			LOG("MasterAgentXMib: matching vb (sid)(tid)(oid)(value)(syntax)");
			LOG(response.get_session_id());
			LOG(response.get_transaction_id());
			LOG(vbs[i].get_printable_oid());
			LOG(vbs[i].get_printable_value());
			LOG(vbs[i].get_syntax());
			LOG_END;

			request->finish(refs[i], vbs[i]);
		}
	}
	int j=0;
	for (; i+j<bindings; j++) {
		int repetition = j / repeaters;
		int column = (j % repeaters) + i;
		int target = refs[column] + repetition * request->get_rep();
		int source = i+j;

		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
		LOG("MasterAgentXMib: processing repeaters (rep)(col)(target)(source)(oid)(value)");
		LOG(repetition);
		LOG(column);
		LOG(target);
		LOG(source);
		LOG(vbs[source].get_printable_oid());
		LOG(vbs[source].get_printable_value());
		LOG_END;

		SmiUINT32 syn = vbs[source].get_syntax();
		if ((syn == sNMP_SYNTAX_NOSUCHOBJECT) ||
		    (syn == sNMP_SYNTAX_NOSUCHINSTANCE)) {
			// Response is a failed AgentX GET request on a 
			// GETNEXT/GETBULK SNMP request
			request->set_oid(search.get_range(column).get_lower(), 
					 target);
			// protect subrequest against overwriting by 
			// Request::init_rep_row
			request->set_ready(target);
			reprocess = TRUE;
		}
		else if (syn == sNMP_SYNTAX_ENDOFMIBVIEW) {
			LOG_BEGIN(loggerModuleName, DEBUG_LOG | 7);
			LOG("MasterAgentXMib: end of mib view (rep)(col)(target)(source)(oid)(value)");
			LOG(repetition);
			LOG(column);
			LOG(target);
			LOG(source);
			LOG(vbs[source].get_printable_oid());
			LOG(vbs[source].get_printable_value());
			LOG_END;
			// need to reprocess
			AgentXSearchRange range = search.get_range(column);
			if (range.is_unbounded()) {
				// subrequest is finished
				request->finish(target, vbs[source]);
			}
			else {
				request->set_oid(search.get_range(column).
						 get_upper().predecessor(), 
						 target);
				// protect subrequest against overwriting by 
				// Request::init_rep_row
				request->set_ready(target);
				reprocess = TRUE;
			}
		}
		else if (!search.get_range(column).includes(
				       vbs[source].get_oid())) {

			AgentXSearchRange r(search.get_range(column));
			LOG_BEGIN(loggerModuleName, WARNING_LOG | 2);
			LOG("MasterAgentXMib: bulk response not in range (resp)(lower)(upper)(incl)");
			LOG(vbs[source].get_oid().get_printable());
			LOG(r.get_lower().get_printable());
			LOG(r.get_upper().get_printable());
			LOG(r.is_lower_included());
			LOG_END;		
			
			if (r.is_unbounded()) {
				// subrequest is finished
				request->finish(target, vbs[source]);
			}
			else {
				request->set_oid(r.get_upper().predecessor(), 
						 target);
				// protect subrequest against overwriting by 
				// Request::init_rep_row
				request->set_ready(target);
				reprocess = TRUE;
			}			
		}
#ifdef _SNMPv3
		else if (requestList->get_vacm()->
			 isAccessAllowed(request->get_view_name(), 
					 vbs[source].get_oid()) != 
			 VACM_accessAllowed) {
			// need to reprocess
			request->set_oid(vbs[source].get_oid(), target);
			// protect subrequest against overwriting by 
			// Request::init_rep_row
			request->set_ready(target);
			reprocess = TRUE;
		}
#endif
		else {
			LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
			LOG("MasterAgentXMib: matching vb (sid)(tid)(oid)(value)(syntax)");
			LOG(response.get_session_id());
			LOG(response.get_transaction_id());
			LOG(vbs[source].get_printable_oid());
			LOG(vbs[source].get_printable_value());
			LOG(vbs[source].get_syntax());
			LOG_END;

			request->finish(target, vbs[source]);
		}
	}
	delete[] vbs;
	if ((reprocess) || (!request->finished())) {
		LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
		LOG("MasterAgentXMib: reprocessing bulk request (tid)");
		LOG(request->get_transaction_id());
		LOG_END;
		reprocess = TRUE;
        request->unlock();
		do_process_request(request);
	}
	return reprocess;
}

bool MasterAgentXMib::is_dupl_reg(const AgentXRegEntry& newReg)
{
	lock_reg();
	ListCursor<AgentXRegEntry> cur;
	for (cur.init(&registrations); cur.get(); cur.next()) {
		if (cur.get()->is_duplicate_of(newReg)) {
			unlock_reg();
			return TRUE;
		}
	}
	unlock_reg();
	return FALSE;
}

bool MasterAgentXMib::has_conflict(const AgentXRegEntry& newReg)
{
	List<AgentXNode> dummy;
	bool retval = (!get_affected_regs(newReg, newReg.region, dummy));
	dummy.clear();
	return retval;
}
	
bool MasterAgentXMib::get_affected_regs(const AgentXRegEntry& newReg,
					   const AgentXRegion& region,
					   List<AgentXNode>& list)
{
	MibContext* c = get_context(newReg.context);
	if (!c) return TRUE; // no context -> no affected regions
	Oidx start(region.get_lower());
	MibEntryPtr entry = 0;
	if (c->find_lower(start, entry) == SNMP_ERROR_SUCCESS) {
		bool overlapsNonAgentX = FALSE;
		while ((entry) && (*entry->max_key() <= region.get_lower()))
			entry = c->find_next(*entry->key());

		while ((entry) && (*entry->key() < region.get_upper())) {
			if (!AgentXNode::is_agentx(*entry)) {
				overlapsNonAgentX = TRUE;
				break;
			}
			list.add((AgentXNode*)entry);
			entry = c->find_next(*entry->key());
		}
		if (overlapsNonAgentX) 
			return FALSE;
	}
	return TRUE;
}


void MasterAgentXMib::add_entry(AgentXRegEntry* newReg)
{
	lock_reg();
	registrations.add(newReg);
	if (newReg->region.is_range()) {

		u_int start = 
		  newReg->region.get_lower()[newReg->region.get_subid()-1];
		u_int stop = 
		  newReg->region.get_upper()[newReg->region.get_subid()-1];
		  
		if (start > stop) {
			LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
			LOG("MasterAgentXMib: empty range registration (lower)(upper)(rsid)");
			LOG(newReg->region.lower().get_printable());
			LOG(newReg->region.upper().get_printable());
			LOG(newReg->region.get_subid());
			LOG_END;
		}
		else {
			LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
			LOG("MasterAgentXMib: range registration (lower)(upper)(rsid)");
			LOG(newReg->region.lower().get_printable());
			LOG(newReg->region.upper().get_printable());
			LOG(newReg->region.get_subid());
			LOG_END;
		}
		for (u_int i=start; i<=stop; i++) {
			Oidx root(newReg->region.get_lower());
            if (newReg->region.get_subid()-1 < root.len()) {
                root[newReg->region.get_subid()-1] = i;
            }
			AgentXRegion r(root, root.next_peer());
			//      			r.set_single(TRUE);
			add_entry(newReg, r);
		}
	}
	else {
		add_entry(newReg, newReg->region);
	}
	unlock_reg();
}

Oidx MasterAgentXMib::get_upper_session_boundary(const OctetStr& context, 
						 AgentXNode* n)
{
#ifdef _THREADS
        Synchronized::TryLockResult lockResult;
	if ((lockResult = mibLock.trylock()) == Synchronized::BUSY) {
		LOG_BEGIN(loggerModuleName, INFO_LOG | 3);
		LOG("MasterAgentXMib: region for GETNEXT/BULK not optimized due to concurrent requests");
		LOG_END;
		return *n->max_key();
	}
#endif
	MibContext* c = get_context(context);
	if (!c) {
		unlock_mib();
		return *n->max_key();
	}
	Oidx oid(*n->max_key());
	MibEntry* entry = c->find_next(*n->key());
	while ((entry) && (AgentXNode::is_agentx(*entry)) &&
	       (((AgentXNode*)entry)->active_registration()->sessionID == 
		n->active_registration()->sessionID)) {
	    oid = *entry->max_key();
	    entry = c->find_next(*entry->key());
	}
#ifdef _THREADS
        if (lockResult == Synchronized::LOCKED) {
            mibLock.unlock();
        }
#endif
	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 3);
	LOG("MasterAgentXMib: GETBULK/NEXT region upper bound optimized (old)(optimized)");
	LOG(n->max_key()->get_printable());
	LOG(oid.get_printable());
	LOG_END;
	return oid;
}


bool MasterAgentXMib::remove_entry(u_int cid, const AgentXRegEntry& reg)
{
	lock_mib();
	List<Oidx> victims;
	ListCursor<Oidx> cur;
	MibContext* c = get_context(reg.context);
	if (!c) {
		unlock_mib();
		return FALSE;
	}
	OidListCursor<MibEntry> cr(c->get_content());
	AgentXNode* last = 0;
	for (; cr.get(); cr.next()) {
		if (AgentXNode::is_agentx(*cr.get())) {
			AgentXNode* node = (AgentXNode*)cr.get();
			if (node->remove_registration(reg)) {
				victims.add((Oidx*)node->key()->clone());
			}
			else {
				// merge regions
				if ((last) && 
				    (last->registration_count() == 1) && 
				    (node->registration_count() == 1) &&
				    (*last->max_key() == *node->key()) &&
				    (node->active_registration() == 
				     last->active_registration())) {
				  AgentXRegion r(*node->key(),
						 *last->max_key());
				  if (node->active_registration()->get_region()
				      .covers(r)) {
					victims.add((Oidx*)
						    node->key()->clone());
					last->expand(*node->max_key());
				  }
				  else last = node;
				}
				else last = node;
			}
		}
	}
	unlock_mib();
	for (cur.init(&victims); cur.get(); cur.next()) {
		remove(reg.context, *cur.get());
	}
	victims.clearAll(); 

	bool found = FALSE;
	// pointer to reg is always != pointers in registrations
	// so we need to check for equality 
	ListCursor<AgentXRegEntry> r;
	for (r.init(&registrations); r.get(); ) {
		if (*r.get() == reg) {
			found = TRUE;
			AgentXRegEntry* victim = r.get();
			// remove from AgentX MIB
			if (agentxRegistrationEntry::instance) {
			  agentxRegistrationEntry::instance->
			    remove(cid, victim->sessionID, victim->mibID);
			}
			r.next();
			delete registrations.remove(victim);
		}
		else r.next();
	}
	return found;
}

time_t MasterAgentXMib::timeout_requests(time_t timeout) 
{
    if (agentx->stopit) {
        // do not wait for further AgentX packets.
        return -1;
    }
	AgentXQueue* axQueue = agentx->get_queue();
	axQueue->lock();
    AgentXPdu* late = axQueue->get_late(timeout);
	while (late) {
		u_int sid = late->get_session_id();
		AgentXPdu response(*late);
		axQueue->unlock();
		// increment timeout counter on session
		openSessions.lock();
		AgentXSession* s = openSessions.get_session(sid);
		bool stop = FALSE;
		if (s) {
			LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
			LOG("MasterAgentXMib: subrequest timeout (timeout)(sid)(tid)");
			LOG(timeout);
			LOG(sid);
			LOG(response.get_transaction_id());
			LOG_END;
			s->inc_timeouts();			
		        stop = s->is_unable_to_respond();
			// process dummy response with genErr
			response.set_type(AGENTX_RESPONSE_PDU);
			response.set_error_status(SNMP_ERROR_GENERAL_VB_ERR);
			response.set_peer(new AgentXPeer(s->get_peer()));
		}
		openSessions.unlock();
		// this will remove the request from the queue
		dispatch_ax_response(response, FALSE);
		// delete peer copy 
		delete response.get_peer();

		if (stop) {
			LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
			LOG("MasterAgentXMib: session timeout, closing it (sid)");
			LOG(sid);
			LOG_END;
			close_session(sid, AGENTX_REASON_TIMEOUTS);
		}			
		// get next timed out request
		axQueue->lock();
		late = axQueue->get_late(timeout);
	}
	axQueue->unlock();
	return axQueue->timeout(timeout);
}

void MasterAgentXMib::remove_indexes(u_int sid) 
{
	OidListCursor<AgentXIndexDB> cur;
	for (cur.init(&indexDatabase); cur.get(); cur.next()) {
		cur.get()->remove_session(sid);
	}
}

void MasterAgentXMib::remove_agent_caps_by_session(AgentXSession* s) 
{
	lock_reg();
	ListCursor<Vbx>* cur = s->get_agent_caps();
	for (; cur->get(); cur->next()) {
		OctetStr c;
		cur->get()->get_value(c);
		Mib::remove_agent_caps(c, cur->get()->get_oid());
	}
	delete cur;
	unlock_reg();
}

void MasterAgentXMib::close_session(u_int sid, u_char reason) 
{
	// remove is synchronized! 
	AgentXSession* s = openSessions.remove(sid);
	if (s) {
		remove_agent_caps_by_session(s);
		remove_indexes(sid);
		agentx->close_session(*s, 
				      requestList->create_transaction_id(), 
				      reason);
		remove_entries(s->get_peer().mibID, sid);
		delete s;
	}
}

void MasterAgentXMib::remove_entries(u_int cid, u_int sid) 
{
	lock_reg();
	ListCursor<AgentXRegEntry> cur;
        for (cur.init(&registrations); cur.get();) {
		if (cur.get()->sessionID == sid) {
			AgentXRegEntry victim(*cur.get());
			remove_entry(cid, victim);
			// We cannot simply go to the next
			// entry because through a merge which may have
			// occured above in "remove_entry" the next entry
			// may not be valid any more -> only solution is
			// to go back entirely. This is not very fast but
			// removing of entries should be a rare event.
			cur.init(&registrations);
		}
		else cur.next();
	}
	unlock_reg();
}


void MasterAgentXMib::add_entry(AgentXRegEntry* newReg,
				const AgentXRegion& region)
{
	if (region.is_range()) {
	  
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
		LOG("MasterAgentXMib: register: range registration not allowed");
		LOG_END;
		return;
	}
	lock_mib();
	List<AgentXNode> affected;
	ListCursor<AgentXNode> curNodes;
	AgentXRegion r1(region);

	List<AgentXNode> splitted;
	
	if (region.is_empty()) {
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 2);
		LOG("MasterAgentXMib: empty region, ignored (lower)(upper)");
		LOG(r1.get_lower().get_printable());
		LOG(r1.get_upper().get_printable());
		LOG_END;
		unlock_mib();
		return;
	}

	if (get_affected_regs(*newReg, region, affected)) {

		if (affected.size() > 0) {

		  AgentXNode* last = 0;
		  for (curNodes.init(&affected); 
		       curNodes.get();
		       curNodes.next()) {

		    AgentXRegion r2(*curNodes.get()->key(),
				    *curNodes.get()->max_key());

		    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 5);
		    LOG("MasterAgentXMib: affected region r2 (lower)(upper)");
		    LOG(r2.get_lower().get_printable());
		    LOG(r2.get_upper().get_printable());
		    LOG(r1.get_lower().get_printable());
		    LOG(r1.get_upper().get_printable());
		    LOG_END;
		    // compute new regions
		    if (r2.covers(r1)) {

                LOG_BEGIN(loggerModuleName, DEBUG_LOG | 5);
                LOG("MasterAgentXMib: r2 covers r1");
                LOG_END;

                AgentXNode* r2c = (AgentXNode*)curNodes.get()->clone();
                r2c->set_region(AgentXRegion(r1.get_upper(),
                            *curNodes.get()->max_key()));
                if (r2.get_lower() == r1.get_lower()) {
                    curNodes.get()->shrink(r1.get_upper());
                    curNodes.get()->add_registration(newReg);
                }
                else {
                    curNodes.get()->shrink(r1.get_lower());
                    AgentXNode* r2b = (AgentXNode*)
                      curNodes.get()->clone();
                    r2b->set_region(r1);
                    r2b->add_registration(newReg);
                    splitted.add(r2b);
                }
                if (!r2c->is_empty()) {
                    splitted.add(r2c);
                }
                else delete r2c;
                break;
		    }
		    else if (r1.covers(r2)) {
		      
                LOG_BEGIN(loggerModuleName, DEBUG_LOG | 5);
                LOG("MasterAgentXMib: r1 covers r2");
                LOG_END;
                AgentXNode* r1a = 0;
                if (last) {
                  r1a = 
                    new AgentXNode(newReg, 
                           AgentXRegion(*last->max_key(), 
                                r2.get_lower()));

                }
                else
                  r1a = 
                    new AgentXNode(newReg, 
                           AgentXRegion(r1.get_lower(), 
                                r2.get_lower()));
                if (splitted.last()) {
                    splitted.last()->shrink(r2.get_lower());
                }
                curNodes.get()->add_registration(newReg);
                if ((r1.get_lower() == r2.get_lower()) ||
                    ((splitted.last()) && 
                     (splitted.last()->get_region() == 
                      r1a->get_region())))
                    delete r1a;
                else    
                    splitted.add(r1a);

                r1 = AgentXRegion(r2.get_upper(), r1.get_upper());
		    }
		    else if ((r1.overlaps(r2)) && 
			     (r2.get_lower() < r1.get_lower())) {
			
                LOG_BEGIN(loggerModuleName, DEBUG_LOG | 5);
                LOG("MasterAgentXMib: r1 overlaps r2");
                LOG_END;

                curNodes.get()->shrink(r1.get_lower());
                AgentXNode* r2b = (AgentXNode*)curNodes.get()->clone();
                r2b->set_region(AgentXRegion(r1.get_lower(),
                                 r2.get_upper()));
                r2b->add_registration(newReg);
                splitted.add(r2b);
                r1 = AgentXRegion(r2.get_upper(), r1.get_upper());
		    }
		    // r1.overlaps(r2) and (r1.get_lower() < r2.get_lower())
		    else {

                LOG_BEGIN(loggerModuleName, DEBUG_LOG | 5);
                LOG("MasterAgentXMib: r1 overlaps r2");
                LOG_END;

                curNodes.get()->shrink(r1.get_upper());
                AgentXNode* r2b = (AgentXNode*)curNodes.get()->clone();
                r2b->set_region(AgentXRegion(r1.get_upper(),
                                 r2.get_upper()));
                curNodes.get()->add_registration(newReg);
                splitted.add(r2b);
                AgentXNode* r1a = 
                  new AgentXNode(newReg, AgentXRegion(r1.get_lower(),
                                      r2.get_lower()));
                splitted.add(r1a);
                break;
		    }
		    if (!r1.is_empty()) { 
    			splitted.add(new AgentXNode(newReg, r1));
		    }
		    else {
    			splitted.add(new AgentXNode(newReg, region));
		    }
		    last = curNodes.get();
		  }
		  
		  unlock_mib();
		  ListCursor<AgentXNode> curSplit;
		  for (curSplit.init(&splitted); curSplit.get(); 
		       curSplit.next()) {
			if (add(newReg->context, curSplit.get())) {
                LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
                LOG("MasterAgentXMib: registered (context)(lower)(upper)");
                LOG(newReg->context.get_printable());
                LOG(curSplit.get()->key()->get_printable());
                LOG(curSplit.get()->max_key()->get_printable());
                LOG_END;
            }
            else {
                LOG_BEGIN(loggerModuleName, WARNING_LOG | 2);
                LOG("MasterAgentXMib: register failed (context)(lower)(upper)");
                LOG(newReg->context.get_printable());
                LOG(curSplit.get()->key()->get_printable());
                LOG(curSplit.get()->max_key()->get_printable());
                LOG_END;
                delete curSplit.get();
            }
		  }
		}
		else {
			// add a new context to the VACM all other view and
		        // user configuration has to be done via SNMP or
			// by overwriting add_new_context
			if ((autoContext) && (!get_context(newReg->context))) 
				add_new_context(newReg->context);
			unlock_mib();
            AgentXNode* newNode = new AgentXNode(newReg, region);
			if (add(newReg->context, newNode)) {
                LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
                LOG("MasterAgentXMib: registered new (context)(lower)(upper)");
                LOG(newReg->context.get_printable());
                LOG(Oidx(region.get_lower()).get_printable());
                LOG(Oidx(region.get_upper()).get_printable());
                LOG_END;		       
            }
            else {
                LOG_BEGIN(loggerModuleName, WARNING_LOG | 2);
                LOG("MasterAgentXMib: registered new (context)(lower)(upper)");
                LOG(newReg->context.get_printable());
                LOG(Oidx(region.get_lower()).get_printable());
                LOG(Oidx(region.get_upper()).get_printable());
                LOG_END;		       
                delete newNode;
            }
		}
	}
	else {
		unlock_mib();
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
		LOG("MasterAgentXMib: register: failed (would affect non agentx registrations)");
		LOG_END;
	}
	splitted.clear();
	affected.clear();
}

void MasterAgentXMib::add_new_context(const OctetStr& newContext)
{
#ifdef _SNMPv3
	requestList->get_vacm()->addNewContext(newContext);
#endif
}

	
void MasterAgentXMib::lock_reg() 
{
	registrationLock.lock();
}

void MasterAgentXMib::unlock_reg() 
{
	registrationLock.unlock();
}


void MasterAgentXMib::close_all_sessions() 
{
	Lock sync(openSessions);
	ListCursor<AgentXSession> cur(openSessions.get_sessions());
	for (; cur.get(); cur.next()) {
		agentx->close_session(*cur.get(), 
				      requestList->create_transaction_id(),
				      AGENTX_REASON_SHUTDOWN);
	}
	openSessions.clear();
}

#ifdef AGENTPP_NAMESPACE
}
#endif

