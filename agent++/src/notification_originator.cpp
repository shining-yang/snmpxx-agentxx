/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - notification_originator.cpp  
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

#include <agent_pp/snmp_target_mib.h>
#include <agent_pp/snmp_notification_mib.h>
#include <agent_pp/system_group.h>

#include <agent_pp/notification_originator.h>
#include <agent_pp/snmp_request.h>
#include <agent_pp/snmp_community_mib.h>
#include <agent_pp/notification_log_mib.h>
#include <agent_pp/vacm.h>
#include <snmp_pp/log.h>

#define NO_TRAP 0
#define TRAP    1
#define INFORM  2

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

static const char *loggerModuleName = "agent++.notification_originator";

/*------------------ class NotificationOriginator -----------------------*/

NotificationOriginator::NotificationOriginator()
{
#ifdef _SNMPv3
	localEngineID = 0;
#endif
}

NotificationOriginator::~NotificationOriginator()
{
#ifdef _SNMPv3
	if (localEngineID)
		delete localEngineID;
#endif
}


void NotificationOriginator::generate(Vbx* vbs, int size, const Oidx& id,
				      const Oidx& enterprise,
				      const OctetStr& contextName) {
	generate(vbs, size, id, sysUpTime::get(), enterprise, contextName);
}

void NotificationOriginator::generate(Vbx* vbs, int size, const Oidx& id,
				      unsigned int timestamp,
				      const OctetStr& contextName) {
	generate(vbs, size, id, timestamp, "", contextName);
}

int NotificationOriginator::notify(const OctetStr& context,
				   const Oidx& oid,
				   Vbx* vbs, int sz,
				   unsigned int timestamp)
{
	if (timestamp == 0)
		timestamp = sysUpTime::get();
	return generate(vbs, sz, oid, timestamp, Oidx(), context);
}

int NotificationOriginator::generate(Vbx* vbs, int size, const Oidx& id,
				     unsigned int timestamp,
				     const Oidx& enterprise,
				     const OctetStr& contextName)
{
	// We have to be careful here about synchronization because,
	// we may be called after an interrupt
	// Therefore synch everything or use synch methods like
	// MibTable::get_rows_cloned();
	List<MibTableRow>* typeList =
	  snmpNotifyEntry::instance->get_rows_cloned();
	ListCursor<MibTableRow> typeCur;

	List<MibTableRow>* list =
	  snmpTargetAddrEntry::instance->get_rows_cloned();
	ListCursor<MibTableRow> cur;


#ifdef _SNMPv3
	if (!localEngineID) {
		if (!v3MP::I) {
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
			LOG("NotificationOriginator: v3MP has to be initialized before notifications can be sent");
			LOG_END;
		}
		localEngineID = new OctetStr(v3MP::I->get_local_engine_id());
	}
#endif

	int totalStatus = SNMP_ERROR_SUCCESS;

	NotificationOriginatorParams nop(vbs, size, id, timestamp, enterprise, contextName);
#ifdef _SNMPv3
	if (nlmLogEntry::instance) {
	  nlmLogEntry::instance->
	    add_notification(NULL, id, vbs, size, contextName,
			     *localEngineID,
			     *localEngineID);
	}
#endif
	for (cur.init(list); cur.get(); cur.next()) {

	  int notify = NO_TRAP;

	  // look for tags that identify notifications
	  for (typeCur.init(typeList); typeCur.get(); typeCur.next()) {

		OctetStr tag;
		typeCur.get()->first()->get_value(tag);
		char* tagstr = new char[tag.len()+1];
		strncpy(tagstr, (char*)tag.data(), tag.len());
		tagstr[tag.len()] = 0;

		if (((SnmpTagList*)cur.get()->get_nth(4))->
		    contains(tagstr)) {
			// determine notification type
			typeCur.get()->get_nth(1)->get_value(notify);

			nop.target = 0;
			if (check_access(cur, nop)) {
				int status = send_notify(cur, nop, notify);
				if (status != SNMP_ERROR_SUCCESS)
					totalStatus = status;
				delete nop.target;
			}
		}
		delete[] tagstr;
	  }
	  if (notify == NO_TRAP) {
		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
		LOG("NotificationOriginator: generate: could not find valid tag");
		LOG_END;
		continue;
	  }
	}

	typeList->clearAll();
	delete typeList;
	list->clearAll();
	delete list;

	return totalStatus;
}


