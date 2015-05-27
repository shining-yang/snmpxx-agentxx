/*_############################################################################
  _## 
  _##  agentx_node.cpp  
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

#include <agentx_pp/agentx_node.h>
#include <agentx_pp/agentx_master.h>
#include <snmp_pp/log.h>

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

static const char *loggerModuleName = "agentx++.agentx_node";


/*-------------------------- class AgentXNode -----------------------*/

AgentXNode::AgentXNode(AgentXRegEntry* entry, const AgentXRegion& r) 
{
	origReg.add(entry);
	set_region(r);
	singleInstance = FALSE;
	backReference = 0;
}

AgentXNode::AgentXNode(const AgentXNode& other)
{
	OrderedListCursor<AgentXRegEntry> cur;
	for (cur.init(&other.origReg); cur.get(); cur.next()) {
		origReg.add(cur.get()->clone());
	}
	set_region(other.region);
	singleInstance = other.singleInstance;
	backReference = 0;
}

AgentXNode::~AgentXNode()
{
	origReg.clear(); // do not delete registrations
}

void AgentXNode::set_back_reference(MasterAgentXMib* ref) 
{
	backReference = ref;
}

mib_type AgentXNode::type() const
{
	return (singleInstance) ? AGENTX_LEAF : AGENTX_NODE;
}
 
bool AgentXNode::is_agentx(const MibEntry& entry) 
{
	return ((entry.type() == AGENTX_LEAF) || 
		(entry.type() == AGENTX_NODE));
} 

void AgentXNode::set_region(const AgentXRegion& r) 
{
	region = r;
	set_oid(r.get_lower());
}

bool AgentXNode::shrink(const Oidx& upper)
{
	if ((*key() <= upper) && (upper <= *max_key())) {
		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 4);
		LOG("AgentXNode: shrunk (oid)(old upper)(new upper)");
		LOG(key()->get_printable());
		LOG(region.get_upper().get_printable());
		LOG(Oidx(upper).get_printable());
		LOG_END;
		region.set_upper(upper);
		return TRUE;
	}
	return FALSE;
}

bool AgentXNode::expand(const Oidx& upper)
{
	if ((*key() <= upper) && (upper >= *max_key())) {
		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 4);
		LOG("AgentXNode: expand (oid)(old upper)(new upper)");
		LOG(key()->get_printable());
		LOG(region.get_upper().get_printable());
		LOG(Oidx(upper).get_printable());
		LOG_END;
		region.set_upper(upper);
		return TRUE;
	}
	return FALSE;
}


MibEntry* AgentXNode::clone()
{
	return new AgentXNode(*this);
}

OidxPtr AgentXNode::max_key()
{
	return &region.upper();
}

Oidx AgentXNode::find_succ(const Oidx& o, Request* req)
{ 
	Oidx nullOid;
	Oidx next(o.successor());
	if (next < *max_key()) return *key();
	return nullOid; 
} 

void AgentXNode::add_registration(AgentXRegEntry* regEntry)
{
	start_synch();
	origReg.add(regEntry);
	end_synch();
}

bool	AgentXNode::remove_registration(const AgentXRegEntry& regEntry)
{
	start_synch();
	OrderedListCursor<AgentXRegEntry> cur;
	for (cur.init(&origReg); cur.get(); cur.next()) {
		if (*cur.get() == regEntry) {
			// do not delete registration itself here!
			// it is deleted later on
			origReg.remove(cur.get());
			break;
		}
	}
	end_synch();
	return (origReg.size() == 0);
}

