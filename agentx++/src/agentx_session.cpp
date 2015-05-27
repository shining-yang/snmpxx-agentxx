/*_############################################################################
  _## 
  _##  agentx_session.cpp  
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

#include <agentx_pp/agentx_session.h>

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif


#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

// static const char *loggerModuleName = "agentx++.agentx_session";


AgentXSession::AgentXSession()
{
	sessionID = 0;
	networkByteOrder = FALSE;
	defaultTimeout = AGENTX_DEFAULT_TIMEOUT;
	consecutiveTimeouts = 0;
	id = "0.0";
}

AgentXSession::AgentXSession(u_int sid)
{
	sessionID = sid;
	networkByteOrder = FALSE;
	defaultTimeout = AGENTX_DEFAULT_TIMEOUT;
	consecutiveTimeouts = 0;
	id = "0.0";
}

AgentXSession::~AgentXSession()
{
}

void AgentXSession::remove_agent_caps(const OctetStr& c, const Oidx& oid) 
{
	ListCursor<Vbx> cur;
	for (cur.init(&sysORIDs); cur.get();) {
		OctetStr cnt;
		cur.get()->get_value(cnt);
		if ((cur.get()->get_oid() == oid) &&
		    (cnt == c)){
			Vbx* victim = cur.get();
			cur.next();
			delete sysORIDs.remove(victim);
		}
		else cur.next();
	}
}

AgentXSession& AgentXSession::operator=(const AgentXSession& other)
{
	sessionID = other.sessionID;
	networkByteOrder = other.networkByteOrder;
	defaultTimeout = other.defaultTimeout;
	state = other.state;
	id = other.id;
	descr = other.descr;
	peer = other.peer;
	sysORIDs.clearAll();
	ListCursor<Vbx> cur;
	for (cur.init(&other.sysORIDs); cur.get(); cur.next()) {
		sysORIDs.add(cur.get()->clone());
	}
	return *this;
}


AgentXSessions::AgentXSessions(): Synchronized()
{
	lastSessionID = 0;
}

AgentXSessions::~AgentXSessions()
{
}


AgentXSession* AgentXSessions::get_session(u_int sessionId) 
{
	ListCursor<AgentXSession> cur;
	for (cur.init(&sessions); cur.get(); cur.next()) {
		if (cur.get()->get_id() == sessionId)
			return cur.get();
	}
	return 0;
}

bool AgentXSessions::get_session(u_int sessionId, AgentXSession& target) 
SYNCHRONIZED(
{
	ListCursor<AgentXSession> cur;
	for (cur.init(&sessions); cur.get(); cur.next()) {
		if (cur.get()->get_id() == sessionId) {
			target = *cur.get();
			return TRUE;
		}
	}
	return FALSE;
})

u_int AgentXSessions::make_new_session(bool order, u_char timeout,
				       const Oidx& id, 
				       const OctetStr& descr,
				       const AgentXPeer& peer)
SYNCHRONIZED(
{
	AgentXSession* s = new AgentXSession(++lastSessionID);
	s->networkByteOrder = order;
	s->defaultTimeout = timeout;
	s->id = id;
	s->descr = descr;
	s->set_peer(peer);
	sessions.add(s);
	return s->get_id();
})

AgentXSession* AgentXSessions::remove(u_int sessionID) 
SYNCHRONIZED(
{
	ListCursor<AgentXSession> cur;
	for (cur.init(&sessions); cur.get(); cur.next()) {
		if (cur.get()->get_id() == sessionID) {
			return sessions.remove(cur.get_cursor());
		}
	}
	return 0;
})

void AgentXSessions::remove(const AgentXPeer& peer) 
SYNCHRONIZED(
{
	ListCursor<AgentXSession> cur;
	for (cur.init(&sessions); cur.get(); ) {
		if (cur.get()->get_peer() == peer) {
			AgentXSession* victim = cur.get();
			cur.next();
			delete sessions.remove(victim);
		}
		else cur.next();
	}	
})

List<AgentXSession>* 
AgentXSessions::remove_sessions_of_peer(const AgentXPeer& peer) 
SYNCHRONIZED(
{
	List<AgentXSession>* r = new List<AgentXSession>(); 
	ListCursor<AgentXSession> cur;
	for (cur.init(&sessions); cur.get(); ) {
		if (cur.get()->get_peer() == peer) {
			AgentXSession* victim = cur.get();
			cur.next();
			sessions.remove(victim);
			r->add(victim);
		}
		else cur.next();
	}	
	return r;
})

List<AgentXSession>* 
AgentXSessions::get_sessions_of_peer(const AgentXPeer& peer) 
{
	List<AgentXSession>* r = new List<AgentXSession>(); 
	ListCursor<AgentXSession> cur;
	for (cur.init(&sessions); cur.get(); cur.next()) {
		if (cur.get()->get_peer() == peer) {
			r->add(cur.get());
		}	
	}
	return r;
}

#ifdef AGENTPP_NAMESPACE
}
#endif

