/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - snmp_notification_mib.cpp  
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

#include <agent_pp/snmp_notification_mib.h>
#include <agent_pp/snmp_target_mib.h>
#include <agent_pp/mib_complex_entry.h>
#include <snmp_pp/log.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

static const char *loggerModuleName = "agent++.snmp_notification_mib";


/**
 *  snmpNotifyType
 *
 */


/**
 *  snmpNotifyRowStatus
 *
 */


/**
 *  snmpNotifyFilterProfileName
 *
 */


/**
 *  snmpNotifyFilterProfileRowStatus
 *
 */


/**
 *  snmpNotifyFilterMask
 *
 */



/**
 *  snmpNotifyFilterType
 *
 */


/**
 *  snmpNotifyFilterRowStatus
 *
 */



/**
 *  snmpNotifyEntry
 *
 */

snmpNotifyEntry* snmpNotifyEntry::instance = 0;

snmpNotifyEntry::snmpNotifyEntry():
   StorageTable("1.3.6.1.6.3.13.1.1.1", iSnmpAdminString, 1)
{
	// This table object is a singleton. In order to access it use
	// the static pointer snmpNotifyEntry::instance.
	instance = this;

	add_col(new SnmpTagValue("2"));
	add_col(new SnmpInt32MinMax("3", READCREATE, 1, VMODE_DEFAULT,
				    1, 2));
	add_storage_col(new StorageType("4", 3));
	add_col(new snmpRowStatus("5"));
}

snmpNotifyEntry::~snmpNotifyEntry()
{
}

MibTableRow* snmpNotifyEntry::add_entry(const OctetStr& name,
					const OctetStr& tag,
					const int type)
{
	start_synch();
	Oidx index = Oidx::from_string(name, FALSE);
	MibTableRow* r = find_index(index);
	if (r) {
		end_synch();
		return 0;
	}
	r = add_row(index);
	r->get_nth(0)->replace_value(new OctetStr(tag));
	r->get_nth(1)->replace_value(new SnmpInt32(type));
	// leave default value untouched (storage type)
	r->get_nth(3)->replace_value(new SnmpInt32(rowActive));
	end_synch();
	return r;
}


/**
 *  snmpNotifyFilterProfileEntry
 *
 */

snmpNotifyFilterProfileEntry* snmpNotifyFilterProfileEntry::instance = 0;

const index_info indSnmpNotifyFilterProfileEntry[1] = {
  { sNMP_SYNTAX_OCTETS, TRUE, 1, 32 } };

snmpNotifyFilterProfileEntry::snmpNotifyFilterProfileEntry():
   StorageTable("1.3.6.1.6.3.13.1.2.1", indSnmpNotifyFilterProfileEntry, 1)
{
	// This table object is a singleton. In order to access it use
	// the static pointer snmpNotifyFilterProfileEntry::instance.
	instance = this;

	add_col(new SnmpAdminString("1", READCREATE, new OctetStr(""),
				    VMODE_NONE, 1, 32));
	add_storage_col(new StorageType("2", 3));
	add_col(new snmpRowStatus("3"));
}

snmpNotifyFilterProfileEntry::~snmpNotifyFilterProfileEntry()
{
}



/**
 *  snmpNotifyFilterEntry
 *
 */

snmpNotifyFilterEntry* snmpNotifyFilterEntry::instance = 0;

const index_info	iSnmpNotifyFilterEntry[2] = 
{ { sNMP_SYNTAX_OCTETS, FALSE, 0, 32 }, { sNMP_SYNTAX_OID, TRUE, 1, 95 } };

snmpNotifyFilterEntry::snmpNotifyFilterEntry():
   StorageTable("1.3.6.1.6.3.13.1.3.1", iSnmpNotifyFilterEntry, 2)
{
	// This table object is a singleton. In order to access it use
	// the static pointer snmpNotifyFilterEntry::instance.
	instance = this;

	add_col(new OctetStrMinMax("2", READCREATE, new OctetStr(""),
				   VMODE_DEFAULT, 0, 16));
	add_col(new SnmpInt32MinMax("3", READCREATE, 1, VMODE_DEFAULT, 1, 2));
	add_storage_col(new StorageType("4", 3));
	add_col(new snmpRowStatus("5"));
}

