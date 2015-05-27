/*_############################################################################
  _## 
  _##  agentx_subagent.h  
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

#ifndef agentx_subagent_h_
#define agentx_subagent_h_


#include <agent_pp/mib.h>
#include <agentx_pp/agentx.h>
#include <agentx_pp/agentx_request.h>
#include <agentx_pp/agentx_index.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


typedef enum {
  AGENTX_STATUS_ERROR    = 0,
  AGENTX_STATUS_CONNECT  = 1,
  AGENTX_STATUS_OPEN     = 2,
  AGENTX_STATUS_REGISTER = 3,
  AGENTX_STATUS_RUN      = 4,
  AGENTX_STATUS_CLOSED   = 5
} agentx_run_status_t;


typedef enum {
    /**
     * Do not allocate any indexes at the master agent.
     */
    noIndexAllocation = 1,
    /**
     * Only allocate an index value for the first sub-index of a row at the
     * master agent. This is the recommended strategy for most use cases.
     * Index values already allocated are not allocated again. A reference
     * count is being hold to guarantee proper deallocation.
     */
    firstSubIndexOnly,
    /**
     * Allocate for all sub-indexes of a row index as long as the sub-index
     * value has not yet been allocated by this subagent instance.
     */
    anyNonAllocatedSubIndex,
    /**
     * Always allocate the first sub-index only and do not hold a local reference
     * count for allocated sub-indexes.
     */
    alwaysFirstSubIndex,
    /**
     * Always allocate any sub-index values and do not hold a local reference
     * count for allocated sub-indexes.
     */
    alwaysAnySubIndex   
            
} agentx_index_strategy;


class SubAgentXMib;

/*--------------------- class AgentXSharedTable ----------------------*/

/**
 * The AgentXSharedTable class is derived from MibTable and represents
 * a shared table between AgentX subagents. The AgentXSharedTable only
 * registers table rows instead of a whole table. Thus, other subagents
 * may also register rows within the same conceptual table.
 *
 * Do not add rows to this table before the SubAgentXMib::init() method
 * has been called, because allocating an index or adding (registering) 
 * a row requires AgentX communication with the master agent.
 *
 * Before adding a row by using the add_row method, the index of the
 * new row should be allocated by allocate_index. A row with a new or
 * any index may be added by calling new_index or any_index, 
 * respectively. The row_added method should be overriden, in order to
 * set the values of the new row. The row_added method is called 
 * whenever a new row has been successfully registered with the master.
 *
 * @author Frank Fock
 * @version 1.4.11
 */

class AGENTXPP_DECL AgentXSharedTable: public MibTable {
 public:
	/** 
	 * Copy constructor.
	 */
	AgentXSharedTable(const AgentXSharedTable&);

	/**
	 * Constructor with object identifier and given index structure.
	 * In AGENT++ the structure of a table's index is given by an array
	 * of integer values. Each element represents an object of the index.
	 * A value of -1 represents a implied variable length string or oid.
	 * A value of 0 a variable length string or oid index object.
	 * A value greater than 0 determines the length of the index object 
	 * meassured in subidentifiers. 
	 * 
	 * @param o
	 *    the object identifier of the table, which has to be
	 *    the oid of the the SMI table entry object (table.1).
	 * @param index
	 *    an integer array. The length of the array corresponds to
	 *    the number of objects in the INDEX clause.
	 * @param size
	 *    the size of the above index array.
	 * @param indexOIDs
	 *    an array of size "size" with the OIDs of the index 
	 *    objects.
	 * @param backReference
	 *    a pointer to a SubAgentXMib instance.
	 * @param context
	 *    the context the shared table belongs to. An empty string
	 *    denotes the default context.
	 */
	AgentXSharedTable(const Oidx&, const index_info*, unsigned int,
			  const Oidx*, SubAgentXMib*, const NS_SNMP OctetStr&);

	/**
	 * Destructor.
	 */
	virtual ~AgentXSharedTable();

