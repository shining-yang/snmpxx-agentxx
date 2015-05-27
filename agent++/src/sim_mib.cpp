/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - sim_mib.cpp  
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

#include <agent_pp/sim_mib.h>
#include <snmp_pp/target.h>
#include <agent_pp/snmp_request.h> 
#include <agent_pp/system_group.h>
#include <snmp_pp/log.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

SimMib::SimMib(): Mib()
{
}

SimMib::~SimMib() 
{
}



SimMibLeaf::SimMibLeaf(): MibLeaf() 
{
}

SimMibLeaf::SimMibLeaf(const Oidx& o, mib_access a, SmiUINT32 s): 
  MibLeaf(o, a, s)
{
}

SimMibLeaf::SimMibLeaf(const Oidx& o, mib_access a, SnmpSyntax* s): 
  MibLeaf(o, a, s)
{
}

SimMibLeaf::SimMibLeaf(const Oidx& o, mib_access a, SnmpSyntax* s, bool d):
  MibLeaf(o, a, s, d)
{
}

SimMibLeaf::SimMibLeaf(const SimMibLeaf& other): MibLeaf(other) 
{
}

SimMibLeaf::~SimMibLeaf()
{
}

MibEntryPtr SimMibLeaf::clone()
{
	MibEntryPtr other = new SimMibLeaf(*this);
	return other;
}

mib_access SimMibLeaf::get_access()
{
	if ((configMode) && (my_table)) {
		return READCREATE;
	}
	if (configMode) return READWRITE;
	return MibLeaf::get_access();
}  

void SimMibLeaf::set_config_mode() 
{
	configMode = TRUE;
}

void SimMibLeaf::unset_config_mode() 
{
	configMode = FALSE;
}

bool SimMibLeaf::get_config_mode() 
{
	return configMode;
}

bool SimMibLeaf::configMode = FALSE;


int SimMibTable::check_creation(Request* req, int& ind) 
{
	if (SimMibLeaf::get_config_mode()) {
		return SNMP_ERROR_SUCCESS;
	}
	else return MibTable::check_creation(req, ind);
}

MibEntryPtr SimRowStatus::clone()
{
	MibEntryPtr other = new SimRowStatus(oid, access);
	((SimRowStatus*)other)->replace_value(value->clone());
	((SimRowStatus*)other)->set_reference_to_table(my_table);
	return other;
}

bool SimRowStatus::transition_ok(const Vbx& vb)
{
	if (SimMibLeaf::get_config_mode()) {
		return TRUE;
	}
	else return snmpRowStatus::transition_ok(vb);
}

mib_access SimRowStatus::get_access()
{
	if ((SimMibLeaf::get_config_mode()) && (my_table)) {
		return READCREATE;
	}
	if (SimMibLeaf::get_config_mode()) return READWRITE;
	return MibLeaf::get_access();
}  



/**********************************************************************
 *  
 *  class simSysUpTime
 * 
 **********************************************************************/

time_t simSysUpTime::start = 0;

simSysUpTime::simSysUpTime(): SimMibLeaf(oidSysUpTime, READONLY, 
					 new TimeTicks(0))
{
	start = get_currentTime();
}

time_t simSysUpTime::get_currentTime()
{
	time_t now;
	time(&now);
	return now;
}

time_t simSysUpTime::get()
{
	return (get_currentTime() - start)*100;
}

void simSysUpTime::get_request(Request* req, int ind)
{
	*((TimeTicks*)value) = (unsigned long)get();
	MibLeaf::get_request(req, ind);
}

simSysGroup::simSysGroup(): MibGroup("1.3.6.1.2.1.1", "simSysGroup")
{
	add(new simSysUpTime());
	add(new SimMibLeaf("1.3.6.1.2.1.1.1.0", READONLY, 
			   new OctetStr(""), FALSE));
	add(new SimMibLeaf("1.3.6.1.2.1.1.2.0", READONLY, 
			   new Oid(""), FALSE));
	add(new SimMibLeaf("1.3.6.1.2.1.1.4.0", READWRITE, 
			   new OctetStr(""), FALSE));
	add(new SimMibLeaf("1.3.6.1.2.1.1.5.0", READWRITE, 
			   new OctetStr(""), FALSE));
	add(new SimMibLeaf("1.3.6.1.2.1.1.6.0", READWRITE, 
			   new OctetStr(""), FALSE));
	add(new SimMibLeaf("1.3.6.1.2.1.1.7.0", READONLY,
			   new SnmpInt32(0), FALSE));
}

#ifdef AGENTPP_NAMESPACE
}
#endif
