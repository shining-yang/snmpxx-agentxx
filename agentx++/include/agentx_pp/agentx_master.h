/*_############################################################################
  _## 
  _##  agentx_master.h  
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


#ifndef agentx_master_h_
#define agentx_master_h_

#include <agentx_pp/agentx.h>
#include <agentx_pp/agentx_reg.h>
#include <agentx_pp/agentx_session.h>
#include <agentx_pp/agentx_request.h>
#include <agentx_pp/agentx_node.h>
#include <agentx_pp/agentx_index.h>
#include <agent_pp/mib.h>
#include <agent_pp/notification_originator.h>

#ifndef _WIN32THREADS
#include <pthread.h>
#endif

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

class MasterAgentXMib;

/**
 * The AgentXResponseTask class implements a task for processing 
 * AgentX responses.
 *
 * @author Frank Fock
 * @version 1.4b
 */ 
class AGENTXPP_DECL AgentXResponseTask: public Runnable {
  public:
	AgentXResponseTask(MasterAgentXMib* m, AgentXPdu* t, bool ti) 
	  { mib = m; task = t; timely = ti; }
	virtual ~AgentXResponseTask() { delete task; }
	
	virtual void run();
 protected:
	MasterAgentXMib* mib;
	AgentXPdu* task;
	bool timely;
};

/**
 * The AgentXCloseTask class implements a task for processing 
 * AgentX close PDUs.
 *
 * @author Frank Fock
 * @version 1.4b
 */ 
class AGENTXPP_DECL AgentXCloseTask: public Runnable {
  public:
	AgentXCloseTask(MasterAgentXMib* m, AgentXPeer* p, AgentXPdu* c) 
	  { mib = m; peer = p; closePdu = c; }
	virtual ~AgentXCloseTask();
	
	virtual void run();
 protected:
	MasterAgentXMib* mib;
	AgentXPeer*      peer;
	AgentXPdu*       closePdu;
};

/**
 * The AgentXTask class implements a generic task for processing 
 * AgentX PDUs. Currently, only register PDUs are supported.
 *
 * @author Frank Fock
 * @version 1.4.10
 */ 
class AGENTXPP_DECL AgentXTask: public Runnable {
  public:
	AgentXTask(MasterAgentXMib* m, AgentXPdu* t) 
	  { mib = m; task = t; }
	virtual ~AgentXTask() { delete task; }
	
	virtual void run();
 protected:
	MasterAgentXMib* mib;
	AgentXPdu* task;
};

/*----------------------- class MasterAgentXMib -----------------------*/

/*
 * The thread that receives AgentX requests
 */
#ifdef _WIN32THREADS
void master_daemon(void*);
#else
void* master_daemon(void*);
#endif

/**
 * The MasterAgentXMib class represents the MIB of an AgentX master agent.
 *
 * For security reasons, the AgentX++ master agent does not add a context
 * that does not exists to its MIB registration and the VACM by default.
 * Call the enable_auto_context method to enable auto context creation.
 *
 * @author Frank Fock
 * @version 1.4.10
 */

#if !defined (AGENTXPP_DECL_TEMPL_LIST_THREAD)
#define AGENTXPP_DECL_TEMPL_LIST_THREAD
	AGENTXPP_DECL_TEMPL template class AGENTXPP_DECL List<Thread>;
#endif
#if !defined (AGENTXPP_DECL_TEMPL_LIST_AGENTXREGENTRY)
#define AGENTXPP_DECL_TEMPL_LIST_AGENTXREGENTRY
	AGENTXPP_DECL_TEMPL template class AGENTXPP_DECL List<AgentXRegEntry>;
#endif
#if !defined (AGENTXPP_DECL_TEMPL_LIST_AGENTXINDEXDB)
#define AGENTXPP_DECL_TEMPL_LIST_AGENTXINDEXDB
	AGENTXPP_DECL_TEMPL template class AGENTXPP_DECL OidList<AgentXIndexDB>;