snmpNotifyFilterEntry::~snmpNotifyFilterEntry()
{
}

bool snmpNotifyFilterEntry::passes_filter(const Oidx& target, 
					     const Oidx& oid,
					     const Vbx* vbs,
					     unsigned int vb_count)
{
	snmpNotifyFilterProfileEntry::instance->start_synch();
	MibTableRow* found = 
	  snmpNotifyFilterProfileEntry::instance->find_index(target);

	// no filter -> passes filter
	if (!found) {
		snmpNotifyFilterProfileEntry::instance->end_synch();
		return TRUE;
	} 
	OctetStr profileName;
	found->first()->get_value(profileName);
	snmpNotifyFilterProfileEntry::instance->end_synch();

	Oidx profileOid;
	profileOid = Oidx::from_string(profileName);

	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 4);
	LOG("NotificationOriginator: filter: using (profile) (as oid)");
	LOG(profileName.get_printable_hex());
	LOG(profileOid.get_printable());
	LOG_END;

	List<MibTableRow>* list = 
	  snmpNotifyFilterEntry::instance->get_rows_cloned(&profileOid, 
							   rowActive);
	ListCursor<MibTableRow> cur;

	// no filter -> passes filter
	if (list->size() == 0) {
		delete list;
		return TRUE;
	}

	OidList<MibStaticEntry> matches; 
	OidList<MibStaticEntry>* oidmatches = 
	  new OidList<MibStaticEntry>[vb_count]; 
	for (cur.init(list); cur.get(); cur.next()) {

		Oidx     subtree  = cur.get()->get_index();
		// no need to check: if (subtree.len()<3) continue;
		subtree = subtree.cut_left(subtree[0]+1);

		OctetStr filterMask;
		long     filterType;
		
		cur.get()->first()->get_value(filterMask);
		cur.get()->get_nth(1)->get_value(filterType);
		
		// check if oid is in the filter specified by filterMask
		// and subtree.
		if (subtree.compare(oid, filterMask) >= 0) {
			Oidx sid;
			sid += subtree.len();
			sid += subtree;
			MibStaticEntry* match = 
			  new MibStaticEntry(sid, SnmpInt32(filterType));
			matches.add(match);

			LOG_BEGIN(loggerModuleName, INFO_LOG | 4);
			LOG("NotificationOriginator: filter: (trapoid)(subtree)(filterMask)(filterType)(match)");
			LOG(Oidx(oid).get_printable());
			LOG(subtree.get_printable());
			LOG(filterMask.get_printable_hex());
			LOG(filterType);
			LOG(sid.get_printable());
			LOG_END;
		}
		else {

			LOG_BEGIN(loggerModuleName, INFO_LOG | 5);
			LOG("NotificationOriginator: filter: (trapoid)(subtree)(filterMask)(filterType)(match)");
			LOG(Oidx(oid).get_printable());
			LOG(subtree.get_printable());
			LOG(filterMask.get_printable_hex());
			LOG(filterType);
			LOG("no match");
			LOG_END;
		}
		for (unsigned int i=0; i<vb_count; i++) {
			if (subtree.compare(vbs[i].get_oid(),filterMask) >= 0){
				Oidx sid;
				sid += subtree.len();
				sid += subtree;
				MibStaticEntry* match = 
				  new MibStaticEntry(sid, 
						     SnmpInt32(filterType));
				oidmatches[i].add(match);
			}
		} 
	}
	delete list;
	if (matches.size() == 0) {
		delete[] oidmatches;
		return FALSE;
	}
	long pass = 0;
	matches.last()->get_value(pass);
	if (pass == 1) {
		for (unsigned int i=0; i<vb_count; i++) {
			if (oidmatches[i].size() > 0) {
				oidmatches[i].last()->get_value(pass);
				if (pass == 2) {
					delete[] oidmatches;
					return FALSE;
				}
			}
		} 
	}
	delete[] oidmatches;
	return (pass == 1);
}	

snmp_notification_mib::snmp_notification_mib(): MibGroup("1.3.6.1.6.3.13.1",
							 "snmpNotificationMIB")
{
	add(new snmpNotifyEntry());
	add(new snmpNotifyFilterProfileEntry());
	add(new snmpNotifyFilterEntry());
}

#ifdef AGENTPP_NAMESPACE
}
#endif