	/**
	 * Add a row with the given index to the table.
	 * by cloning the generator row and setting the oids accordingly. 
	 * Unlike MibTable the row_added is only called when the 
	 * registration of the row at the master was successful.
	 * The call of MibTable::row_added is synchronized with the table.
	 * This is different to MibTable::row_added which is not 
	 * synchronized by AGENT++.
	 *
	 * @param ind 
	 *    the index of the new row.
	 * @return 
	 *    a pointer to the added row.
	 */
	MibTableRow*   	add_row(const Oidx&);

	/**
	 * Remove a row with the given index from the table. Before 
	 * row_delete is called, the row will be unregistered at the
	 * master.
	 *
	 * @param ind 
	 *    the index of the row.
	 */
	void	       	remove_row(const Oidx&);

	/**
	 * Initialize a row with values. This method is typically called
	 * when a row is loaded from persistent storage. In contrast
	 * to the corresponding method of MibTable, this method allocates
	 * the index at the master agent.
	 *
	 * @param index
	 *   the index of the row.
	 * @param vbs
	 *   a pointer to an array of variable bindings that provides
	 *   initial values for all columns in the newly created row.
	 *   The size of the array must be the same as the size of the row.
	 * @return 
	 *   a pointer to the added row or 0 if the index is not valid.
	 */
	virtual MibTableRow*   	init_row(const Oidx&, Vbx*);

	/**
	 * Request a new index from the master agent. This requests
	 * an index that must not have been allocated (even if sub-
	 * sequently released) to any subagent since the last re-
	 * initialization of the master agent.
	 * If a new index is available it is returned by calling the
	 * callback method index_allocated.  
	 */
	virtual void		new_index();

	/**
	 * Request any index from the master agent. This requests an
	 * index that must not be currently allocated to any other 
	 * subagent.
	 * If a new index is available it is returned by calling the
	 * callback method index_allocated.
	 */
	virtual void		any_index();

	
	/**
	 * Allocate an index at the master. The index value is checked
	 * before it is send to the master. If the index is invalid 
	 * for this table, FALSE will be returned and no index allocation
	 * will take place. If the index is valid, then an allocation
	 * request will be send to the master. If the request has been
	 * successfully processed, then index_allocated will be called
	 * with error status set to AGENTX_OK, otherwise the error status
	 * will be set to AGENTX_INDEX_ALREADY_ALLOCATED or
	 * AGENTX_INDEX_NONE_AVAILABLE and the error index value will be
	 * set to the subindex that failed. 
	 *
	 * @param index
	 *    an object identifier representing the whole index of
	 *    a new row.
	 */
	virtual bool	allocate_index(const Oidx&);

	/**
	 * This method is being called, whenever an index allocation
	 * has been processed by the master and the result has been
	 * returned to the subagent.
	 *
	 * @param index
	 *    the row index of the row for which the index allocation
         *    succeeded.
	 * @param error
	 *    an AgentX error code if the index allocation failed, or
	 *    0 if it has been completed successfully.
	 * @param errorIndex
	 *    if error is not 0, errorIndex points to the index object
	 *    within the index that actually failed (starting from 1).
         * @param vbs
         *    the variable bindings array of the AgentX index allocation 
         *    response PDU which reflect the actually allocated (or failed) 
         *    sub-index values.
         * @param vbs_length
         *    the length of the vbs array.
	 */ 
	virtual void	index_allocated(const Oidx&, int, int, 
                                        Vbx* vbs=0, u_int vbs_length=0);

	/**
	 * Get an index as an object identifier from a list of index
	 * objects.
	 *
	 * @param vbs
	 *    an array of variable bindings. Each variable binding 
	 *    represents an index object of this table. The order 
	 *    of the vbs must be the same as in the table's INDEX
	 *    clause.
	 * @param size
	 *    the size of the above array.
	 * @return
	 *    an object identifier representing an index of this 
	 *    table.
	 */
	Oidx            get_index_from_vbs(Vbx*, u_int);

	/**
	 * Get the SubAgentXMib instance for the table.
	 *
	 * @return
	 *    a pointer to a SubAgentXMib instance.
	 */
	SubAgentXMib*   get_subagent_mib() { return backReference; }