#endif

class AGENTXPP_DECL MasterAgentXMib: public Mib {

  friend class AgentXResponseTask;
  friend class AgentXCloseTask;
  friend class AgentXTask;

 public:

	/**
	 * Default Constructor
	 */
	MasterAgentXMib();

	/**
	 * Constructor with path where persistent MIB objects should be
	 * stored to and loaded from.
	 *
	 * @param path
	 *    the directory to store persistent objects.
	 */
	MasterAgentXMib(const NS_SNMP OctetStr&);  

	/**
	 * Destructor
	 */
	virtual ~MasterAgentXMib();

	/**
	 * Set the AgentXMaster that will process AgentX messages.
	 *
	 * @param agentXMaster
	 *    a pointer to an AgentXMaster instance.
	 */
	virtual void		set_agentx(AgentXMaster*); 

	/**
	 * Get the AgentXMaster used by the receiver to process AgentX
	 * messages.
	 *
	 * @return 
	 *    a pointer to an AgentXMaster instance.
	 */
	AgentXMaster*		get_agentx();

	/**
	 * Set the access mode for agentxSessionAdminStatus.
	 *
	 * @param writable
	 *    if TRUE the agentxSessionAdminStatus MIB object of the 
	 *    AgentX MIB may be written and thus an AgentX session may 
	 *    be closed via SNMP. If FALSE agentxSessionAdminStatus is
	 *    read-only.
	 */
	void			set_session_admin_status_writable(bool);

	/**
	 * Register a MIB object for the agent³s MIB.
	 *
	 * @param item - A pointer to a MibEntry object (i.e., MibLeaf,
	 *               MibTable, MibGroup, ...).
	 * @return The input pointer (if the registered object is of type
	 *         MibGroup 0).
	 */
	virtual MibEntry*    	add(MibEntry*);

	/**
	 * Register a MIB object within a given context. If the given 
	 * context is a new context, it will be added to the list of
	 * known contexts.
	 *
	 * Note: This method is synchronized using ::lock_mib()
	 *
	 * @param context
	 *    an OctetStr instance specifiying the target context.
	 * @param item
	 *    a pointer to a MibEntry object (i.e., MibLeaf,
	 *    MibTable, MibGroup, ...).
	 * @return 
	 *    the input pointer if the MibEntry could be registered, or
	 *    0 otherwise.
	 */
	virtual MibEntry*    	add(const NS_SNMP OctetStr&, MibEntry*);

	/**
	 * Start the master agent thread and initialize master MIB.
	 *
	 * @return 
	 *    TRUE if initialization was successful, FALSE otherwise.
	 */
	virtual bool		init();

	/**
	 * Wait for an incoming AgentX request and process it. 
	 */
	virtual void		process_agentx();   

	/**
	 * Answer an AgentX request. 
	 *
	 * @param pdu
	 *    the PDU to be sent as response
	 */
	virtual void		answer(AgentXPdu&);   

	/**
	 * Check whether the given region would be a duplicate registration
	 * (a registration that would result in a duplicate region with the
	 * same priority and within the same context is termed a duplicate
	 * registration) 
	 *
	 * @param registration
	 *    the registration to test
	 * @return
	 *    TRUE if the test registration would result in a duplicate
	 *    registration
	 */
	virtual bool		is_dupl_reg(const AgentXRegEntry&);

	/**
	 * Add a get-type sub-request to an AgentX transaction.
	 *
	 * @param sessionID
	 * @param transactionID
	 * @param searchRange
	 * @param timeout
	 * @param type
	 * @param repeater
	 * @param repetitions
	 */
	virtual void		add_get_subrequest(u_int, unsigned long,
						   const AgentXSearchRange&,
						   u_char,
						   u_char, const NS_SNMP OctetStr&,
						   int=0, int=0);
	/**
	 * Add a set-type sub-request to an AgentX transaction.
	 *
	 * @param sessionID
	 * @param transactionID
	 * @param searchRange
	 * @param vb
	 * @param timeout
	 * @param type
	 */
	virtual void		add_set_subrequest(u_int, unsigned long,
						   const AgentXSearchRange&,
						   const Vbx&, u_char, 
						   u_char, const NS_SNMP OctetStr&);