bool NotificationOriginator::check_access(ListCursor<MibTableRow>& cur,
		NotificationOriginatorParams& nop)
{
	Vbx*& vbs = nop.vbs;
	int& size = nop.size;
	const Oidx& id = nop.id;
	OctetStr& securityName = nop.securityName;
	int& securityModel = nop.securityModel;
	int& securityLevel = nop.securityLevel;
	int& mpModel = nop.mpModel;
#ifdef _SNMPv3
	const OctetStr& contextName = nop.contextName;
	UTarget*& target = nop.target;
#else
	CTarget*& target = nop.target;
#endif

	  snmpTargetAddrParams* paramsPtr =
	    (snmpTargetAddrParams*)cur.get()->get_nth(5);
	  OctetStr paramsStr;
	  paramsPtr->get_value(paramsStr);

	  // Check whether trap oid passes filter
	  Oidx targetOid(Oidx::from_string(paramsStr, FALSE));
	  if (!snmpNotifyFilterEntry::passes_filter(targetOid, id, vbs, size)){

		LOG_BEGIN(loggerModuleName, INFO_LOG | 2);
		LOG("NotificationOriginator: generate: event did not pass notification filter (trapoid)(filter)");
		LOG(Oidx(id).get_printable());
		LOG(paramsStr.get_printable());
		LOG_END;
		return FALSE;
	  }

	  OctetStr targetAddress;
	  cur.get()->get_nth(1)->get_value(targetAddress);

	  snmpTargetParamsEntry::instance->start_synch();
	  MibTableRow* paramsRow =
	    snmpTargetParamsEntry::instance->
	    find_index(Oidx::from_string(paramsStr, FALSE));

	  if ((!paramsRow) ||
	      (paramsRow->get_row_status()->get() != rowActive)) {

	    snmpTargetParamsEntry::instance->end_synch();
	    LOG_BEGIN(loggerModuleName, WARNING_LOG | 3);
	    LOG("NotificationOriginator: generate: target addr parameter row not found.");
	    LOG(paramsStr.get_printable());
	    LOG((paramsRow) ? "no active row found" : "missing row");
	    LOG_END;
	    return FALSE;
	  }

	  paramsRow->get_nth(0)->get_value(mpModel);
	  paramsRow->get_nth(2)->get_value(securityName);
	  paramsRow->get_nth(1)->get_value(securityModel);
	  paramsRow->get_nth(3)->get_value(securityLevel);

	  snmpTargetParamsEntry::instance->end_synch();

	  bool accessAllowed = TRUE;
#ifdef _SNMPv3
	  if ((!Mib::instance) ||
	      (!Mib::instance->get_request_list()) ||
	      (!Mib::instance->get_request_list()->get_vacm())) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
		LOG("NotificationOriginator: Mib or its requestList are not initialized, aborting!");
		LOG_END;
		return FALSE;
	  }
	  Vacm* vacm = Mib::instance->get_request_list()->get_vacm();

	  for (int i=0; i<size; i++) {

	    if (vacm->isAccessAllowed(securityModel,
				      securityName,
				      securityLevel,
				      mibView_notify,
				      contextName,
				      vbs[i].get_oid()) !=
		VACM_accessAllowed) {
	      accessAllowed = FALSE;
	      break;
	    }
	  }

	  if (accessAllowed) {
	    accessAllowed =
	      (vacm->isAccessAllowed(securityModel, securityName,
				     securityLevel, mibView_notify,
				     contextName, id) ==
	       VACM_accessAllowed);
	  }
#endif
	  if (!accessAllowed) {
	    LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	    LOG("Notification not sent (reason) (addr) (params)");
	    LOG("no access");
	    LOG(targetAddress.get_printable());
	    LOG(paramsStr.get_printable());
	    LOG_END;

	    return FALSE;
	  }

	  target = 0;
	  Address* address =
	    snmpTargetAddrEntry::instance->get_address(cur.get());
	  if (address) {
#ifdef _SNMPv3
		target = new UTarget(*address, securityName, securityModel);
#else
		target = new CTarget(*address, securityName, securityName);
#endif
		delete address;
	  }
	  else return FALSE;

	  return TRUE;
}


