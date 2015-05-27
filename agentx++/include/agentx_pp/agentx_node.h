/*_############################################################################
  _## 
  _##  agentx_node.h  
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


#ifndef _agentx_node_h_
#define _agentx_node_h_

#include <agentx_pp/agentx_reg.h>
#include <agentx_pp/agentx_pdu.h>
#include <agent_pp/mib.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

/*----------------------- class AgentXNode ---------------------------*/


class MasterAgentXMib;

class AgentXNode: public MibEntry {

friend class MasterAgentXMib;

 public:
	/**
	 * Create an AgentX registration
	 *
	 * @param regEntry
	 *    the first registration entry associated with this region
	 *    (includes the session that registrates the region)
	 * @param region
	 *    the region to registrate - can differ from the original 
	 *    region registration contained in regEntry (region splitting) 
	 */
	AgentXNode(AgentXRegEntry*, const AgentXRegion&);

	/**
	 * Copy constructor.
	 *
	 * @param other
	 *    another AgentXNode instane.
	 */
	AgentXNode(const AgentXNode&);

	/**
	 * Destructor.
	 */
	virtual ~AgentXNode();

	/**
	 * Add a registration for the same region but with different
	 * priority
	 *
	 * @param regEntry
	 *    a registration entry that also registrates exactly the
	 *    receiver region
	 */
	virtual void	add_registration(AgentXRegEntry*); 

	/**
	 * Remove a registration for the same region but with different
	 * priority
	 *
	 * @param regEntry
	 *    a registration entry 
	 * @return
	 *    TRUE if there are no more registrations left for the receiver, 
	 *    FALSE otherwise.
	 */
	virtual bool	remove_registration(const AgentXRegEntry&); 

	/**
	 * Shrink the registered region 
	 *
	 * @param newUpperBound
	 *    an object identifier that denotes the new upper bound of
	 *    the region 
	 * @return 
	 *    TRUE if (key() <= newUpperBound <= max_key()), FALSE otherwise
	 */
	virtual bool		shrink(const Oidx&);

	/**
	 * Expand the registered region 
	 *
	 * @param newUpperBound
	 *    an object identifier that denotes the new upper bound of
	 *    the region 
	 * @return 
	 *    TRUE if (key() <= newUpperBound >= max_key()), FALSE otherwise
	 */
	virtual bool		expand(const Oidx&);

	/**
	 * Return the type of the receiver MIB node.
	 *
	 * @return One of the following types: NONE, SHADOW, PROXY, LEAF, 
	 *         TABLE, GROUP, or COMPLEX.
	 */
	virtual mib_type	type() const;

	/**
	 * Return a clone of the receiver.
	 *
	 * @return A pointer to a clone of the MibEntry object.  
	 */
	virtual MibEntryPtr	clone();


 	// interfaces dispatch table <-> management instrumentation
	
	/**
	 * Let the receiver process a SNMP GET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP GET request.
	 * @param ind - The index of the subrequest to be processed.
	 */
	virtual void		get_request(Request*, int);

	/**
	 * Let the receiver process a SNMP GETNEXT subrequest
	 * 
	 * @param req - A pointer to the whole SNMP GETNEXT request.
	 * @param ind - The index of the subrequest to be processed.
	 */
	virtual void		get_next_request(Request*, int);
	
	/**
	 * Let the receiver commit a SNMP SET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP SET request.
	 * @param ind - The index of the subrequest to be processed.
	 * @return SNMP_ERROR_SUCCESS on success and 
	 *         SNMP_ERROR_COMITFAIL on failure.
	 */
	virtual int		commit_set_request(Request*, int);

	/**
	 * Let the receiver prepare a SNMP SET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP SET request.
	 * @param ind - The index of the subrequest to be processed.
	 * @return SNMP_ERROR_SUCCESS on success and SNMP_ERROR_WRONG_VALUE,
	 *         SNMP_ERROR_WRONG_TYPE, or SNMP_ERROR_NO_ACCESS on failure.
	 */
	virtual int		prepare_set_request(Request*, int&); 