	/**
	 * Set the timeout value characteristics for this group
	 * of MIB objects. Within AGENT++ this value is currently
	 * not used, but AgentX++ is using this value to set
	 * the region timeout when registering a subagent. Future
	 * implementations may also use this value for setting
	 * region (or subtree) timeouts.
	 *
	 * @param timeout
	 *    a timeout value in seconds. 0 sets the systems default
	 *    timeout.
	 * @since 3.5
	 */
	void			set_timeout(unsigned int t) { timeout = t; }

	/**
	 * Get the timeout (in seconds) for this MIB group.
	 *
	 * @return
	 *    a timeout value in seconds or 0 if default timeout should
	 *    be used.
	 * @since 3.5
	 */ 
	unsigned int		get_timeout() { return timeout; }

	/**
	 * Removes all rows from this table.
	 */
	virtual void		clear();

	/**
	 * Reallocates any indexes and reregisters any rows of this table.
         * Using the currently configured index allocation strategy.
	 */
	virtual void		reinit();
        
        /**
         * Sets the index allocation strategy to be used by this shared
         * table.
         * For most use cases the {@link agentx_index_strategy#firstSubIndexOnly} 
         * strategy is recommended. With that strategy, only the first 
         * sub-index value will be allocated at the master agent if it has not
         * yet been allocated by this table support (for any table).
         * Unless you need to allocated sub-indexes by a multi-sub-index table, 
         * because there does not exists a shared table for the same session 
         * with that sub-index as first sub-index value, you should always use
         * the {@link agentx_index_strategy#firstSubIndexOnly} or 
         * {@link agentx_index_strategy#anyNonAllocatedSubIndex}.
         * The {@link agentx_index_strategy#anyNonAllocatedSubIndex} should 
         * be used for a master and dependent table combination where the 
         * dependent table(s) extend the index of the master table by 
         * additional sub-indexes.
         * 
         * @param is
         *    the new index allocation/deallocation strategy.
         * @since 4.0.2
         */
        void                    set_index_strategy(agentx_index_strategy is)
                                        { indexStrategy = is; }
        
        /**
         * Gets the current index allocation strategy. 
         * See {@link #set_index_strategy(agentx_index_strategy)}.
         * @return
         *    the index allocation strategy as defined by the 
         *   {@link agentx_index_strategy} enum.
         * @since 4.0.2
         */
        agentx_index_strategy get_index_strategy() { return indexStrategy; }
	
 protected:

	void		get_index_value(NS_SNMP SmiUINT32, const Oidx&, Vbx& vb);
	Vbx*		create_index_vbs(const Oidx&);

	NS_SNMP OctetStr	myContext;
	Oidx*		indexOIDs;
	SubAgentXMib*   backReference;
	unsigned int	timeout;
        
        agentx_index_strategy   indexStrategy;
};


/*------------------ class AgentXPendingIndex ------------------------*/

/**
 * The AgentXPendingMessage class represents the mapping between a 
 * packet ID and a AgentXSharedTable instance.
 * For index allocation, a row index can be provided to be able to
 * allocate the original requested row index value instead of the
 * allocated sub-index.
 * 
 * @author Frank Fock
 * @version 2.1
 */

class AGENTXPP_DECL AgentXPendingMessage {
 public:

  AgentXPendingMessage(u_int pid, AgentXSharedTable* o): 
    packet_id(pid) { receiver = o; row_index = NULL; }
  AgentXPendingMessage(u_int pid, AgentXSharedTable* o, Oidx* alloc_index): 
    packet_id(pid) { receiver = o; row_index = alloc_index; }
    
  ~AgentXPendingMessage() 
        { if (row_index) { delete row_index; } }

  u_int			packet_id;
  AgentXSharedTable*	receiver;
  Oidx*                 row_index;
};


/*-------------------------- class SubAgentXMib -----------------------*/

/**
 * The SubAgentXMib class represents the MIB of an AgentX subagent. 
 *
 * @author Frank Fock
 * @version 1.4.11
 */
#if !defined (AGENTXPP_DECL_TEMPL_LIST_AGENTXPENDINGMESSAGE)
#define AGENTXPP_DECL_TEMPL_LIST_AGENTXPENDINGMESSAGE
	AGENTXPP_DECL_TEMPL template class AGENTXPP_DECL List<AgentXPendingMessage>;
