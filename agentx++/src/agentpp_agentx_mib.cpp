/*_############################################################################
  _## 
  _##  agentpp_agentx_mib.cpp  
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
#include <agentx_pp/agentpp_agentx_mib.h>


#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

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
 
agentppAgentXExtSrcAddrValidation* agentppAgentXExtSrcAddrValidation::instance = 0;


agentppAgentXExtSrcAddrValidation::agentppAgentXExtSrcAddrValidation():
    MibLeaf(oidAgentppAgentXExtSrcAddrValidation, READWRITE, new SnmpInt32()){
	// This leaf object is a singleton. In order to access it use
	// the static pointer agentppAgentXExtSrcAddrValidation::instance.
	instance = this;
	//--AgentGen BEGIN=agentppAgentXExtSrcAddrValidation::agentppAgentXExtSrcAddrValidation
	set_state(1); // enable by default
	//--AgentGen END

}

agentppAgentXExtSrcAddrValidation::~agentppAgentXExtSrcAddrValidation()
{

	//--AgentGen BEGIN=agentppAgentXExtSrcAddrValidation::~agentppAgentXExtSrcAddrValidation
	//--AgentGen END

	// clear singleton reference
	agentppAgentXExtSrcAddrValidation::instance = 0;
}

long agentppAgentXExtSrcAddrValidation::get_state()
{
	//--AgentGen BEGIN=agentppAgentXExtSrcAddrValidation::get_state
	//--AgentGen END
	return (long)*((SnmpInt32*)value);
}
	
void agentppAgentXExtSrcAddrValidation::set_state(long l)
{
	//--AgentGen BEGIN=agentppAgentXExtSrcAddrValidation::set_state
	//--AgentGen END
	*((SnmpInt32*)value) = l;
}
	

int agentppAgentXExtSrcAddrValidation::prepare_set_request(Request* req, int& ind)
{
	int status;
	if ((status = MibLeaf::prepare_set_request(req, ind)) !=
	    SNMP_ERROR_SUCCESS) return status;

	//--AgentGen BEGIN=agentppAgentXExtSrcAddrValidation::prepare_set_request
	//--AgentGen END
	return SNMP_ERROR_SUCCESS;
}

//--AgentGen BEGIN=agentppAgentXExtSrcAddrValidation
//--AgentGen END



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
 
agentppAgentXExtSrcAddrTag::agentppAgentXExtSrcAddrTag(const Oidx& id):
    MibLeaf(id, READCREATE, new OctetStr("agentX"), VMODE_DEFAULT){
	//--AgentGen BEGIN=agentppAgentXExtSrcAddrTag::agentppAgentXExtSrcAddrTag
	//--AgentGen END

}

agentppAgentXExtSrcAddrTag::~agentppAgentXExtSrcAddrTag()
{

	//--AgentGen BEGIN=agentppAgentXExtSrcAddrTag::~agentppAgentXExtSrcAddrTag
	//--AgentGen END

}

MibEntryPtr agentppAgentXExtSrcAddrTag::clone()
{
	MibEntryPtr other = new agentppAgentXExtSrcAddrTag(oid);
	((agentppAgentXExtSrcAddrTag*)other)->replace_value(value->clone());
	((agentppAgentXExtSrcAddrTag*)other)->set_reference_to_table(my_table);
	//--AgentGen BEGIN=agentppAgentXExtSrcAddrTag::clone
	//--AgentGen END
	return other;
}

OctetStr agentppAgentXExtSrcAddrTag::get_state() {
	//--AgentGen BEGIN=agentppAgentXExtSrcAddrTag::get_state
	//--AgentGen END
	return *((OctetStr*)value);
}

int agentppAgentXExtSrcAddrTag::prepare_set_request(Request* req, int& ind)
{
	int status;
	if ((status = MibLeaf::prepare_set_request(req, ind)) !=
	    SNMP_ERROR_SUCCESS) return status;

	Vb vb(req->get_value(ind));
	OctetStr v;
	vb.get_value(v);
	if (v.len() > 255)
		 return SNMP_ERROR_WRONG_LENGTH;
	//--AgentGen BEGIN=agentppAgentXExtSrcAddrTag::prepare_set_request
	//--AgentGen END
	return SNMP_ERROR_SUCCESS;
}

//--AgentGen BEGIN=agentppAgentXExtSrcAddrTag
//--AgentGen END




/**
 *  agentppAgentXExtSrcAddrRowStatus
 *
 * "The status of this conceptual row.
 * To create a row in this table, a manager must
 * set this object to either createAndGo(4) or
 * createAndWait(5)."
 */
 
agentppAgentXExtSrcAddrRowStatus::agentppAgentXExtSrcAddrRowStatus(const Oidx& id):
    snmpRowStatus(id, READCREATE)
{
	//--AgentGen BEGIN=agentppAgentXExtSrcAddrRowStatus::agentppAgentXExtSrcAddrRowStatus
	//--AgentGen END

}

