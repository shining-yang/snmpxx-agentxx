/*_############################################################################
  _## 
  _##  agentx_reg.h  
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


#ifndef agentx_reg_h_
#define agentx_reg_h_

#include <agentx_pp/agentx_pdu.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


class AgentXSession;

class AgentXRegEntry {

 public:
	/**
	 * Construct a registration entry
	 *
	 * @param sessionID
	 *    a session id of the session object associated with this
	 *    registration
	 * @param region
	 *    a region with lower and upper bound of the registration
	 * @param priority
	 *    the priority of the registration
	 * @param specific
	 *    the number of subidentifiers of the original registration
	 * @param context
	 *    the context of the registration
	 * @param timeout
	 *    the timeout in seconds or 0 for the sessions default timeout. 
	 */
	AgentXRegEntry(u_int, const AgentXRegion&, u_int, u_int,
		       const NS_SNMP OctetStr&, u_char);

	/**
	 * Copy constructor
	 */
	AgentXRegEntry(const AgentXRegEntry&);

	/**
	 * Destructor
	 */
	virtual ~AgentXRegEntry();

	/**
	 * Clone
	 *
	 * @return a clone of the receiver
	 */
	virtual AgentXRegEntry* clone();


	virtual bool		operator<(const AgentXRegEntry&);
	virtual bool		operator>(const AgentXRegEntry&);
	virtual bool		operator==(const AgentXRegEntry&);
	

	virtual bool		is_duplicate_of(const AgentXRegEntry&);

	virtual bool		is_overlapping(const AgentXRegEntry&);

	AgentXRegion		get_region() { return region; } 


	u_int		sessionID;
	AgentXRegion   	region;
	u_int		priority, specific;
	NS_SNMP OctetStr	context;
	u_char		timeout;

	u_int		mibID;
	//OrderedList<AgentXNode> nodes; //may be used to speed up unregister 
};

#ifdef AGENTPP_NAMESPACE
}
#endif


#endif