#endif

class AGENTXPP_DECL SubAgentXMib: public Mib {

    friend class AgentXResponse;

 public:

	/**
	 * Default Constructor
	 */
	SubAgentXMib();

	/**
	 * Constructor with path where persistent MIB objects should be
	 * stored to and loaded from.
	 *
	 * @param path
	 *    the directory to store persistent objects.
	 */
	SubAgentXMib(const NS_SNMP OctetStr&);  

	/**
	 * Destructor
	 */
	virtual ~SubAgentXMib();

	/**
	 * Initialize the sub-agent by opening a session with the master
	 * agent and registering all MIB objects already added to the
	 * receiver.
	 *
	 * @return
	 *    TRUE if connection to master could be established,
	 *    FALSE otherwise.
	 */
	virtual bool		init();

	/**
	 * Register a MIB object for the agent's MIB. If the subagent's
	 * connection to the master is established, then the object will
	 * be immediately registered at the master.
	 *
	 * @param item
	 *     a pointer to a MibEntry object (i.e., MibLeaf,
	 *     MibTable, MibGroup, ...).
	 * @return The input pointer (if the registered object is of type
	 *         MibGroup 0).
	 */
	virtual MibEntryPtr    	add(MibEntryPtr);

	/**
	 * Register a MIB object for the agent's MIB. If the subagent's
	 * connection to the master is established, then the object will
	 * not be registered at the master, otherwise it will be registered
	 * through the reconnect method.
	 *
	 * @param item
	 *     a pointer to a MibEntry object (i.e., MibLeaf,
	 *     MibTable, MibGroup, ...).
	 * @return 
	 *     the input pointer (if the registered object is of type
	 *     MibGroup 0).
	 */
	virtual MibEntryPtr    	add_no_reg(MibEntryPtr);

	/**
	 * Register a MIB object within a given context. If the given 
	 * context is a new context, it will be added to the list of
	 * known contexts. If the subagent's connection to the master
	 * is established, then the object will be immediately 
	 * registered at the master.
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
	 * Register a MIB object within a given context at the master. 
	 * If the given context is a new context, it will be added 
	 * to the list of known contexts. 
	 * Note that the subagent's connection to the master must be 
	 * established before this method is called, otherwise this 
	 * method will return 0.
	 *
	 * Note: This method is synchronized using ::lock_mib()
	 *
	 * @param context
	 *    an OctetStr instance specifiying the target context.
	 * @param item
	 *    a pointer to a MibEntry object (i.e., MibLeaf,
	 *    MibTable, MibGroup, ...).
	 * @param timeout
	 *    the entry's (region's) timeout value.
	 * @return 
	 *    the input pointer if the MibEntry could be registered, or
	 *    0 otherwise.
	 */
	virtual MibEntry*    	register_entry(const NS_SNMP OctetStr&, 
					       MibEntry*,
					       u_int);

	/**
	 * Register a MIB object within a given context. If the given 
	 * context is a new context, it will be added to the list of
	 * known contexts. If the subagent's connection to the master
	 * is established, then the object will not be registered at
	 * the master, otherwise it will be registered through the
	 * reconnect method.
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
	virtual MibEntry*    	add_no_reg(const NS_SNMP OctetStr&, MibEntry*);

	/** 
	 * Unregister a MIB object from the agent's MIB.
	 * This method now (>3.3) works for MibGroups, too.
	 *
	 * Note: This method is synchronized. It first acquires 
	 * ::mib_lock(), then searches for the target MIB object and
	 * then acquires target::start_synch().
	 * Then it RELEASES the MIB object lock by calling 
	 * target::end_synch(). The target object is deleted and the
	 * MIB lock is released.
	 *
	 * Why is this save?
	 * - we hold the lock for the MIB (lock_mib())
	 * - before a request can lock a MibEntry object it
         *   must get the MIB lock
	 * - so if we get the MibEntry lock here no other request
         *   can be currently using this object 
         * - even if we release the lock, because we hold the
         *   MIB lock
	 *
	 * @param oid
	 *    the Oidx instance pointing to the key of the MibEntry
	 *    to be removed.
	 * @return 
	 *    TRUE if the entry could be removed, FALSE otherwise
	 */
	virtual bool		remove(const Oidx&);