	/**
	 * Let the receiver undo a SNMP SET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP SET request.
	 * @param ind - The index of the subrequest to be processed.
	 * @return SNMP_ERROR_SUCCESS on success and 
	 *         SNMP_ERROR_UNDO_FAIL on failure.
	 */
	virtual int	        undo_set_request(Request*, int&); 

	/**
	 * Set the receiver's value and backup its old value for a later undo. 
	 *
	 * @param vb - The variable binding that holds the new value.
	 * @return SNMP_ERROR_SUCCESS if the new value has been set,
	 *         SNMP_ERROR_WRONG_TYPE or SNMP_ERROR_BAD_VALUE otherwise.
	 */
	virtual void	        cleanup_set_request(Request*, int&);

	/**
	 * Check whether the reciever node contains any instance of a
	 * managed object.
	 *
	 * @return TRUE if the node currently manages an instance.
	 */  
	virtual bool		is_empty() { return false; }

	/**
	 * Return the maximum object identifier the receiver is 
	 * responsible for.
	 * 
	 * @return A pointer to the maximum object identifier this object is
	 *         responsible for.
	 */
	virtual OidxPtr      	max_key();

	/**
	 * Return the successor of a given object identifier within the 
	 * receiver's scope and the context of a given Request.
	 *
	 * @param oid
	 *    an object identifier
	 * @param request
	 *    a pointer to a Request instance.
	 * @return 
	 *    an object identifier if a successor could be found,
	 *    otherwise (if no successor exists or is out of scope) 
	 *    a zero length oid is returned
	 */
	virtual Oidx		find_succ(const Oidx&, Request* req = 0); 

	/**
	 * Check whether a MibEntry is a AgentX entry or not
	 *
	 * @param entry
	 *    a MIB entry
	 * @return 
	 *    TRUE if the given entry is an AgentX entry, FALSE otherwise
	 */
	static bool		is_agentx(const MibEntry&);

	/**
	 * Get the number of registrations for this region.
	 *
	 * @return
	 *    an unsigned integer.
	 */
	u_int			registration_count() { return origReg.size(); }

	/**
	 * Get active registration.
	 *
	 * @return
	 *    the registration entry with the lowest priority value.
	 */
	AgentXRegEntry*         active_registration() 
					{ return origReg.first(); }

	/**
	 * Get the region registered by this node.
	 *
	 * @return
	 *    a AgentXRegion instance.
	 */
	AgentXRegion		get_region() { return region; } 
	
 protected:

	/**
	 * Set the region of the receiver. Do not call this when the
	 * node has been registered within a MIB.
	 *
	 * @param region
	 *    the new region
	 */
	void			set_region(const AgentXRegion&); 

	/**
	 * Let the receiver process a SNMP GET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP GET request.
	 * @param ind - The index of the subrequest to be processed.
	 * @param getType
	 *    specifies if a GET, GETNEXT, or GETBULK request is processed.
	 */
	virtual void		get_request(Request*, int, u_char);

	/**
	 * Let the receiver process a SNMP GET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP GET request.
	 * @param ind - The index of the subrequest to be processed.
	 * @param getType
	 *    specifies if a TESTSET, COMMITSET, UNDOSET, or CLEANUPSET
	 *    request is processed.
	 */
	virtual void		set_request(Request*, int, u_char);

	/**
	 * Set back reference to MasterAgentXMib holding the receiver.
	 *
	 * @param backReference
	 *    a pointer to a MasterAgentXMib instance.
	 */
	void			set_back_reference(MasterAgentXMib*);

	//	Oidx			upper;
	OrderedList<AgentXRegEntry> origReg;
	AgentXRegion		region;

	bool			singleInstance;
	MasterAgentXMib*	backReference;
};

#ifdef AGENTPP_NAMESPACE
}
#endif


#endif
