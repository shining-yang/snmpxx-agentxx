/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - proxy_forwarder.cpp  
  _## 
  _##  Copyright (C) 2000-2013  Frank Fock and Jochen Katz (agentpp.com)
  _##  
  _##  Licensed under the Apache License, Version 2.0 (the "License");
  _##  you may not use this file except in compliance with the License.
  _##  You may obtain a copy of the License at
  _##  
  _##      http://www.apache.org/licenses/LICENSE-2.0
  _##  
  _##  Unless required by applicable law or agreed to in writing, software
  _##  distributed under the License is distributed on an "AS IS" BASIS,
  _##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  _##  See the License for the specific language governing permissions and
  _##  limitations under the License.
  _##  
  _##########################################################################*/

#include <libagent.h>

#include <agent_pp/proxy_forwarder.h>
#include <agent_pp/snmp_target_mib.h>
#include <agent_pp/snmp_proxy_mib.h>
#include <snmp_pp/log.h>

#ifdef _SNMPv3
#ifdef _PROXY_FORWARDER

#ifdef AGENTPP_NAMESPACE
using namespace Agentpp;
#endif

static const char *loggerModuleName = "agent++.proxy_forwarder";

ProxyForwarder::ProxyForwarder(const OctetStr& contextEngineID, pdu_type t)
{
	regKey = Oidx::from_string(contextEngineID);
	regKey += t;
	snmp = 0;
	initialize();
}

ProxyForwarder::~ProxyForwarder()
{
	if (snmp)
		delete snmp;
}

void ProxyForwarder::initialize()
{
	check_references();
	snmp = new SnmpRequestV3();
}

void ProxyForwarder::check_references()
{
	if ((!snmpProxyEntry::instance) ||
	    (!snmpTargetParamsEntry::instance) ||
	    (!snmpTargetAddrEntry::instance)) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
		LOG("ProxyForwarder: internal error: need SNMP-PROXY- and SNMP-TARGET-MIB.");
		LOG_END;
	}
}

OidList<MibTableRow>*  ProxyForwarder::get_matches(Request* req)
{
	List<MibTableRow>* list =
	  snmpProxyEntry::instance->get_rows_cloned(TRUE);
	OidList<MibTableRow>* matches = new OidList<MibTableRow>;
	ListCursor<MibTableRow> cur;
	for (cur.init(list); cur.get(); cur.next()) {
		int type = req->get_type();
		int state = 0;
		cur.get()->get_nth(0)->get_value(state);

		OctetStr contextID, contextName, paramsIn;

		if ((!(((type == sNMP_PDU_GET) ||
			(type == sNMP_PDU_GETNEXT) ||
			(type == sNMP_PDU_GETBULK)) && (state == 1))) &&
		    (!((type == sNMP_PDU_SET) && (state == 2))) &&
		    (!(((type == sNMP_PDU_TRAP) ||
			(type == sNMP_PDU_V1TRAP)) && (state == 3))) &&
		    (!((type == sNMP_PDU_INFORM) && (state == 4))))
			continue;

		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
		LOG("ProxyForwarder: matched proxy (pdu type)(type)");
		LOG(type);
		LOG(state);
		LOG_END;

		cur.get()->get_nth(1)->get_value(contextID);
		cur.get()->get_nth(2)->get_value(contextName);
		cur.get()->get_nth(3)->get_value(paramsIn);

		OctetStr cid;
		req->get_pdu()->get_context_engine_id(cid);

		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
		LOG("ProxyForwarder: matching (context engine id)(match)");
		LOG(cid.get_printable());
		LOG(cid.len());
		LOG(contextID.get_printable());
		LOG(contextID.len());
		LOG_END;

		if (cid != contextID)
			continue;

		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
		LOG("ProxyForwarder: matched (context name)(match)");
		LOG(req->get_pdu()->get_context_name().get_printable());
		LOG(contextName.get_printable());
		LOG_END;

		OctetStr cname;
		req->get_pdu()->get_context_name(cname);
		if (cname != contextName)
			continue;

		if (!match_target_params(req, paramsIn))
			continue;
		matches->add(cur.get()->clone());
	}
	delete list;
	return matches;
}