	/** 
	 * Unregister a MIB object from the agent's MIB, but do not
	 * unregister it with the master. Use this method when the
	 * object to be removed has been registred by add_no_reg().
	 *
	 * @param oid
	 *    the Oidx instance pointing to the key of the MibEntry
	 *    to be removed.
	 * @return 
	 *    TRUE if the entry could be removed, FALSE otherwise
	 */
	virtual bool		remove_no_unreg(const Oidx&);

	/** 
	 * Unregister a MIB object from the agent³s MIB.
	 * This method now (>3.3) works for MibGroups, too.
	 *
	 * Note: This method is synchronized. It first acquires 
	 * ::mib_lock(), then searches for the target MIB object and
	 * then acquires target::start_synch().
	 * Then it RELEASES the MIB object lock by calling 
	 * target::end_synch(). The target object is deleted and the
	 * MIB lock is released.
	 *
	 * Why is this save?
	 * - we hold the lock for the MIB (lock_mib())
	 * - before a request can lock a MibEntry object it
         *   must get the MIB lock
	 * - so if we get the MibEntry lock here no other request
         *   can be currently using this object 
         * - even if we release the lock, because we hold the
         *   MIB lock
	 *
	 * @param context
	 *    an OctetStr instance specifiying the target context.
	 * @param oid
	 *    the Oidx instance pointing to the key of the MibEntry
	 *    to be removed.
	 * @return 
	 *    TRUE if the entry could be removed, FALSE otherwise
	 */
	virtual bool		remove(const NS_SNMP OctetStr&, const Oidx&);

	
	/** 
	 * Unregister a MIB object from the given context, but do not
	 * unregister it with the master. Use this method when the
	 * object to be removed has been registred by add_no_reg().
	 *
	 * @param context
	 *    an OctetStr instance specifiying the target context.
	 * @param oid
	 *    the Oidx instance pointing to the key of the MibEntry
	 *    to be removed.
	 * @return 
	 *    TRUE if the entry could be removed, FALSE otherwise
	 */
	virtual bool		remove_no_unreg(const NS_SNMP OctetStr&, const Oidx&);


	/**
	 * Open session.
	 */
	virtual int		open_session();

	/**
	 * (Re)Connect with the master. Allocates regions for the objects
	 * of this Mib instance and also allocates indexes and rows of shared
	 * tables.
	 */
	virtual void		reconnect();

	/**
	 * Set the default priority for registrations by this sub-agent.
	 *
	 * @param priority
	 *    a value between 0 and 255 (default is 127).
	 */
	void			set_default_priority(const u_char p) 
							{ priority = p; }

	/**
	 * Set the RequestList to be used for answering request.
	 * Note: The request list is NOT deleted by the receiver's destructor.
	 *       This should be done outside class Mib.
	 *
	 * @param requestList
	 *    an AgentXRequestList instance with its agentx protocol service
	 *    set. (If the agentx protocol service is not set, this will end
	 *    program execution)
	 */
#ifndef STATIC_REQUEST_LIST
	void			set_request_list(AgentXRequestList*);
#else
	static void		set_request_list(AgentXRequestList*);
#endif
	/**
	 * Send a notification.
	 *
	 * @param context
	 *    the context originating the notification ("" for the default 
	 *    context).
	 * @param trapoid
	 *    the oid of the notification.
	 * @param vbs
	 *    an array of variable bindings.
	 * @param size
	 *    the size of the above variable binding array.
	 * @param timestamp
	 *    an optional timestamp.
	 * @return
	 *    SNMP_ERROR_SUCCESS.
	 */
	virtual int		notify(const NS_SNMP OctetStr&, const Oidx&,
				       Vbx*, int, unsigned int=0); 