int NotificationOriginator::send_notify(ListCursor<MibTableRow>& cur,
		NotificationOriginatorParams& nop, int notify)
{
	Vbx*& vbs = nop.vbs;
	int& size = nop.size;
	const Oidx& id = nop.id;
	const Oidx& enterprise = nop.enterprise;
	OctetStr& securityName = nop.securityName;
	int& mpModel = nop.mpModel;
#ifdef _SNMPv3
	int& securityLevel = nop.securityLevel;
	const OctetStr& contextName = nop.contextName;
	unsigned int& timestamp = nop.timestamp;
	UTarget*& target = nop.target;
#else
	CTarget*& target = nop.target;
#endif

	long targetDomain;
	targetDomain = ((snmpTargetAddrTDomain*)cur.get()->first())->get_state();

	Oidx trapoid(id);
#ifdef _SNMPv3
	Pdux pdu;
	pdu.set_type((notify == TRAP) ? sNMP_PDU_TRAP : sNMP_PDU_INFORM);
	pdu.set_vblist(vbs, size);
	pdu.set_notify_id(id);
	pdu.set_notify_enterprise(enterprise);
	pdu.set_notify_timestamp(timestamp);
	pdu.set_security_level(securityLevel);
	pdu.set_context_name(contextName);

	  // get community info from SNMP-COMMUNITY-MIB
	  if (((mpModel == mpV1) || (mpModel == mpV2c)) &&
	      (snmpCommunityEntry::instance)) {
		if (snmpCommunityEntry::instance->get_community(securityName,
								*localEngineID,
								contextName)) {
			target->set_security_name(securityName);
		}
		else {
			LOG_BEGIN(loggerModuleName, WARNING_LOG | 2);
			LOG("NotificationOriginator: community lookup failed for (securityName) (context)");
			LOG(securityName.get_printable());
			LOG(OctetStr(contextName).get_printable());
			LOG_END;
		}
	  }
#endif
	  int status = SNMP_ERROR_SUCCESS;

	  if (mpModel == mpV1) {

#ifdef _SNMPv3
	    pdu.set_type(sNMP_PDU_V1TRAP);
#endif
	    target->set_version(version1);

#ifdef _SNMPv3
	    status = SnmpRequestV3::send(*target, pdu);
	    if (nlmLogEntry::instance) {
	      nlmLogEntry::instance->
		add_notification(target, id, vbs, size, contextName,
				 *localEngineID,
				 *localEngineID);
	    }
#else
	    status = SnmpRequest::process_trap(*target, vbs, size,
						   id, enterprise,
						   (notify != TRAP));
#endif

	    GenAddress address;
	    target->get_address(address);

	    LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
	    LOG("NotificationGenerator: sent v1 trap (id)(tdomain)(addr)(vbs)(community)");
	    LOG(trapoid.get_printable());
	    LOG(targetDomain);
	    LOG(address.get_printable());
	    LOG(size);
	    LOG(securityName.get_printable());
	    LOG_END;
	  }
	  else {
#ifdef _SNMPv3
	    if (mpModel == mpV3)
		target->set_version(version3);
	    else
#endif
		target->set_version(version2c);

	    if (notify != TRAP) {
		target->set_retry(((SnmpInt32MinMax*)
				   cur.get()->get_nth(3))->get_state());
		target->set_timeout(((SnmpInt32MinMax*)
				     cur.get()->get_nth(2))->get_state());
	    }

#ifdef _SNMPv3
	    status = SnmpRequestV3::send(*target, pdu);
	    if (nlmLogEntry::instance) {
	      OctetStr ceid;
	      nlmLogEntry::instance->
		add_notification(target, trapoid, vbs, size, 
				 contextName, ceid,
				 *localEngineID);
	    }
#else
	    status = SnmpRequest::process_trap(*target, vbs, size, id, enterprise,
				      (notify != TRAP));
#endif

	    GenAddress address;
	    target->get_address(address);

	    LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
	    if (notify == TRAP)
		LOG("NotificationGenerator: sent trap (vers)(id)(tdomain)(addr)(vbs)(community/secName)(status)");
	    else
		LOG("NotificationGenerator: sent inform (vers)(id)(tdomain)(addr)(vbs)(community/secName)(status)");
	    LOG(mpModel);
	    LOG(trapoid.get_printable());
	    LOG(targetDomain);
	    LOG(address.get_printable());
	    LOG(size);
	    LOG(securityName.get_printable());
	    LOG(status);
	    LOG_END;
	  }
	  return status;
}