bool ProxyForwarder::match_target_params(Request* req,
					    const OctetStr& paramsIn)
{
	snmpTargetParamsEntry::instance->start_synch();
	MibTableRow* paramsRow =
	  snmpTargetParamsEntry::instance->
	  find_index(Oidx::from_string(paramsIn, FALSE));

	if ((!paramsRow) ||
	    (paramsRow->get_row_status()->get() != rowActive)) {

		snmpTargetParamsEntry::instance->end_synch();

		LOG_BEGIN(loggerModuleName, WARNING_LOG | 3);
		LOG("ProxyForwarder: target addr parameter (row) not found.");
		LOG(OctetStr(paramsIn).get_printable());
		LOG((paramsRow) ? "no active row found" :
		    "missing row");
		LOG_END;
		return FALSE;
	}

	int secModel, secLevel, mpModel;
	OctetStr secName;

	paramsRow->get_nth(0)->get_value(mpModel);
	paramsRow->get_nth(1)->get_value(secModel);
	paramsRow->get_nth(2)->get_value(secName);
	paramsRow->get_nth(3)->get_value(secLevel);

	snmpTargetParamsEntry::instance->end_synch();

	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
	LOG("ProxyForwarder: matching (MPModel)(match)(secModel)(match)(secLevel)(match)");
	LOG(req->get_address()->get_version());
	LOG(mpModel);
	LOG(req->get_address()->get_security_model());
	LOG(secModel);
	LOG(req->get_pdu()->get_security_level());
	LOG(secLevel);
	LOG_END;

	if ((req->get_address()->get_version() == version1) &&
	    (mpModel != 0))
		return FALSE;
	if ((req->get_address()->get_version() == version2c) &&
	    (mpModel != 1))
		return FALSE;
	if ((req->get_address()->get_version() == version3) &&
	    (mpModel != 3))
		return FALSE;

	OctetStr sname;
	req->get_address()->get_security_name(sname);

	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
	LOG("ProxyForwarder: matching (secName)(match)");
	LOG(sname.get_printable());
	LOG(secName.get_printable());
	LOG_END;

	if (sname != secName)
		return FALSE;
	if ((secModel != 0) &&
	    (req->get_address()->get_security_model() != secModel))
		return FALSE;
	if (req->get_pdu()->get_security_level() != secLevel)
		return FALSE;
	return TRUE;
}

bool ProxyForwarder::process_multiple(Pdux& pdu, Request* req)
{
       	OidList<MibTableRow>* matches = get_matches(req);
	if (!matches) return FALSE;

	bool OK = FALSE;
	OidListCursor<MibTableRow> cur;
	for (cur.init(matches); cur.get(); cur.next()) {
		OctetStr out;
		cur.get()->get_nth(5)->get_value(out); // multiple target out

		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 2);
		LOG("ProxyForwarder: multiple targets (tag)");
		LOG(out.get_printable());
		LOG_END;

		List<MibTableRow>* targets =
		  snmpTargetAddrEntry::instance->
		                          get_rows_cloned_for_tag(out);
		ListCursor<MibTableRow> tcur;
		for (tcur.init(targets); tcur.get(); tcur.next()) {

			OctetStr targetOut;
			OctetStr params;

			targetOut = tcur.get()->get_index().as_string();
			//cur.get()->get_nth(1)->get_value(taddr);
			//cur.get()->get_nth(4)->get_value(tagList);
			tcur.get()->get_nth(5)->get_value(params);

			LOG_BEGIN(loggerModuleName, DEBUG_LOG | 2);
			LOG("ProxyForwarder: searching target address (name)(params)");
			LOG(targetOut.get_printable());
			LOG(params.get_printable());
			LOG_END;

			if (params.len() == 0)
				continue;

			int secLevel = 0;
			UTarget* target =
			  snmpTargetAddrEntry::instance->
			  get_target(targetOut,
				     snmpTargetParamsEntry::instance,
				     secLevel);
			if (!target)
				continue;
			GenAddress addr;
			target->get_address(addr);

			LOG_BEGIN(loggerModuleName, INFO_LOG | 2);
			LOG("ProxyForwarder: contacting agent (type)(address)(secName)(secLevel)(context)(contextEngineID)");
			LOG(pdu.get_type());
			LOG(addr.get_printable());
			LOG(target->get_security_name().get_printable());
			LOG(secLevel);
			LOG(pdu.get_context_name().get_printable());
			LOG(pdu.get_context_engine_id().get_printable());
			LOG_END;

			int status;
			status = snmp->send_request(*target, pdu);

			OK = TRUE;
			delete target;
			LOG_BEGIN(loggerModuleName, INFO_LOG | 3);
			LOG("ProxyForwarder: agent contacted: (status)");
			LOG(status);
			LOG_END;
		}
		delete targets;
	}
	delete matches;
	return OK;
}