void AgentXNode::get_request(Request* req, int reqind, u_char get_type)
{
	if (!backReference) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
		LOG("AgentXNode: node not registered at MasterAgentXMib");
		LOG_END;
		req->error(reqind, SNMP_ERROR_GENERAL_VB_ERR); 
	}
	else {
		AgentXRegEntry* reg = origReg.first();
		if (!reg) {
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			LOG("AgentXNode: no registration entry");
			LOG_END;
			req->error(reqind, SNMP_ERROR_GENERAL_VB_ERR); 
			return;
		}
		OctetStr context;
#ifdef _SNMPv3
		req->get_pdu()->get_context_name(context);
#endif
		AgentXSearchRange* range = 0;
		if (get_type == AGENTX_GET_PDU) {
			range = new AgentXSearchRange(req->get_oid(reqind), 
						      Oidx(), TRUE);
		}
		else {
			bool include = FALSE;
			Oidx o(req->get_oid(reqind));
			if ((o.len()>0) && (o[o.len()-1] == 0xFFFFFFFFul)) {
				o = o.successor();
				include = TRUE;
			}
			range = 
			    new AgentXSearchRange(o, 
			       backReference->
				 get_upper_session_boundary(context, this),
			       include);
			// test for single instance search
			if (range->get_lower() == range->get_upper()) {
				delete range;
				if (get_type == AGENTX_GETBULK_PDU) {
				  // For BULK requests we do not issue a GET
				  // because it would be rather difficult
				  // to map the response back to the request.
				  // Instead we do a GETBULK with a single
				  // instance range. This should perform 
				  // nearly as good as a GET.
				  range = 
				    new AgentXSearchRange(o, 
					     max_key()->successor(), TRUE);
				}
				else {
				  range = 
				    new AgentXSearchRange(o, Oidx(), TRUE);
				  get_type = AGENTX_GET_PDU;
				}
			}
		}
		// set reference to original subrequest
		range->set_reference(reqind);

		if (get_type == AGENTX_GETBULK_PDU) {
			int repeater = 0;
			if (reqind >= req->get_non_rep())
				repeater = req->get_rep();
			int maxrep = MAX_AGENTX_REPETITIONS - 
					req->get_pdu()->get_vb_count() + 1;
			if (maxrep <= 0)
				maxrep = 1;
			if (maxrep > req->get_max_rep()) 
				maxrep = req->get_max_rep();
			backReference->
			  add_get_subrequest(reg->sessionID,
					     req->get_transaction_id(),
					     *range,
					     reg->timeout,
					     get_type,
					     context,
					     repeater,
					     maxrep);  
		}
		else 
			backReference->
			  add_get_subrequest(reg->sessionID,
					     req->get_transaction_id(),
					     *range,
					     reg->timeout,
					     get_type,
					     context);
		delete range;
	}
}

void AgentXNode::get_request(Request* req, int reqind)
{
	get_request(req, reqind, AGENTX_GET_PDU);
}

void AgentXNode::get_next_request(Request* req, int reqind)
{
  /* This done above in a more general way by detecting whether a request
     can only result in a single OID
  	if (region.is_single()) {
		get_request(req, reqind, AGENTX_GET_PDU);		
	}
	else
  */
	if (req->get_type() == sNMP_PDU_GETBULK)
		get_request(req, reqind, AGENTX_GETBULK_PDU);	
	else
		get_request(req, reqind, AGENTX_GETNEXT_PDU);	
}

void AgentXNode::set_request(Request* req, int reqind, u_char set_type)
{
	if (!backReference) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
		LOG("AgentXNode: node not registerd at MasterAgentXMib");
		LOG_END;
		req->error(reqind, SNMP_ERROR_GENERAL_VB_ERR);
	}
	else {
		AgentXRegEntry* reg = origReg.first();
		if (!reg) {
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			LOG("AgentXNode: no registration entry");
			LOG_END;
			req->error(reqind, SNMP_ERROR_GENERAL_VB_ERR); 
			return;
		}
		OctetStr context;
#ifdef _SNMPv3
		req->get_pdu()->get_context_name(context);
#endif
		// create dummy search range -> is only used to map
		// subrequests
		AgentXSearchRange r;
		r.set_reference(reqind);
		backReference->add_set_subrequest(reg->sessionID, 
						  req->get_transaction_id(),
						  r,
						  req->get_value(reqind), 
						  reg->timeout,
						  set_type, context);
	}
}


int AgentXNode::prepare_set_request(Request* req, int& reqind)
{
	set_request(req, reqind, AGENTX_TESTSET_PDU);
	return SNMP_ERROR_SUCCESS;
}


int AgentXNode::commit_set_request(Request* req, int reqind)
{
	set_request(req, reqind, AGENTX_COMMITSET_PDU);
	return SNMP_ERROR_SUCCESS;
}


int AgentXNode::undo_set_request(Request* req, int& reqind)
{
	set_request(req, reqind, AGENTX_UNDOSET_PDU);
	return SNMP_ERROR_SUCCESS;
}

void AgentXNode::cleanup_set_request(Request* req, int& reqind)
{
	set_request(req, reqind, AGENTX_CLEANUPSET_PDU);
}

#ifdef AGENTPP_NAMESPACE
}
#endif

