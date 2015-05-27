/*_############################################################################
  _## 
  _##  agentx_queue.cpp  
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

#include <agentx_pp/agentx_queue.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

// static const char *loggerModuleName = "agentx++.agentx_queue";


u_int AgentXQueue::pid = 0;


AgentXQueue::AgentXQueue()
{
}

AgentXQueue::~AgentXQueue()
{
}

// should be synchronized?
u_int AgentXQueue::create_packet_id() {
	return pid++;
}

u_int AgentXQueue::add(AgentXPdu* req) SYNCHRONIZED(
{
	queue.add(req);
	req->set_packet_id(create_packet_id());
	return req->get_packet_id();
})

bool AgentXQueue::remove(AgentXPdu* req) 
{
	AgentXPdu* victim = queue.remove(req);
	if (victim) {
		delete victim;
		return TRUE;
	}
	return FALSE;
}

bool AgentXQueue::pending(u_int tid) SYNCHRONIZED(
{
	OrderedListCursor<AgentXPdu> cur;
	for (cur.init(&queue); cur.get(); cur.next()) {
		if (cur.get()->get_transaction_id() == tid) 
			return TRUE;
	}
	return FALSE;
}) 


void AgentXQueue::cancel(u_int tid) SYNCHRONIZED(
{
	OrderedListCursor<AgentXPdu> cur;
	for (cur.init(&queue); (cur.get()); ) {
		if (cur.get()->get_transaction_id() == tid) {
			AgentXPdu* victim = cur.get();
			cur.next();
			delete queue.remove(victim);
		}
		else cur.next();
	}
}) 

time_t AgentXQueue::timeout(time_t t) SYNCHRONIZED(
{
	OrderedListCursor<AgentXPdu> cur;
	for (cur.init(&queue); ((cur.get()) && 
	                        // do not timeout request that are not sent yet
	                        (cur.get()->get_timestamp()>0) &&
				(cur.get()->get_timestamp() < t)); ) {

		AgentXPdu* victim = cur.get();
		cur.next();
		delete queue.remove(victim);
	}
	// Ari: handle the case when there is only a pending request in queue
	time_t r = 0;
	if ((cur.get()) && (cur.get()->get_timestamp()>0)) {
		r = cur.get()->get_timestamp()-t;
	}	
	return r;
}) 

AgentXPdu* AgentXQueue::get_late(time_t t)
{
        AgentXPdu* f = queue.first(); 
	if ((f) && (f->get_timestamp()>0) && (f->get_timestamp() < t)) {
		return f;
	}
	return 0;
}

AgentXPdu* AgentXQueue::find(u_int sid, unsigned long tid, 
			     bool pending)
{
	// not synchronized because we give pointer outside.
	OrderedListCursor<AgentXPdu> cur;
	for (cur.init(&queue); cur.get(); cur.next()) {

		if ((cur.get()->get_session_id() == sid) &&
		    (cur.get()->get_transaction_id() == tid) &&
		    ((!pending) || (cur.get()->get_timestamp() == 0))) {
			return cur.get();
		}
	}
	return 0;	
}

AgentXPdu* AgentXQueue::find(u_int _pid)
{
	// not synchronized because we give pointer outside.
	OrderedListCursor<AgentXPdu> cur;
	for (cur.init(&queue); cur.get(); cur.next()) {
		if (cur.get()->get_packet_id() == _pid) 
			return cur.get();
	}
	return 0;	
}

OrderedListCursor<AgentXPdu> AgentXQueue::elements() 
{
	// not synchronized because we give pointer outside.
	OrderedListCursor<AgentXPdu> cur;
	cur.init(&queue);
	return cur;
}

#ifdef AGENTPP_NAMESPACE
}
#endif






