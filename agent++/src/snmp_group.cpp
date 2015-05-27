/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - snmp_group.cpp  
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

#include <agent_pp/snmp_counters.h>
#include <agent_pp/snmp_group.h>

#ifdef AGENTPP_NAMESPACE
using namespace Agentpp;
#endif


void snmpInPkts::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::inPkts();
	MibLeaf::get_request(req, index);
}

void snmpOutPkts::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::outPkts();
	MibLeaf::get_request(req, index);
}

void snmpInBadVersions::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::inBadVersions();
	MibLeaf::get_request(req, index);
}

void snmpInBadCommunityNames::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::inBadCommunityNames();
	MibLeaf::get_request(req, index);
}

void snmpInBadCommunityUses::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::inBadCommunityUses();
	MibLeaf::get_request(req, index);
}

void snmpInASNParseErrs::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::inASNParseErrs();
	MibLeaf::get_request(req, index);
}

void snmpInTooBigs::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::inTooBigs();
	MibLeaf::get_request(req, index);
}

void snmpInNoSuchNames::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::inNoSuchNames();
	MibLeaf::get_request(req, index);
}

void snmpInBadValues::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::inBadValues();
	MibLeaf::get_request(req, index);
}

void snmpInReadOnlys::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::inReadOnlys();
	MibLeaf::get_request(req, index);
}

void snmpInGenErrs::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::inGenErrs();
	MibLeaf::get_request(req, index);
}

void snmpInTotalReqVars::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::inTotalReqVars();
	MibLeaf::get_request(req, index);
}

void snmpInTotalSetVars::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::inTotalSetVars();
	MibLeaf::get_request(req, index);
}

void snmpInGetRequests::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::inGetRequests();
	MibLeaf::get_request(req, index);
}

void snmpInGetNexts::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::inGetNexts();
	MibLeaf::get_request(req, index);
}

void snmpInSetRequests::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::inSetRequests();
	MibLeaf::get_request(req, index);
}

void snmpInGetResponses::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::inGetResponses();
	MibLeaf::get_request(req, index);
}

void snmpInTraps::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::inTraps();
	MibLeaf::get_request(req, index);
}

void snmpOutTooBigs::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::outTooBigs();
	MibLeaf::get_request(req, index);
}

void snmpOutNoSuchNames::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::outNoSuchNames();
	MibLeaf::get_request(req, index);
}

void snmpOutBadValues::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::outBadValues();
	MibLeaf::get_request(req, index);
}

void snmpOutGenErrs::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::outGenErrs();
	MibLeaf::get_request(req, index);
}

void snmpOutGetRequests::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::outGetRequests();
	MibLeaf::get_request(req, index);
}

void snmpOutGetNexts::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::outGetNexts();
	MibLeaf::get_request(req, index);
}

void snmpOutSetRequests::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::outSetRequests();
	MibLeaf::get_request(req, index);
}

void snmpOutGetResponses::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::outGetResponses();
	MibLeaf::get_request(req, index);
}

void snmpOutTraps::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::outTraps();
	MibLeaf::get_request(req, index);
}

void snmpSilentDrops::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::silentDrops();
	MibLeaf::get_request(req, index);
}

void snmpProxyDrops::get_request(Request* req, int index)
{
	*((Counter32*)value) = MibIIsnmpCounters::proxyDrops();
	MibLeaf::get_request(req, index);
}


/**
 *  snmpEnableAuthenTraps
 *
 */

snmpEnableAuthenTraps* snmpEnableAuthenTraps::instance = 0;

snmpEnableAuthenTraps::snmpEnableAuthenTraps():
   MibLeaf(oidSnmpEnableAuthenTraps, READWRITE, new SnmpInt32(2))
{
	instance = this;
}

snmpEnableAuthenTraps::~snmpEnableAuthenTraps()
{
}

long snmpEnableAuthenTraps::get_state()
{
	return (long)*((SnmpInt32*)value);
}

bool snmpEnableAuthenTraps::value_ok(const Vbx& vb)
{
	long v;
	if (vb.get_value(v) != SNMP_CLASS_SUCCESS)
	    return FALSE;
	if ((v != 1)
	     && (v != 2)) return FALSE;
	return TRUE;
}


/**********************************************************************
 * 
 *  class snmpGroup
 *
 **********************************************************************/


snmpGroup::snmpGroup(): MibGroup(oidSnmpGroup, "snmpGroup")
{
	MibIIsnmpCounters::reset();

	add(new snmpInPkts());
	add(new snmpOutPkts());
	add(new snmpInBadVersions());
	add(new snmpInBadCommunityNames());
	add(new snmpInBadCommunityUses());
	add(new snmpInASNParseErrs());
	add(new snmpInTooBigs());
	add(new snmpInNoSuchNames());
	add(new snmpInBadValues());
	add(new snmpInReadOnlys());
	add(new snmpInGenErrs());
	add(new snmpInTotalReqVars());
	add(new snmpInTotalSetVars());
	add(new snmpInGetRequests());
	add(new snmpInGetNexts());
	add(new snmpInSetRequests());
	add(new snmpInGetResponses());
	add(new snmpInTraps());
	add(new snmpOutTooBigs());
	add(new snmpOutNoSuchNames());
	add(new snmpOutBadValues());
	add(new snmpOutGenErrs());
	add(new snmpOutGetRequests());
	add(new snmpOutGetNexts());
	add(new snmpOutSetRequests());
	add(new snmpOutGetResponses());
	add(new snmpOutTraps());
	add(new snmpEnableAuthenTraps());
	add(new snmpSilentDrops());
	add(new snmpProxyDrops());
}	
