/*_############################################################################
  _## 
  _##  agentpp_agentx_mib.h  
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
   

//--AgentGen BEGIN=_BEGIN
//--AgentGen END

#ifndef _agentpp_agentx_mib_h
#define _agentpp_agentx_mib_h

#include <agent_pp/mib.h>
#include <agent_pp/mib_complex_entry.h>
#include <agent_pp/snmp_textual_conventions.h>
#include <agent_pp/notification_originator.h>


// Scalars
#define oidAgentppAgentXExtSrcAddrValidation      "1.3.6.1.4.1.4976.3.1.2.2.1.0"

// Columns
#define oidAgentppAgentXExtSrcAddrTag     "1.3.6.1.4.1.4976.3.1.2.2.2.1.2"
#define colAgentppAgentXExtSrcAddrTag     "2"
#define oidAgentppAgentXExtSrcAddrRowStatus     "1.3.6.1.4.1.4976.3.1.2.2.2.1.3"
#define colAgentppAgentXExtSrcAddrRowStatus     "3"


// Tables
#define oidAgentppAgentXExtSrcAddrEntry   "1.3.6.1.4.1.4976.3.1.2.2.2.1"
#define nAgentppAgentXExtSrcAddrTag      0
#define cAgentppAgentXExtSrcAddrTag      2
#define nAgentppAgentXExtSrcAddrRowStatus    1
#define cAgentppAgentXExtSrcAddrRowStatus    3


// Notifications

//--AgentGen BEGIN=_INCLUDE
#include "agentx_def.h"
//--AgentGen END

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


// Scalar Objects



/**
 *  agentppAgentXExtSrcAddrValidation
 *
 * "To enable source address validation of 
 * incoming subagent connections in an AgentX++
 * master agent, set this object to enabled(1).

 * Source address validation is necessary for TCP
 * AgentX connections for security reasons. In contrast
 * to UNIX domain sockets, (pure) TCP provides no means 
 * to control the access of a server port. So any subagent,
 * even remote subagents, may connect to an AgentX master
 * agent, if source address validation is disabled(2) and
 * not other security policies (i.e. firewall) are installed.

 * On non UNIX based systems, AgentX++ enables source
 * address validation by default to prevent subagents trying
 * to connect from non local IP addresses, because
 * these systems provide TCP support only."
 */
 
class AGENTXPP_DECL agentppAgentXExtSrcAddrValidation: public MibLeaf
{
	
public:
	agentppAgentXExtSrcAddrValidation();
	virtual ~agentppAgentXExtSrcAddrValidation();

	static agentppAgentXExtSrcAddrValidation* instance;
	
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	prepare_set_request(Request*, int&);
	enum labels {
		      e_enabled = 1,
		      e_disabled = 2	};

//--AgentGen BEGIN=agentppAgentXExtSrcAddrValidation
//--AgentGen END
};


// Columnar Objects



/**
 *  agentppAgentXExtSrcAddrTag
 *
 * "The source address tag selects address descriptions
 * from the snmpTargetAddrTable and the
 * snmpTargetAddrExtTable. The selected addresses
 * from the snmpTargetAddrTable combined with the
 * masks defined in the snmpTargetAddrExtTable specify
 * valid source addresses for AgentX++ subagent
 * connections."
 */
 
class AGENTXPP_DECL agentppAgentXExtSrcAddrTag: public MibLeaf
{
	
public:
	agentppAgentXExtSrcAddrTag(const Oidx&);
	virtual ~agentppAgentXExtSrcAddrTag();

	virtual MibEntryPtr	clone();
	virtual NS_SNMP OctetStr       	get_state();
	virtual int        	prepare_set_request(Request*, int&);

//--AgentGen BEGIN=agentppAgentXExtSrcAddrTag
//--AgentGen END
};




/**
 *  agentppAgentXExtSrcAddrRowStatus
 *
 * "The status of this conceptual row.
 * To create a row in this table, a manager must
 * set this object to either createAndGo(4) or
 * createAndWait(5)."
 */
 
class AGENTXPP_DECL agentppAgentXExtSrcAddrRowStatus: public snmpRowStatus
{
	
public:
	agentppAgentXExtSrcAddrRowStatus(const Oidx&);
	virtual ~agentppAgentXExtSrcAddrRowStatus();

	virtual MibEntryPtr	clone();
	virtual long       	get_state();
	virtual int        	prepare_set_request(Request*, int&);
	enum labels {
		      e_active = 1,
		      e_notInService = 2,
		      e_notReady = 3,
		      e_createAndGo = 4,
		      e_createAndWait = 5,
		      e_destroy = 6	};

//--AgentGen BEGIN=agentppAgentXExtSrcAddrRowStatus
//--AgentGen END
};


// Tables


/**
 *  agentppAgentXExtSrcAddrEntry
 *
 * "A row in the subagent address table specifies
 * a tag value that selects entries of the
 * snmpTargetAddrTable and its snmpTargetAddrExtTable
 * augmentation table. The selected entries define
 * valid source addresses for subagent connections.
 * Invalid subagent connection attempts will be silently
 * dropped if the agentppAgentXExtSrcAddrValidation 
 * object is enabled(1)."
 */
 
class AGENTXPP_DECL agentppAgentXExtSrcAddrEntry: public MibTable
{
	
public:
	agentppAgentXExtSrcAddrEntry();
	virtual ~agentppAgentXExtSrcAddrEntry();

	static agentppAgentXExtSrcAddrEntry* instance;
	
	virtual int         	prepare_set_request(Request*, int&);
	virtual void        	row_activated(MibTableRow*, const Oidx&, MibTable*);
	virtual void        	row_deactivated(MibTableRow*, const Oidx&, MibTable*);
//--AgentGen BEGIN=agentppAgentXExtSrcAddrEntry
//--AgentGen END
};

// Notifications
#ifdef _SNMPv3
#endif

// Group

class AGENTXPP_DECL agentpp_agentx_mib: public MibGroup
{
  public:
        agentpp_agentx_mib();
        virtual ~agentpp_agentx_mib() { }
//--AgentGen BEGIN=agentpp_agentx_mib
//--AgentGen END
};


#ifdef AGENTPP_NAMESPACE
}
#endif


//--AgentGen BEGIN=_END
//--AgentGen END

#endif
