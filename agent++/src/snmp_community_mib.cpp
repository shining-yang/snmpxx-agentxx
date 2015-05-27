/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - snmp_community_mib.cpp  
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

#include <agent_pp/snmp_community_mib.h>
#include <agent_pp/snmp_target_mib.h>
#include <snmp_pp/log.h>

#ifdef _SNMPv3

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

static const char *loggerModuleName = "agent++.snmp_community_mib";

/**
 *  snmpTargetAddrTMask
 *
 */

snmpTargetAddrTMask::snmpTargetAddrTMask(const Oidx& id):
   snmpTargetAddrTAddress(id, READCREATE, new OctetStr(""), VMODE_DEFAULT)
{

}

snmpTargetAddrTMask::~snmpTargetAddrTMask()
{
}

MibEntryPtr snmpTargetAddrTMask::clone()
{
	MibEntryPtr other = new snmpTargetAddrTMask(oid);
	((snmpTargetAddrTMask*)other)->replace_value(value->clone());
	((snmpTargetAddrTMask*)other)->set_reference_to_table(my_table);
	return other;
}

UdpAddress* snmpTargetAddrTMask::getUdpAddress()
{
	snmpTargetAddrEntry::instance->start_synch();
	MibTableRow* r =
	  snmpTargetAddrEntry::instance->find_index(my_row->get_index());
	if (!r)
	{
	  snmpTargetAddrEntry::instance->end_synch();
	  return 0;
	}
	int domain = ((snmpTargetAddrTDomain*)r->get_nth(0))->get_state();
	snmpTargetAddrEntry::instance->end_synch();
	switch (domain) {
	    case 1:
	    case 101:
	    case 102:
		UdpAddress* address = new UdpAddress();
		*address =  (*(OctetStr*)value);
		return address;
	}
	return 0;
}

int snmpTargetAddrTMask::prepare_set_request(Request* req, int& ind)
{
	Vb vb(req->get_value(ind));
	OctetStr v;
	if (vb.get_value(v) != SNMP_CLASS_SUCCESS)
	    return SNMP_ERROR_WRONG_TYPE;
	if (!(((v.len() >= 0) && (v.len() <= 255))))
		 return SNMP_ERROR_WRONG_LENGTH;
	// check if snmpTargetAddrTMask has same length as
	// snmp anmpTargetAddrTAddress
	if (snmpTargetAddrEntry::instance) {
	    if (req->lock_index(snmpTargetAddrEntry::instance) < 0) {
		snmpTargetAddrEntry::instance->start_synch();
	    }
	    MibTableRow* r =
		snmpTargetAddrEntry::instance->find_index(my_row->get_index());
	    OctetStr addr;
	    long status = ((snmpRowStatus*)r->get_nth(7))->get();
	    r->get_nth(1)->get_value(addr);
	    if (req->lock_index(snmpTargetAddrEntry::instance) < 0) {
		snmpTargetAddrEntry::instance->end_synch();
	    }
	    if ((status == 1) || ((v.len()!=0) && (addr.len() != v.len()))) {
		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 7);
		LOG("Setting snmpTargetAddrTMask failed (rowStatus)(maskLen)(addrLen)");
		LOG(status);
		LOG(v.len());
		LOG(addr.len());
		LOG_END;
		return SNMP_ERROR_INCONSIST_VAL;
	    }
	}
	return SNMP_ERROR_SUCCESS;
}

/**
 *  snmpCommunityEntry
 *
 */

snmpCommunityEntry* snmpCommunityEntry::instance = 0;

const index_info iSnmpCommunityEntry[1] =
{ { sNMP_SYNTAX_OCTETS, TRUE, 1, 32 } };