	/**
	 * Get the AgentXSlave instance used for AgentX services.
	 *
	 * @return
	 *    an AgentXSlave instance.
	 */
	static AgentXSlave*    	get_agentx() { return agentx; }

	/**
	 * Send a ping PDU to the master agent to monitor the master 
	 * agent's ability to receive and send AgentX PDUs over their 
	 * AgentX session.
	 * 
	 * @return
	 *    a transaction id to match ping requests to responses later.
	 */
	virtual unsigned long	ping_master();

	/**
	 * This method is called when this subagent received a PING 
	 * response from its master.
	 *
	 * @param pdu
	 *    the AgentX PDU containing the response for the ping.
	 */
	virtual void		ping_received(const AgentXPdu&);

	/**
	 * Register a region at the master agent.
	 *
	 * @param context
	 *    the context.
	 * @param region
	 *    an AgentXRegion instance.
	 * @param timeout
	 *    the regions timeout value or 0 for using the sessions
	 *    timeout value.
	 * @param mode
	 *    if TRUE the region is registered at the master,
	 *    if FALSE the region is deregistered.
	 * @param reference
	 *    a pointer to an AgentXSharedTable instance that initiates
	 *    the registration. If the registration attempt is initiated 
	 *    by any other MIB object instance, then this paramter 
	 *    should be set to 0 or omitted.
	 * @return
	 *    TRUE if the registration message has been successfully sent,
	 *    FALSE otherwise.
	 */
	virtual bool		register_region(const NS_SNMP OctetStr&,
						const AgentXRegion&,
						u_int timeout,
						bool,
						AgentXSharedTable* t=0);

	/**
	 * Registration failed. This method is called when a registration 
	 * attempt failed.
	 *
	 * @param region
	 *    the region.
	 * @param errorStatus
	 *    the error status of the attempt.
	 * @param reference
	 *    a pointer to the AgentXSharedTable instance that initiated the
	 *    registration. This parameter is 0 for other registrations. 
	 */
	virtual void		registration_failed(const AgentXRegion&, int,
						    AgentXSharedTable*){}

	/**
	 * Registration success.  This method is called when a registration 
	 * attempt succeeded.
	 *
	 * @param region
	 *    the region.
	 * @param reference
	 *    a pointer to the AgentXSharedTable instance that initiated the
	 *    registration. This parameter is 0 for other registrations. 
	 */
	virtual void		registration_success(const AgentXRegion&,
						     AgentXSharedTable*);

	/**
	 * Allocate a set of index values.
	 *
	 * @param context
	 *    a context. "" denotes the default context.
	 * @param indexValues
	 *    an array of Vbx instances. The oid portion denotes the oid
	 *    of the specified index object and the value portion denotes
	 *    the value of that index object.
	 * @param size
	 *    the size of the above array.
	 * @param reference
	 *    a pointer to an AgentXSharedTable instance association with
	 *    this index allocation. Default for this parameter is 0.
	 * @param indexType
	 *    the type of index to be allocated. Possible values are:
	 *    0                 - index values are given by indexValues
	 *    AGENTX_NEW_INDEX  - allocate unique new index values
	 *    AGENTX_ANY_INDEX  - allocate any new index values
         * @param rowIndex
         *    the (optional) row index for which index values are allocated.
         *    Because index values can be allocated for one or more sub-indexes  
         *    of a row index, the rowIndex value is not necessarily equal to
         *    the indexValues.
         *    The rowIndex will be deleted when the corresponding AgentX packet
         *    is deleted (or if the AgentX request could not be sent).
         *    Thus, the caller does not need to delete this pointer nor may the
         *    caller use it after the call anymore!
         * @return 
         *    the AgentX packet ID which can be used to associate
         *    context information when the corresponding response has been
         *    received.
	 */
	virtual u_int		allocate_index(const NS_SNMP OctetStr&,
					       Vbx*, int, 
					       AgentXSharedTable* t=0,
					       u_char indexType=0,
                                               Oidx* rowIndex=0);