	/**
	 * Close a session.
	 *
	 * @param sid
	 *    a session ID.
	 * @param reason
	 *    the reason
	 */
	virtual void		close_session(u_int, u_char);

	/**
	 * Enable or disable automatic context creation. If enabled and the
	 * master receives a registration request for an unknown subagent it 
	 * will add the unknown context to its Mib and to the VACM. The 
	 * default is FALSE (auto context creation disabled).
	 *
	 * @param enable
	 *    If TRUE auto context creation is enabled, otherwise disabled.
	 */
	virtual void		set_auto_context(bool enabled)
					{ autoContext = enabled; }
 
	/**
	 * Gets the upper boundary of the AgentX session that the given
	 * node's active registration belongs to. "Upper boundary" in this
	 * is context is defined as the upper bound (max_key) of the 
	 * AgentXNode with the greatest key following the given one where
	 * all intermediate MibEntry instances are AgentXNodes whose active
	 * registration belongs to the same session as the start node.
	 *
	 * @param context
	 *    the MibContext name of the context to search.
	 * @param node
	 *    a pointer to an AgentXNode that denotes the lower bound of the
	 *    search.
	 * @return
	 *    an Oidx that equals max_key() of the found upper bound 
	 *    AgentXNode. If the search for the upper boundary could not be
	 *    executed, because of concurrent requests that lock the
	 *    Mib instance, then the value of node->max_key() is returned. 
	 */
	virtual Oidx		get_upper_session_boundary(const NS_SNMP OctetStr&, 
							   AgentXNode*);

 protected:
	
	/**
	 * Add a new context to the master. This method is never called 
	 * when auto context creation is disabled (default). Overwrite
	 * this method if you want to dynamically create view and user
	 * configuration for the new context. 
	 *
	 * @param newContext
	 *    an OctetStr representing the new context name.
	 */
	virtual void		add_new_context(const NS_SNMP OctetStr&);

	/**
	 * Get index database for a given context.
	 *
	 * @param context
	 *    a context string (<128 character).
	 * @return
	 *    the index database for the specified context or 0 if
	 *    such a database does not exist.
	 */
	AgentXIndexDB*		get_index_database(const NS_SNMP OctetStr&);

	/**
	 * Timeout old requests and return the time until next request 
	 * will timeout.
	 *
	 * @param timeout
	 *    a time value. All requests with a timeout value less than
	 *    timeout will be deleted from the queue.
	 * @return
	 *    the time until the next request will timeout.
	 */
	virtual time_t		timeout_requests(time_t);

	/**
	 * Process a set request.
	 * 
	 * @param rep
	 *    the set request to process.
	 */
	virtual void	process_set_request(Request*);

        virtual void   	do_process_request(Request*);

	/**
	 * Finalize a request. The finalize method is called if all
	 * sub-requests have been successfully processed.
	 *
	 * @param request
	 *    the request to finalize.
	 */
	virtual void	finalize(Request*);

	/**
	 * Unlocks the supplied request before it calls it corresponding
	 * super class method to actually delete the request.
	 * @param request
	 *    a pointer to the Request to delete.
	 * @since 1.4.12 
	 */
	virtual void    delete_request(Request* req) 
        { if (req->trylock() != Synchronized::BUSY) { req->notify(); 
	      req->unlock(); } Mib::delete_request(req); } 

	/**
	 * Send pending AgentX requests for a given transaction id.
	 *
	 * @param transactionID
	 *    a transaction ID.
	 * @return 
	 *    TRUE if at least one pending request could be found (and sent),
	 *    FALSE otherwise.
	 */
	virtual bool send_pending_ax(unsigned long);