agentppAgentXExtSrcAddrRowStatus::~agentppAgentXExtSrcAddrRowStatus()
{

	//--AgentGen BEGIN=agentppAgentXExtSrcAddrRowStatus::~agentppAgentXExtSrcAddrRowStatus
	//--AgentGen END

}

MibEntryPtr agentppAgentXExtSrcAddrRowStatus::clone()
{
	MibEntryPtr other = new agentppAgentXExtSrcAddrRowStatus(oid);
	((agentppAgentXExtSrcAddrRowStatus*)other)->replace_value(value->clone());
	((agentppAgentXExtSrcAddrRowStatus*)other)->set_reference_to_table(my_table);
	//--AgentGen BEGIN=agentppAgentXExtSrcAddrRowStatus::clone
	//--AgentGen END
	return other;
}

long agentppAgentXExtSrcAddrRowStatus::get_state()
{
	//--AgentGen BEGIN=agentppAgentXExtSrcAddrRowStatus::get_state
	//--AgentGen END
	return (long)*((SnmpInt32*)value);
}
	

int agentppAgentXExtSrcAddrRowStatus::prepare_set_request(Request* req, int& ind)
{
	int status;
	if ((status = snmpRowStatus::prepare_set_request(req, ind)) !=
	    SNMP_ERROR_SUCCESS) return status;

	//--AgentGen BEGIN=agentppAgentXExtSrcAddrRowStatus::prepare_set_request
	//--AgentGen END
	return SNMP_ERROR_SUCCESS;
}

//--AgentGen BEGIN=agentppAgentXExtSrcAddrRowStatus
//--AgentGen END



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
 
agentppAgentXExtSrcAddrEntry* agentppAgentXExtSrcAddrEntry::instance = 0;

const index_info indAgentppAgentXExtSrcAddrEntry[1] = {
	{ sNMP_SYNTAX_OCTETS, FALSE, 0, 64 }
};

agentppAgentXExtSrcAddrEntry::agentppAgentXExtSrcAddrEntry():
   MibTable(oidAgentppAgentXExtSrcAddrEntry, indAgentppAgentXExtSrcAddrEntry, 1)
{
	// This table object is a singleton. In order to access it use
	// the static pointer agentppAgentXExtSrcAddrEntry::instance.
	instance = this;

	add_col(new agentppAgentXExtSrcAddrTag(colAgentppAgentXExtSrcAddrTag));
	add_col(new agentppAgentXExtSrcAddrRowStatus(colAgentppAgentXExtSrcAddrRowStatus));
	//--AgentGen BEGIN=agentppAgentXExtSrcAddrEntry::agentppAgentXExtSrcAddrEntry
	//--AgentGen END
}

agentppAgentXExtSrcAddrEntry::~agentppAgentXExtSrcAddrEntry()
{
	//--AgentGen BEGIN=agentppAgentXExtSrcAddrEntry::~agentppAgentXExtSrcAddrEntry
	//--AgentGen END
	// clear singleton reference
	agentppAgentXExtSrcAddrEntry::instance = 0;
}

int agentppAgentXExtSrcAddrEntry::prepare_set_request(Request* req, int& ind)
{
	//--AgentGen BEGIN=agentppAgentXExtSrcAddrEntry::prepare_set_request
	//--AgentGen END
	return MibTable::prepare_set_request(req, ind);
}

void agentppAgentXExtSrcAddrEntry::row_activated(MibTableRow* row, const Oidx& index, MibTable* src)
{
	// The row 'row' with 'index' has been activated.
	//--AgentGen BEGIN=agentppAgentXExtSrcAddrEntry::row_activated
	//--AgentGen END
}

void agentppAgentXExtSrcAddrEntry::row_deactivated(MibTableRow* row, const Oidx& index, MibTable* src)
{
	// The row 'row' with 'index' has been deactivated.
	//--AgentGen BEGIN=agentppAgentXExtSrcAddrEntry::row_deactivated
	//--AgentGen END
}

//--AgentGen BEGIN=agentppAgentXExtSrcAddrEntry
//--AgentGen END

// Notifications

// Group
agentpp_agentx_mib::agentpp_agentx_mib():
   MibGroup("1.3.6.1.4.1.4976.3.1", "agentpp_agentx_mib")
{
	//--AgentGen BEGIN=agentpp_agentx_mib::agentpp_agentx_mib
	//--AgentGen END
	add(new agentppAgentXExtSrcAddrValidation());
	add(new agentppAgentXExtSrcAddrEntry());
	//--AgentGen BEGIN=agentpp_agentx_mib::agentpp_agentx_mib:post
	//--AgentGen END
}

//--AgentGen BEGIN=agentpp_agentx_mib
//--AgentGen END




#ifdef AGENTPP_NAMESPACE
}
#endif