snmpCommunityEntry::snmpCommunityEntry():
   StorageTable(oidSnmpCommunityEntry, iSnmpCommunityEntry, 1)
{
	// This table object is a singleton. In order to access it use
	// the static pointer snmpCommunityEntry::instance.
	instance = this;

	if (!v3MP::I)
	{
	  LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
	  LOG("v3MP must be initialized before snmpCommunityTable");
	  LOG_END;
	  return;
	}

	add_col(new MibLeaf("2", READCREATE, new OctetStr(""), FALSE));
	add_col(new SnmpAdminString("3", READCREATE,
				    new OctetStr(""), FALSE, 1, 32));
	add_col(new SnmpEngineID("4", READCREATE,
				 new OctetStr(v3MP::I->get_local_engine_id()),
				 VMODE_DEFAULT));
	add_col(new SnmpAdminString("5", READCREATE,
				    new OctetStr(""), TRUE, 1, 32));
	add_col(new SnmpTagValue("6"));
	add_storage_col(new StorageType("7", 3));
	add_col(new snmpRowStatus("8", READCREATE));
}

snmpCommunityEntry::~snmpCommunityEntry()
{
	instance = 0;
}


void snmpCommunityEntry::set_row(MibTableRow* r,
				 const OctetStr& p0,
				 const OctetStr& p1,
				 const OctetStr& p2,
				 const OctetStr& p3,
				 const OctetStr& p4, int p5, int p6)
{
	r->get_nth(0)->replace_value(new OctetStr(p0));
	r->get_nth(1)->replace_value(new OctetStr(p1));
	r->get_nth(2)->replace_value(new OctetStr(p2));
	r->get_nth(3)->replace_value(new OctetStr(p3));
	r->get_nth(4)->replace_value(new OctetStr(p4));
	r->get_nth(5)->replace_value(new SnmpInt32(p5));
	r->get_nth(6)->replace_value(new SnmpInt32(p6));
}

bool snmpCommunityEntry::get_v3_info(OctetStr& security_name,
					OctetStr& context_engine_id,
					OctetStr& context_name,
					OctetStr& transport_tag)
{
	OctetStr community(security_name);
	List<MibTableRow>* list = get_rows_cloned(TRUE);
	ListCursor<MibTableRow> cur;
	for (cur.init(list); cur.get(); cur.next()) {
		OctetStr entry;
		cur.get()->get_nth(0)->get_value(entry);
		if (entry == community) {

			cur.get()->get_nth(1)->get_value(security_name);
			cur.get()->get_nth(2)->get_value(context_engine_id);
			cur.get()->get_nth(3)->get_value(context_name);
			cur.get()->get_nth(4)->get_value(transport_tag);

			LOG_BEGIN(loggerModuleName, INFO_LOG | 2);
			LOG("snmpCommunityEntry: found v3 info for (community)(security_name)(tag)");
			LOG(community.get_printable());
			LOG(transport_tag.get_printable());
			LOG_END;

			delete list;
			return TRUE;
		}

	}
	delete list;
	return FALSE;
}

bool snmpCommunityEntry::get_community(OctetStr& security_name,
					  const OctetStr& context_engine_id,
					  const OctetStr& context_name)
{
	List<MibTableRow>* list = get_rows_cloned(TRUE);
	ListCursor<MibTableRow> cur;
	for (cur.init(list); cur.get(); cur.next()) {
		OctetStr sname;
		cur.get()->get_nth(1)->get_value(sname);
		OctetStr eid;
		cur.get()->get_nth(2)->get_value(eid);
		OctetStr cname;
		cur.get()->get_nth(3)->get_value(cname);
		if ((sname == security_name) &&
		    (eid == context_engine_id) &&
		    (cname == context_name)) {

			cur.get()->get_nth(0)->get_value(security_name);

			LOG_BEGIN(loggerModuleName, INFO_LOG | 2);
			LOG("snmpCommunityEntry: found community for (sname)(context)");
			LOG(sname.get_printable());
			LOG(cname.get_printable());
			LOG_END;

			delete list;
			return TRUE;
		}

	}
	delete list;
	return FALSE;
}


/**
 *  snmpTargetAddrExtEntry
 *
 */

snmpTargetAddrExtEntry* snmpTargetAddrExtEntry::instance = 0;