bool ProxyForwarder::process_single(Pdux& pdu, Request* req)
{
	OidList<MibTableRow>* matches = get_matches(req);
	if (!matches) return FALSE;

	MibTableRow* match = matches->first();
	if (!match) {
		LOG_BEGIN(loggerModuleName, INFO_LOG | 3);
		LOG("ProxyForwarder: no matching proxy entry");
		LOG_END;
		delete matches;
		return FALSE;
	}
	OctetStr out;
	match->get_nth(4)->get_value(out);

	int secLevel = 0;
	UTarget* target =
	  snmpTargetAddrEntry::instance->
	  get_target(out, snmpTargetParamsEntry::instance, secLevel);
	if (!target) {
		LOG_BEGIN(loggerModuleName, INFO_LOG | 3);
		LOG("ProxyForwarder: no matching single out address entry");
		LOG_END;
		delete matches;
		return FALSE;
	}
	pdu.set_security_level(secLevel);

	GenAddress addr;
	target->get_address(addr);
	LOG_BEGIN(loggerModuleName, INFO_LOG | 3);
	LOG("ProxyForwarder: get: contacting agent (address)(secName)(secLevel)(context)(contextEngineID)");
	LOG(addr.get_printable());
	LOG(target->get_security_name().get_printable());
	LOG(secLevel);
	LOG(pdu.get_context_name().get_printable());
	LOG(pdu.get_context_engine_id().get_printable());
	LOG_END;

	int status = snmp->send_request(*target, pdu, req->get_non_rep(), req->get_max_rep());
	if (status != SNMP_ERROR_SUCCESS) {
		pdu.set_error_status(SNMP_ERROR_GENERAL_VB_ERR);
		pdu.set_error_index(0);
	}

	delete target;
	Vbx vb;
	pdu.get_vb(vb, 0);

	LOG_BEGIN(loggerModuleName, INFO_LOG | 3);
	LOG("ProxyForwarder: agent contacted: (oid), (value), (status)");
	LOG(vb.get_printable_oid());
	LOG(vb.get_printable_value());
	LOG(status);
	LOG_END;

	delete matches;

	return TRUE;
}

bool ProxyForwarder::process_request(Request* req)
{
	Pdux pdu(*req->get_pdu());
	switch (pdu.get_type()) {
	case sNMP_PDU_GET:
	case sNMP_PDU_GETNEXT:
	case sNMP_PDU_GETBULK:
	case sNMP_PDU_SET: {
		bool success = process_single(pdu, req);
		transform_pdu(pdu, *req->get_pdu());
		return success;
	}
	}
	return process_multiple(pdu, req);
}

void ProxyForwarder::transform_pdu(const Pdux& in, Pdux& out)
{
    out.trim(out.get_vb_count()); // remove all Vbs in ou tpdu
	for (int i=0; i<in.get_vb_count(); i++) {
		Vbx vb;
		in.get_vb(vb, i);
		out += vb;
	}
	out.set_error_status(in.get_error_status());
	out.set_error_index(in.get_error_index());
}


#endif
#endif