bool NotificationOriginator::add_v1_trap_destination(const UdpAddress& addr,
    const OctetStr &name, const OctetStr &tag, const OctetStr &community)
{
	OctetStr address;
	IpAddress ip(addr);
	for (int i=0; i<addr.get_length()-2; i++) {
		address += (unsigned char)ip[i];
	}
	address += (addr.get_port() >> 8);
	address += (addr.get_port() & 0x00FF);

	if (!snmpTargetParamsEntry::instance ||
	    !snmpTargetAddrEntry::instance)
	    return FALSE;

	if (snmpTargetParamsEntry::instance->add_entry(name, // row index
						       0,    // mpModel
						       1,    // securityModel
						       community, // secName
						       1)) { // secLevel
	    snmpNotifyEntry::instance->add_entry(name, // row index
						 tag,  // tag
						 TRAP);   // type (trap)
	}
	if (snmpTargetAddrEntry::instance->
		add_entry(UdpAddress(addr).get_printable(),  // row index
			  Oidx("1.3.6.1.6.1.1"),    // UDP domain
			  address,                  // target address
			  tag,                      // tag
			  name))                   // params entry
		return TRUE;
	return FALSE;
}


bool NotificationOriginator::add_v2_trap_destination(const UdpAddress& addr,
    const OctetStr &name, const OctetStr &tag, const OctetStr &community)
{
	OctetStr address;
	IpAddress ip(addr);
	for (int i=0; i<addr.get_length()-2; i++) {
		address += (unsigned char)ip[i];
	}
	address += (addr.get_port() >> 8);
	address += (addr.get_port() & 0x00FF);

	if (!snmpTargetParamsEntry::instance ||
	    !snmpTargetAddrEntry::instance)
	    return FALSE;

	if (snmpTargetParamsEntry::instance->add_entry(name, // row index
						       mpV2c,    // mpModel
						       SNMP_SECURITY_MODEL_V2,
						       community, // secName
						       1)) { // secLevel
	    snmpNotifyEntry::instance->add_entry(name, // row index
						 tag,  // tag
						 TRAP);   // type (trap)
	}
	if (snmpTargetAddrEntry::instance->
		add_entry(UdpAddress(addr).get_printable(),  // row index
			  Oidx("1.3.6.1.6.1.1"),    // UDP domain
			  address,                  // target address
			  tag,                      // tag
			  name))                   // params entry
		return TRUE;
	return FALSE;
}

bool NotificationOriginator::add_v3_trap_destination(const UdpAddress& addr,
                                                     const OctetStr &name,
                                                     const OctetStr &tag,
                                                     const OctetStr &secName,
                                                     const int secLevel)
{
	OctetStr address;
	IpAddress ip(addr);
	for (int i=0; i<addr.get_length()-2; i++) {
		address += (unsigned char)ip[i];
	}
	address += (addr.get_port() >> 8);
	address += (addr.get_port() & 0x00FF);

	if (!snmpTargetParamsEntry::instance ||
	    !snmpTargetAddrEntry::instance)
	    return FALSE;

	if (snmpTargetParamsEntry::instance->add_entry(name, // row index
						       mpV3,    // mpModel
						       SNMP_SECURITY_MODEL_USM,
						       secName,
						       secLevel)) {
	    snmpNotifyEntry::instance->add_entry(name, // row index
						 tag,  // tag
						 TRAP);   // type (trap)
	}
	if (snmpTargetAddrEntry::instance->
		add_entry(UdpAddress(addr).get_printable(),  // row index
			  Oidx("1.3.6.1.6.1.1"),    // UDP domain
			  address,                  // target address
			  tag,                      // tag
			  name))                   // params entry
		return TRUE;
	return FALSE;
}

#ifdef AGENTPP_NAMESPACE
}
#endif