snmpTargetAddrExtEntry::snmpTargetAddrExtEntry():
   MibTable(oidSnmpTargetAddrExtEntry, iSnmpAdminString, 1)
{
	// This table object is a singleton. In order to access it use
	// the static pointer snmpTargetAddrExtEntry::instance.
	instance = this;

	add_col(new snmpTargetAddrTMask("1"));
	add_col(new SnmpInt32MinMax("2", READCREATE, 484, VMODE_DEFAULT,
				    484, 2147483647));

	if (snmpTargetAddrEntry::instance) {
		snmpTargetAddrEntry::instance->add_listener(this);
	}
	else {
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
		LOG("Please instantiate snmpTargetAddrEntry before snmpTargetAddrExtEntry");
		LOG_END;
	}
}

snmpTargetAddrExtEntry::~snmpTargetAddrExtEntry()
{
	instance = 0;
	if (snmpTargetAddrEntry::instance) {
		snmpTargetAddrEntry::instance->remove_listener(this);
	}
}


void snmpTargetAddrExtEntry::row_added(MibTableRow* row, const Oidx& index,
				       MibTable* source)
{
	if (source) add_row(index);
}

void snmpTargetAddrExtEntry::row_delete(MibTableRow* row, const Oidx& index,
					 MibTable* source)
{
	if (source) remove_row(index);
}

int snmpTargetAddrExtEntry::prepare_set_request(Request* req, int& ind)
{
	Oidx oid(req->get_oid(ind));
	if (!find(oid)) {
		Oidx rs("1.3.6.1.6.3.12.1.2.1.9");
		rs += index(oid);
		Vbx* status = req->search_value(rs);
		if (!status)
			return SNMP_ERROR_INCONSIS_NAME;
		long value = 0;
		if (status->get_value(value) != SNMP_CLASS_SUCCESS)
		{
		    delete status;
		    return SNMP_ERROR_WRONG_TYPE;
		}
		delete status;
		if ((value != rowCreateAndWait) &&
		    (value != rowCreateAndGo)) {
			return SNMP_ERROR_INCONSIS_NAME;
		}
	}
	return MibTable::prepare_set_request(req, ind);
}

void snmpTargetAddrExtEntry::set_row(MibTableRow* r, const OctetStr& p0, int p1)
{
	r->get_nth(0)->replace_value(new OctetStr(p0));
	r->get_nth(1)->replace_value(new SnmpInt32(p1));
}

#ifdef _SNMPv3
bool snmpTargetAddrExtEntry::passes_filter(const OctetStr& tag,
					      const UTarget& addr)
{
	if (!snmpTargetAddrEntry::instance) return TRUE;
	if (tag.len() == 0) return TRUE;
	GenAddress gen;
	addr.get_address(gen);
	if (gen.get_type() != Address::type_udp) return FALSE;
	UdpAddress u(gen);

	start_synch();
	List<MibTableRow>* list =
	  snmpTargetAddrEntry::instance->get_rows_cloned_for_tag(tag);
	ListCursor<MibTableRow> cur;
	for (cur.init(list); cur.get(); cur.next()) {
		MibTableRow* ext = find_index(cur.get()->get_index());
		if (ext) {
			UdpAddress* address =
			((snmpTargetAddrTAddress*)cur.get()->
			 get_nth(1))->getUdpAddress();
			if (!address) {
				LOG_BEGIN(loggerModuleName, WARNING_LOG | 4);
				LOG("snmpTargetAddrExtEntry: unsupported domain (entry)");
				LOG(cur.get()->get_index().get_printable());
				LOG_END;
				continue;
			}
			UdpAddress* mask =
			  ((snmpTargetAddrTMask*)ext->get_nth(0))->
			  getUdpAddress();
			UdpAddress a(*address);
			a.mask(*mask);
			UdpAddress b(u);
			b.mask(*mask);
			delete address;
			a.set_port(a.get_port() & mask->get_port());
			b.set_port(b.get_port() & mask->get_port());
			delete mask;
			if (a == b) {
			    end_synch();
			    delete list;
			    return TRUE;
			}

			LOG_BEGIN(loggerModuleName, DEBUG_LOG | 4);
			LOG("snmpTargetAddrExtEntry: not matched (match)(addr)");
			LOG(a.get_printable());
			LOG(b.get_printable());
			LOG_END;
		}
	}
	end_synch();
	delete list;
	return FALSE;
}
#endif