	/**
	 * Deallocate a set of index values.
	 *
	 * @param context
	 *    a context. "" denotes the default context.
	 * @param indexValues
	 *    an array of Vbx instances. The oid portion denotes the oid
	 *    of the specified index object and the value portion denotes
	 *    the value of that index object.
	 * @param size
	 *    the size of the above array.
	 * @param reference
	 *    a pointer to an AgentXSharedTable instance association with
	 *    this index allocation. Default for this parameter is 0.
         * @param rowIndex
         *    the (optional) row index for which index values are allocated.
         *    Because index values can be allocated for one or more sub-indexes  
         *    of a row index, the rowIndex value is not necessarily equal to
         *    the indexValues.
         *    The rowIndex will be deleted when the corresponding AgentX packet
         *    is deleted (or if the AgentX request could not be sent).
         *    Thus, the caller does not need to delete this pointer nor may the
         *    caller use it after the call anymore!
         * @return 
         *    the AgentX packet ID of the AgentX request which can be used to 
         *    associate context information when the corresponding response 
         *    has been received.
	 */
	virtual u_int		deallocate_index(const NS_SNMP OctetStr&,
						 Vbx*, int, 
						 AgentXSharedTable* table=0,
                                                 Oidx* rowIndex=0);

	/**
	 * This method is being called, whenever an index allocation
	 * has been processed by the master and the result has been
	 * returned to the subagent.
	 *
	 * @param error
	 *    an AgentX error code if the index allocation failed, or
	 *    0 if it has been completed successfully.
	 * @param errorIndex
	 *    if error is not 0, errorIndex points to the index object
	 *    within the index that actually failed (starting from 1).
	 * @param vbs
	 *    an array of index objects.
	 * @param size
	 *    the size of the above array.
	 * @param sharedTable
	 *    a pointer to an AgentXSharedTable instance that initiated 
	 *    the index allocation. If the index allocation was not 
	 *    initiated by an AgentXSharedTable instance this pointer
	 *    will be 0.
         * @param packetID
         *    the packet ID of the AgentX request that send the allocation
         *    PDU. This value can be used to associate any objects with
         *    the (asynchronous) index allocation request.
         * @param rowIndex
         *    the row index originally provided to the allocation request.
	 */
	virtual void            index_allocated(u_int, u_int, 
						Vbx*, int, AgentXSharedTable*,
                                                u_int, Oidx*);

	/**
	 * This method is being called, whenever an index deallocation
	 * has been processed by the master and the result has been
	 * returned to the subagent.
	 *
	 * @param error
	 *    an AgentX error code if the index deallocation failed, or
	 *    0 if it has been completed successfully.
	 * @param errorIndex
	 *    if error is not 0, errorIndex points to the index object
	 *    within the index that actually failed (starting from 1).
	 * @param vbs
	 *    an array of index objects.
	 * @param size
	 *    the size of the above array.
	 * @param sharedTable
	 *    a pointer to an AgentXSharedTable instance that initiated 
	 *    the index deallocation. If the index deallocation was not 
	 *    initiated by an AgentXSharedTable instance this pointer
	 *    will be 0.
         * @param packetID
         *    the packet ID of the AgentX request that send the allocation
         *    PDU. This value can be used to associate any objects with
         *    the (asynchronous) index allocation request.
         * @param rowIndex
         *    the row index originally provided to the allocation request.
	 */
	virtual void            index_deallocated(u_int, u_int,
						  Vbx*, int, 
						  AgentXSharedTable*,
                                                  u_int,
                                                  Oidx*) {}

	/**
	 * Add agent capabilities to the sysORTable of the master.
	 *
	 * @param context
	 *    a context. Use "" for the default context.
	 * @param id
	 *    an object identifier containing the value of an invocation
	 *    of the AGENT-CAPABILITIES macro, which the master agent
	 *    exports as a value of sysORID for the indicated context.
	 * @param descr
	 *    an OctetStr containing a DisplayString to be used as
	 *    the value of sysORDescr corresponding to the sysORID value
	 *    above.
	 * @return 
	 *    TRUE if the context could be found.
	 */
	virtual bool		add_agent_caps(const NS_SNMP OctetStr&,
					       const Oidx&,
					       const NS_SNMP OctetStr&);

