/*_############################################################################
  _## 
  _##  agentx_session.h  
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

#ifndef _agentx_session_h_
#define _agentx_session_h_

#include <agent_pp/agent++.h>
#include <agent_pp/snmp_pp_ext.h>
#include <agentx_pp/agentx_reg.h>
#include <agentx_pp/agentx_def.h>
#include <agentx_pp/agentx_threads.h>
#include <agentx_pp/agentx_peer.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

#if !defined (AGENTXPP_DECL_TEMPL_LIST_VBX)
#define AGENTXPP_DECL_TEMPL_LIST_VBX
	AGENTXPP_DECL_TEMPL template class AGENTXPP_DECL 	List<Vbx>;
#endif

class AGENTXPP_DECL AgentXSession {

friend class AgentXRequest;
friend class AgentXQueue;
friend class AgentXSessions;

 public:
	AgentXSession();
	AgentXSession(u_int);
	
	~AgentXSession();

	int		get_state() const      	{ return state; }
	void		set_state(int s)	{ state = s; }

	u_int		get_id() const		{ return sessionID; }
	void		set_id(u_int i)	{ sessionID = i; }

	bool		get_byte_order() const { return networkByteOrder; }
	void		set_byte_order(bool newOrder) 
					{ networkByteOrder = newOrder; }
	u_char		get_timeout() const	{ return defaultTimeout; }
	void		set_timeout(u_char t)	{ defaultTimeout = t; }
	Oidx		get_oid() const		{ return id; }
	NS_SNMP OctetStr	get_descr() const	{ return descr; } 

	void		set_peer(const AgentXPeer& p) { peer = p; }
	AgentXPeer	get_peer() const { return peer; }

	static u_int	get_next_session_id();

	void		add_agent_caps(const NS_SNMP OctetStr& c, const Oidx& oid) 
					{ sysORIDs.add(new Vbx(oid, c));}
	void		remove_agent_caps(const NS_SNMP OctetStr&, const Oidx& id);

	ListCursor<Vbx>* get_agent_caps() { 
				return new ListCursor<Vbx>(&sysORIDs); }

	AgentXSession&  operator=(const AgentXSession&);

	void		inc_timeouts() { consecutiveTimeouts++; }
	void            reset_timeouts() { consecutiveTimeouts = 0; }
	bool         is_unable_to_respond() 
			    { return (consecutiveTimeouts >= 
				      AGENTX_MAX_TIMEOUTS); }  

	AgentXPeer	peer;

 protected:
	u_int		sessionID;

	bool		networkByteOrder;
	u_char		defaultTimeout;

	int		state;

	Oidx		id;
	NS_SNMP OctetStr descr;

	List<Vbx>       sysORIDs;

	u_char          consecutiveTimeouts;
};


/**
 * The AgentXSessions class represents all opened sessions between
 * an AgentX++ master and its AgentX subagents.
 *
 * @author Frank Fock
 * @version 1.3.2  
 */
#if !defined (AGENTXPP_DECL_TEMPL_LIST_AGENTXSESSION)
#define AGENTXPP_DECL_TEMPL_LIST_AGENTXSESSION
	AGENTXPP_DECL_TEMPL template class AGENTXPP_DECL List<AgentXSession>;
#endif

class AGENTXPP_DECL AgentXSessions: public Synchronized
{
 public:
	AgentXSessions();
	virtual ~AgentXSessions();

	/**
	 * Get the session with a given id.
	 * (SYNCHRONIZED)
	 *
	 * @param sessionID
	 *    a session id.
	 * @param session
	 *    contains the session attributes of the requested session
	 *    if TRUE is returned and is unchanged if FALSE is returned.
	 * @return
	 *    TRUE if such a session could be found,
	 *    FALSE otherwise. 
	 */
       
	virtual bool		get_session(u_int, AgentXSession&);
       
	/**
	 * Get the session with a given id.
	 * (NOT SYNCHRONIZED)
	 *
	 * @param sessionID
	 *    a session id.
	 * @return
	 *    a pointer to the requested session or 0 if such an id does
	 *    not exist.
	 */       
	virtual AgentXSession* 	get_session(u_int);	

	/**
	 * Remove a session. (SYNCHRONIZED)
	 *
	 * @param sessionID
	 *    the session id of the session to remove.
	 * @return
	 *    the pointer of the session if it could be removed successfully
	 *    or 0 if not.
	 */
	virtual AgentXSession*	remove(u_int);

	/**
	 * Make a new session.
	 * 
	 * @param byteOrder
	 *    the byte order used for the new session (TRUE==networkByteOrder).
	 * @param timeout
	 *    the timeout of used by the new session.
	 * @param id
	 *    an object id.
	 * @param descr
	 *    an OctetStr instance.
	 * @param peer
	 *    the AgentX subagent initiating this session.
	 * @return 
	 *    the session id of the new session.
	 */
	virtual u_int		make_new_session(bool, u_char, 
						 const Oidx&, 
						 const NS_SNMP OctetStr&,
						 const AgentXPeer&);
	
	/**
	 * Return a ListCursor located at the first session.
	 *
	 * @return 
	 *    a ListCursor instance.
	 */
	ListCursor<AgentXSession> get_sessions() 
	  { return ListCursor<AgentXSession>(&sessions); }

	/**
	 * Clear all sessions.
	 */
	void			clear() { sessions.clearAll(); }

	/**
	 * Remove all sessions associated with a given peer. 
	 * (SYNCHRONIZED)
	 *
	 * @param peer
	 *    a AgentXPeer.
	 */
	virtual void		remove(const AgentXPeer&);

	/**
	 * Return a list of all sessions associated with a given peer and
	 * remove these sessions. 
	 * (SYNCHRONIZED)
	 *
	 * @param peer
	 *    a AgentXPeer.
	 * @return
	 *    a pointer to a List instance which must be deleted (including
	 *    the contained sessions) by the receiver.
	 * @since
	 *    1.3.2
	 */ 
	List<AgentXSession>*	remove_sessions_of_peer(const AgentXPeer&);

	/**
	 * Return a list of all sessions associated with a given peer.
	 * NOTE: Befor calling the destructor of the returned list call its
	 * clear() method!
	 * 
	 * (NOT SYNCHRONIZED)
	 *
	 * @param peer
	 *    a AgentXPeer.
	 * @return
	 *    a pointer to a List instance.
	 */ 
	List<AgentXSession>*	get_sessions_of_peer(const AgentXPeer&);

 protected:

        u_int	       	lastSessionID;

	List<AgentXSession> sessions;
};

#ifdef AGENTPP_NAMESPACE
}
#endif


#endif