bool snmpTargetAddrExtEntry::passes_filter(const OctetStr& taddress,
					      const OctetStr& tag)
{
	if (!snmpTargetAddrEntry::instance) return TRUE;
	if (tag.len() == 0) return TRUE;

	start_synch();
	List<MibTableRow>* list =
	  snmpTargetAddrEntry::instance->get_rows_cloned_for_tag(tag);
	ListCursor<MibTableRow> cur;
	for (cur.init(list); cur.get(); cur.next()) {
		MibTableRow* ext = find_index(cur.get()->get_index());
		if (ext) {
		    OctetStr taddressRequested(taddress);
		    OctetStr taddressAllowed;
		    ((snmpTargetAddrTAddress*)cur.get()->
		     get_nth(1))->get_value(taddressAllowed);
		    OctetStr mask;
		    ((snmpTargetAddrTMask*)ext->get_nth(0))->get_value(mask);
		    for (unsigned int i=0; i<mask.len(); i++) {
			if (i<taddressAllowed.len()) {
			    taddressAllowed[i] = taddressAllowed[i] & mask[i];
			}
			if (i<taddressRequested.len()) {
			    taddressRequested[i] =
				taddressRequested[i] & mask[i];
			}
		    }
		    if (taddressRequested == taddressAllowed) {
			end_synch();
			delete list;
			LOG_BEGIN(loggerModuleName, INFO_LOG | 4);
			LOG("snmpTargetAddrExtEntry: matched (match)(req)");
			LOG(taddressAllowed.get_printable());
			LOG(taddressRequested.get_printable());
			LOG_END;
			return TRUE;
		    }
		    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 4);
		    LOG("snmpTargetAddrExtEntry: not matched (match)(req)");
		    LOG(taddressAllowed.get_printable());
		    LOG(taddressRequested.get_printable());
		    LOG_END;
		}
	}
	end_synch();
	delete list;
	return FALSE;
}


snmp_community_mib::snmp_community_mib(): MibGroup("1.3.6.1.6.3.18.1",
						   "snmpCommunityMIB")
{
	add(new snmpCommunityEntry());
	add(new snmpTargetAddrExtEntry());
}


void snmp_community_mib::add_public()
{
  if (!v3MP::I)
  {
	  LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
    LOG("v3MP must be initialized before snmpCommunityTable");
    LOG_END;
    return;
  }

  Oidx ind = Oidx::from_string("public", FALSE);
  MibTableRow* r = snmpCommunityEntry::instance->find_index(ind);
  if (!r) r = snmpCommunityEntry::instance->add_row(ind);
  snmpCommunityEntry::instance->set_row(r,
                                        OctetStr("public"),
                                        OctetStr("public"),
                                        v3MP::I->get_local_engine_id(),
                                        OctetStr(""),
                                        OctetStr("access"),
                                        3, 1);

  ind = Oidx::from_string("localAccess", FALSE);
  r = snmpTargetAddrEntry::instance->find_index(ind);
  if (!r) r = snmpTargetAddrEntry::instance->add_row(ind);
  snmpTargetAddrEntry::instance->
            set_row(r, "1.3.6.1.6.1.1",
                    OctetStr::from_hex_string("7F 00 00 01 00 A1"),
                    1500, 3,
                    "access",
                    "localAccess",
                    3, 1);

  ind = Oidx::from_string("localAccess");
  r = snmpTargetAddrExtEntry::instance->find_index(ind);
  if (!r) r = snmpTargetAddrExtEntry::instance->add_row(ind);
  snmpTargetAddrExtEntry::instance->
            set_row(r, "\xFF\xFF\xFF\xFF\xFF\xFF", 1500);
}

#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