	/**
	 * Cancel pending AgentX requests for a given transaction id.
	 *
	 * @param transactionID
	 *    a transaction ID.
	 * @return 
	 *    TRUE if at least one pending request could be found 
	 *    (and canceled), FALSE otherwise.
	 */
	virtual bool cancel_pending_ax(unsigned long);

	/**
	 * Process AgentX requests.
	 *
	 * @param pdu
	 *    the request PDU.
	 */
	virtual void		process_ax_request(AgentXPdu&);
	virtual void		process_ax_response(AgentXPdu&, bool);
	virtual bool		process_ax_search_response(AgentXPdu&,
							   const AgentXPdu&,
							   int*,
							   Request*);
	
	virtual void		process_ax_set_response(AgentXPdu&,
							const AgentXPdu&,
							int*,
							Request*);
	
	virtual bool		process_ax_getbulk_response(AgentXPdu&,
							    const AgentXPdu&,
							    int*,
							    Request*);
	

	virtual void		process_ax_open(AgentXPdu&);
	virtual void		process_ax_registration(AgentXPdu&);
	virtual void		process_ax_unregistration(AgentXPdu&);
	virtual void		process_ax_notify(AgentXPdu&);
	virtual void		process_ax_master_request(fd_set*, int*);
	virtual void		process_ax_addagentcaps(AgentXPdu&);
	virtual void		process_ax_removeagentcaps(AgentXPdu&);
	virtual void		process_ax_indexallocate(AgentXPdu&);
	virtual void		process_ax_indexdeallocate(AgentXPdu&);

	/**
	 * Create a response and answer it if either the session id
	 * does not correspond to a currently established session or
	 * if the NON_DEFAULT_CONTEXT bit is set and the master agent
	 * only supports the default context.
	 *
	 * @param request
	 *    a AgentX request.
	 * @param session
	 *    returns the session corresponding to the
	 *    session id in request.
	 * @return 
	 *    NULL if one of the above error conditions occured, or
	 *    a pointer to a newly created response PDU.
	 */
	virtual AgentXPdu* create_response(AgentXPdu&, AgentXSession&);

	/**
	 * Get the affected registrations when trying to add a new one
	 *
	 * @param newReg
	 *    a new AgentX registration
	 * @param actualRegion
	 *    the region to be registered. This may be a subregion of
	 *    newReg.region. 
	 * @param list
	 *    all affected MibEntry instances will be added to list
	 * @return
	 *    TRUE if the registration would not affect non AgentX 
	 *    registrations, FALSE otherwise
	 */
	virtual bool		get_affected_regs(const AgentXRegEntry&,
						  const AgentXRegion&,
						  List<AgentXNode>&);
	/**
	 * Check whether a new registration will result in a conflict with
	 * existing registrations
	 *
	 * @param registration
	 *    the registration to test
	 * @return
	 *    TRUE if the registration will result in a conflict, FALSE
	 *    otherwise
	 */
	virtual bool		has_conflict(const AgentXRegEntry&);

	/**
	 * Add a registration entry (a single region or a range) to the MIB
	 *
	 * @param newReg
	 *    a pointer to a new registration entry (do not delete the 
	 *    pointer - this is done by remove_entry())
	 */
	virtual void		add_entry(AgentXRegEntry*);
	
	/**
	 * Remove a registration entry. 
	 * (NOT SYNCHRONIZED)
	 *
	 * @param connectionID
	 *    a connection ID (AgentX MIB)
	 * @param victim
	 *    a registration entry
	 * @return 
	 *    TRUE if the registration entry has been removed, 
	 *    FALSE otherwise
	 */
	virtual bool		remove_entry(u_int, const AgentXRegEntry&);

	/**
	 * Remove all registration entries of a given session.
	 * (SYNCHRONIZED on registrations)
	 *
	 * @param connectionID
	 *    a connection ID (AgentX MIB)
	 * @param sessionID
	 *    a session id.
	 */
	virtual void		remove_entries(u_int, u_int);