	/**
	 * Add agent capabilities to the sysORTable of the master.
	 *
	 * @param context
	 *    a context. Use "" for the default context.
	 * @param id
	 *    an object identifier containing the value of sysORID
	 *    that should be no longer exported.
	 */
	virtual void		remove_agent_caps(const NS_SNMP OctetStr&,
						  const Oidx&);

	/**
	 * Gets a pointer to the AgentXSession associated with this 
	 * SubAgentXMib instance.
	 * 
	 * @return 
	 *    a pointer to an AgentXSession instance. The content
	 *    of the session should not be modified. In addition,
	 *    the memory allocated for the session object may be
	 *    invalidated when the session is closed.
	 */ 
	AgentXSession*		get_session() { return session; }
        
        /**
         * Gets a pointer to the index registry associated with this 
         * subagent. The index registry records index registrations
         * to avoid duplicate registrations for some index allocation
         * strategies. 
         * @return 
         *    a pointer to the internal index registry.
         */
        AgentXIndexDB*          get_index_registry() { return indexRegistry; }

 protected:

	/** 
	 * Create a session that will be used for this Mib instance.
	 * Override this method to create your own session object,
	 * i.e., with a non-default timeout value.
	 *
	 * @return 
	 *    a pointer to an AgentXSession instance. It will be deleted
	 *    by SubAgentXMib when session is closed.
	 */
	virtual AgentXSession* create_session();
	
	/**
	 * This method is called when an attempt has been made to
	 * send an AgentX message to the master agent. The default
	 * implementation logs a warning message only.
	 */
	virtual void		not_connected();

	/**
	 * Find the next MIB entry which has an oid greater than the given oid.
	 * (Respects AgentX search ranges) 
	 *
	 * @param context
	 *    the target context.
	 * @param oid
	 *    an object identifier.
	 * @param entry 
	 *    will be changed to a pointer to the found entry or remain 
	 *    unchanged if the return value is different from 
	 *    SNMP_ERROR_SUCCESS.
	 * @param request
	 *    a pointer to the Request instance for in whose context the
	 *    find_next is issued.
         * @param nextOid
         *    returns the identified successor OID for complex entries.
         *    For mib entries where the next OID cannot be determined, the
         *    value will remain unchanged.
	 * @return 
	 *    SNMP_ERROR_SUCCESS if an entry has been found and 
	 *    sNMP_SYNTAX_NOSUCHOBJECT otherwise.
	 */
	virtual int    	find_next(MibContext*, const Oidx&, MibEntryPtr&, 
				  Request* req, const int, Oidx& nextOid);

	/**
	 * Process an AgentX request. 
	 *
	 * @param req 
	 *    a request.
	 */
	virtual void   	do_process_request(Request*);      

	/**
	 * Process an AgentX response.
	 *
	 * @param req 
	 *    a request.
	 */
	virtual void   	process_response(AgentXRequest*);      

	/**
	 * Synchronize local sysUpTime with the given time.
	 *
	 * @param time
	 *    the 1/100 seconds since the master system started.
	 */
	virtual void	synchronize_uptime(u_int);

	
	/**
	 * Remove pending messages for a MibEntry instance.
	 *
	 * @param entry
	 *    a pointer to a MibEntry instance.
	 */
	void		remove_pending_messages(MibEntry*);


	AgentXPendingMessage*   get_pending_message(u_int);

	/**
	 * Create and initialize response pool and queue.
	 */
	virtual void	init_response_pool();


	/**
	 * Deletes the supplied request unless it is a
	 * TestSet or CommitSet PDU.
	 * @param req
	 *    the Request to delete. 
	 * @since 1.4.12
	 */
	virtual void	delete_request(Request* req);

	static AgentXSlave*    	agentx;
	AgentXSession*		session;
	agentx_run_status_t	status;
	u_char			priority;
        
        AgentXIndexDB*          indexRegistry;
        

	List<AgentXPendingMessage> pendingMessages;
	Synchronized		pendingMessagesLock;
#ifdef _THREADS
	QueuedThreadPool*      	responsePool;
#endif
};

#ifdef AGENTPP_NAMESPACE
}
#endif


#endif
