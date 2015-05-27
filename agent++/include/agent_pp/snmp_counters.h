/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - snmp_counters.h  
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

#ifndef snmp_counters_h_
#define snmp_counters_h_

#include <agent_pp/agent++.h>
#include <agent_pp/snmp_pp_ext.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

#define SNMP_COUNTERS 29



/**********************************************************************
 *  
 *  class MibIIsnmpCounter
 * 
 */

class AGENTPP_DECL MibIIsnmpCounters {

public:
	MibIIsnmpCounters();

	static NS_SNMP Counter32 inPkts()	       	{ return counter_snmp[0]; }
	static NS_SNMP Counter32 outPkts()	       	{ return counter_snmp[1]; }
	static NS_SNMP Counter32 inBadVersions()        { return counter_snmp[2]; }
	static NS_SNMP Counter32 inBadCommunityNames() 	{ return counter_snmp[3]; }
	static NS_SNMP Counter32 inBadCommunityUses()  	{ return counter_snmp[4]; }
	static NS_SNMP Counter32 inASNParseErrs()      	{ return counter_snmp[5]; }
	static NS_SNMP Counter32 inTooBigs()	       	{ return counter_snmp[6]; }
	static NS_SNMP Counter32 inNoSuchNames()       	{ return counter_snmp[7]; }
	static NS_SNMP Counter32 inBadValues()	       	{ return counter_snmp[8]; }
	static NS_SNMP Counter32 inReadOnlys()	       	{ return counter_snmp[9]; }
	static NS_SNMP Counter32 inGenErrs()	       	{ return counter_snmp[10]; }
	static NS_SNMP Counter32 inTotalReqVars()      	{ return counter_snmp[11]; }
	static NS_SNMP Counter32 inTotalSetVars()      	{ return counter_snmp[12]; }
	static NS_SNMP Counter32 inGetRequests()       	{ return counter_snmp[13]; }
	static NS_SNMP Counter32 inGetNexts()	       	{ return counter_snmp[14]; }
	static NS_SNMP Counter32 inSetRequests()       	{ return counter_snmp[15]; }
	static NS_SNMP Counter32 inGetResponses()      	{ return counter_snmp[16]; }
	static NS_SNMP Counter32 inTraps()	       	{ return counter_snmp[17]; }
	static NS_SNMP Counter32 outTooBigs()	       	{ return counter_snmp[18]; }
	static NS_SNMP Counter32 outNoSuchNames()      	{ return counter_snmp[19]; }
	static NS_SNMP Counter32 outBadValues()		{ return counter_snmp[20]; }
	static NS_SNMP Counter32 outGenErrs()	       	{ return counter_snmp[21]; }
	static NS_SNMP Counter32 outGetRequests()      	{ return counter_snmp[22]; }
	static NS_SNMP Counter32 outGetNexts()	       	{ return counter_snmp[23]; }
	static NS_SNMP Counter32 outSetRequests()      	{ return counter_snmp[24]; }
	static NS_SNMP Counter32 outGetResponses()     	{ return counter_snmp[25]; }
	static NS_SNMP Counter32 outTraps()	       	{ return counter_snmp[26]; }
	static NS_SNMP Counter32 silentDrops()      	{ return counter_snmp[27]; }
	static NS_SNMP Counter32 proxyDrops()     	{ return counter_snmp[28]; }


	static void      incInPkts()	       	{ counter_snmp[0]++; }
	static void      incOutPkts()	       	{ counter_snmp[1]++; }
	static void      incInBadVersions()     { counter_snmp[2]++; }
	static void      incInBadCommunityNames()     	{ counter_snmp[3]++; }
	static void      incInBadCommunityUses(){ counter_snmp[4]++; }
	static void      incInASNParseErrs()   	{ counter_snmp[5]++; }
	static void      incInTooBigs()	       	{ counter_snmp[6]++; }
	static void      incInNoSuchNames()    	{ counter_snmp[7]++; }
	static void      incInBadValues()       { counter_snmp[8]++; }
	static void      incInReadOnlys()	{ counter_snmp[9]++; }
	static void      incInGenErrs()	       	{ counter_snmp[10]++; }
	static void      incInTotalReqVars()   	{ counter_snmp[11]++; }
	static void      incInTotalSetVars()   	{ counter_snmp[12]++; }
	static void      incInGetRequests()    	{ counter_snmp[13]++; }
	static void      incInGetNexts()        { counter_snmp[14]++; }
	static void      incInSetRequests()    	{ counter_snmp[15]++; }
	static void      incInGetResponses()   	{ counter_snmp[16]++; }
	static void      incInTraps()	       	{ counter_snmp[17]++; }
	static void      incOutTooBigs()       	{ counter_snmp[18]++; }
	static void      incOutNoSuchNames()   	{ counter_snmp[19]++; }
	static void      incOutBadValues()      { counter_snmp[20]++; }
	static void      incOutGenErrs()        { counter_snmp[21]++; }
	static void      incOutGetRequests()   	{ counter_snmp[22]++; }
	static void      incOutGetNexts()       { counter_snmp[23]++; }
	static void      incOutSetRequests()   	{ counter_snmp[24]++; }
	static void      incOutGetResponses()  	{ counter_snmp[25]++; }
	static void      incOutTraps()	       	{ counter_snmp[26]++; }
	static void      incSilentDrops()  	{ counter_snmp[27]++; }
	static void      incProxyDrops()       	{ counter_snmp[28]++; }
	
	// set all counters to zero
	static void	 reset();

protected:
	static unsigned long counter_snmp[];
};

#ifdef AGENTPP_NAMESPACE
}
#endif
#endif