	/**
	 * Add Agent Capabilities to a context.
	 *
	 * @param context
	 *    a context. An empty string denotes the default context.
	 * @param sysORID
	 *    an authoritative identification of a capabilities statement
         *    with respect to various MIB modules supported by the local
         *    SNMPv2 entity acting in an agent role.
	 * @param sysORDescr
	 *    a textual description of the capabilities identified by the
         *    corresponding instance of sysORID.
	 * @param session
	 *    the session that will be associated with this capabilities
	 *    registration.
	 * @return 
	 *    TRUE if the context could be found and it contains the
	 *    sysOR MIB objects, FALSE otherwise.
	 */
	virtual bool		add_ax_agent_caps(const NS_SNMP OctetStr&, 
						  const Oidx&,
						  const NS_SNMP OctetStr&,
						  AgentXSession&);

	/**
	 * Remove Agent Capabilities from a context.
	 *
	 * @param context
	 *    a context. An empty string denotes the default context.
	 * @param sysORID
	 *    an authoritative identification of a capabilities statement
         *    with respect to various MIB modules supported by the local
         *    SNMPv2 entity acting in an agent role.
	 * @param session
	 *    the session that is associated with this capabilities
	 *    registration.
	 */
	virtual void		remove_ax_agent_caps(const NS_SNMP OctetStr&, 
						     const Oidx&,
						     AgentXSession&);

 private:

	/**
	 * Remove all allocated indexes for a given session.
	 *
	 * @param session_id
	 *    the session ID of a closing session.
	 */
	void			remove_indexes(u_int);

	void			remove_agent_caps_by_session(AgentXSession*);

	/**
	 * Add a single region registration entry.
	 *
	 * @param newReg
	 *    a pointer to a new registration entry (do not delete the 
	 *    pointer - this is done by remove_entry())
	 * @param region
	 *    the region that should be registrated (differs only from
	 *    newReg's region, if newReg is a range)
	 */
        void			add_entry(AgentXRegEntry*, 
					  const AgentXRegion&);

	/**
	 * Close all sessions.
	 */
        void			close_all_sessions();

	/**
	 * Remove peer
	 */
	void			remove_peer(const AgentXPeer&);

	void			lock_reg();
	void			unlock_reg();

	void			dispatch_ax_close(AgentXPeer*, AgentXPdu*);
	void			dispatch_ax_response(AgentXPdu&, bool);
	void			dispatch_ax_unregistration(AgentXPdu&);
	void			dispatch_ax_registration(AgentXPdu&);

	AgentXSessions		openSessions;

	List<AgentXRegEntry>	registrations;
	Synchronized		registrationLock;

	OidList<AgentXIndexDB>  indexDatabase;		
	Synchronized		indexLock;

	ThreadPool*		axThread;
	QueuedThreadPool*      	axRegThread;
	QueuedThreadPool*      	axBulkThread;
	QueuedThreadPool*      	axSetThread;
	AgentXMaster*		agentx;

	bool			autoContext;
};

/*----------------------- class AgentXThread -----------------------*/

/**
 * The AgentXThread implements the AgentX processing routine of
 * a AgentX++ master agent.
 *
 * @version 1.4
 * @author Frank Fock
 */

class AGENTXPP_DECL AgentXThread: public Runnable {
 public:
	/**
	 * Create an AgentXThread that uses the given
	 * MasterAgentXMib instance to process AgentX requests.
	 *
	 * @param mib
	 *    a pointer to a MasterAgentXMib instance. The instance
	 *    will not be deleted by AgentXThread.
	 */ 
	AgentXThread(MasterAgentXMib* m) { master = m; }

	/**
	 * Destructor.
	 */
	virtual ~AgentXThread() { }
	
	/**
	 * Start AgentX request processing.
	 */
	virtual void run();

 protected:
	MasterAgentXMib* master;
};

#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
