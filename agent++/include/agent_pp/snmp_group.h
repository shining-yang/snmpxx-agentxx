/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - snmp_group.h  
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


#ifndef snmp_group_h_
#define snmp_group_h_


#include <string.h>

#include <agent_pp/agent++.h>
#include <agent_pp/mib.h>


#define oidSnmpGroup			"1.3.6.1.2.1.11"
#define oidSnmpInPkts			"1.3.6.1.2.1.11.1.0"
#define oidSnmpOutPkts			"1.3.6.1.2.1.11.2.0"
#define oidSnmpInBadVersions		"1.3.6.1.2.1.11.3.0"
#define oidSnmpInBadCommunityNames 	"1.3.6.1.2.1.11.4.0"
#define oidSnmpInBadCommunityUses  	"1.3.6.1.2.1.11.5.0"
#define oidSnmpInASNParseErrs      	"1.3.6.1.2.1.11.6.0"
#define oidSnmpInTooBigs	       	"1.3.6.1.2.1.11.8.0"
#define oidSnmpInNoSuchNames       	"1.3.6.1.2.1.11.9.0"
#define oidSnmpInBadValues	       	"1.3.6.1.2.1.11.10.0"
#define oidSnmpInReadOnlys	       	"1.3.6.1.2.1.11.11.0"
#define oidSnmpInGenErrs	       	"1.3.6.1.2.1.11.12.0"
#define oidSnmpInTotalReqVars      	"1.3.6.1.2.1.11.13.0"
#define oidSnmpInTotalSetVars      	"1.3.6.1.2.1.11.14.0"
#define oidSnmpInGetRequests       	"1.3.6.1.2.1.11.15.0"
#define oidSnmpInGetNexts	       	"1.3.6.1.2.1.11.16.0"
#define oidSnmpInSetRequests       	"1.3.6.1.2.1.11.17.0"
#define oidSnmpInGetResponses      	"1.3.6.1.2.1.11.18.0"
#define oidSnmpInTraps	       	        "1.3.6.1.2.1.11.19.0"
#define oidSnmpOutTooBigs	       	"1.3.6.1.2.1.11.20.0"
#define oidSnmpOutNoSuchNames      	"1.3.6.1.2.1.11.21.0"
#define oidSnmpOutBadValues		"1.3.6.1.2.1.11.22.0"
#define oidSnmpOutGenErrs	       	"1.3.6.1.2.1.11.24.0"
#define oidSnmpOutGetRequests      	"1.3.6.1.2.1.11.25.0"
#define oidSnmpOutGetNexts	       	"1.3.6.1.2.1.11.26.0"
#define oidSnmpOutSetRequests      	"1.3.6.1.2.1.11.27.0"
#define oidSnmpOutGetResponses     	"1.3.6.1.2.1.11.28.0"
#define oidSnmpOutTraps			"1.3.6.1.2.1.11.29.0"
#define oidSnmpEnableAuthenTraps	"1.3.6.1.2.1.11.30.0"
#define oidSnmpSilentDrops	       	"1.3.6.1.2.1.11.31.0"
#define oidSnmpProxyDrops		"1.3.6.1.2.1.11.32.0"

#define enableAuthTraps			1
#define disableAuthTraps	        2

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


class AGENTPP_DECL snmpInPkts: public MibLeaf {

public:
	snmpInPkts(): MibLeaf(oidSnmpInPkts, READONLY, new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutPkts: public MibLeaf {

public:
	snmpOutPkts(): MibLeaf(oidSnmpOutPkts, READONLY, new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInBadVersions: public MibLeaf {

public:
	snmpInBadVersions(): MibLeaf(oidSnmpInBadVersions, READONLY, 
			       new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInBadCommunityNames: public MibLeaf {

public:
	snmpInBadCommunityNames(): MibLeaf(oidSnmpInBadCommunityNames, 
					   READONLY, 
					   new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInBadCommunityUses: public MibLeaf {

public:
	snmpInBadCommunityUses(): MibLeaf(oidSnmpInBadCommunityUses, 
					  READONLY, 
					  new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInASNParseErrs: public MibLeaf {

public:
	snmpInASNParseErrs(): MibLeaf(oidSnmpInASNParseErrs, 
				      READONLY, 
				      new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInTooBigs: public MibLeaf {

public:
	snmpInTooBigs(): MibLeaf(oidSnmpInTooBigs, 
				 READONLY, 
				 new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInNoSuchNames: public MibLeaf {

public:
	snmpInNoSuchNames(): MibLeaf(oidSnmpInNoSuchNames, 
				     READONLY, 
				     new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInBadValues: public MibLeaf {

public:
	snmpInBadValues(): MibLeaf(oidSnmpInBadValues, 
				   READONLY, 
				   new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInReadOnlys: public MibLeaf {

public:
	snmpInReadOnlys(): MibLeaf(oidSnmpInReadOnlys, 
				   READONLY, 
				   new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInGenErrs: public MibLeaf {

public:
	snmpInGenErrs(): MibLeaf(oidSnmpInGenErrs, 
				 READONLY, 
				 new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInTotalReqVars: public MibLeaf {

public:
	snmpInTotalReqVars(): MibLeaf(oidSnmpInTotalReqVars, 
				      READONLY, 
				      new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInTotalSetVars: public MibLeaf {

public:
	snmpInTotalSetVars(): MibLeaf(oidSnmpInTotalSetVars, 
				      READONLY, 
				      new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInGetRequests: public MibLeaf {

public:
	snmpInGetRequests(): MibLeaf(oidSnmpInGetRequests, 
				     READONLY, 
				     new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInGetNexts: public MibLeaf {

public:
	snmpInGetNexts(): MibLeaf(oidSnmpInGetNexts, 
				  READONLY, 
				  new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInSetRequests: public MibLeaf {

public:
	snmpInSetRequests(): MibLeaf(oidSnmpInSetRequests, 
				     READONLY, 
				     new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInGetResponses: public MibLeaf {

public:
	snmpInGetResponses(): MibLeaf(oidSnmpInGetResponses, 
				      READONLY, 
				      new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInTraps: public MibLeaf {

public:
	snmpInTraps(): MibLeaf(oidSnmpInTraps, 
			       READONLY, 
			       new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutTooBigs: public MibLeaf {

public:
	snmpOutTooBigs(): MibLeaf(oidSnmpOutTooBigs, 
				  READONLY, 
				  new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutNoSuchNames: public MibLeaf {

public:
	snmpOutNoSuchNames(): MibLeaf(oidSnmpOutNoSuchNames, 
				      READONLY, 
				      new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutBadValues: public MibLeaf {

public:
	snmpOutBadValues(): MibLeaf(oidSnmpOutBadValues, 
				    READONLY, 
				    new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutGenErrs: public MibLeaf {

public:
	snmpOutGenErrs(): MibLeaf(oidSnmpOutGenErrs, 
				  READONLY, 
				  new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutGetRequests: public MibLeaf {

public:
	snmpOutGetRequests(): MibLeaf(oidSnmpOutGetRequests, 
				      READONLY, 
				      new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutGetNexts: public MibLeaf {

public:
	snmpOutGetNexts(): MibLeaf(oidSnmpOutGetNexts, 
				   READONLY, 
				   new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutSetRequests: public MibLeaf {

public:
	snmpOutSetRequests(): MibLeaf(oidSnmpOutSetRequests, 
				      READONLY, 
				      new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutGetResponses: public MibLeaf {

public:
	snmpOutGetResponses(): MibLeaf(oidSnmpOutGetResponses, 
				       READONLY, 
				       new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutTraps: public MibLeaf {

public:
	snmpOutTraps(): MibLeaf(oidSnmpOutTraps, 
				READONLY, 
				new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};

class AGENTPP_DECL snmpSilentDrops: public MibLeaf {

public:
	snmpSilentDrops(): MibLeaf(oidSnmpSilentDrops,
				   READONLY, 
				   new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpProxyDrops: public MibLeaf {

public:
	snmpProxyDrops(): MibLeaf(oidSnmpProxyDrops,
				  READONLY, 
				  new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual bool is_volatile() { return TRUE; }
};


/**
 *  snmpEnableAuthenTraps
 *
"Indicates whether the SNMP entity is permitted to generate
 authenticationFailure traps. The value of this object
 overrides any configuration information; as such, it
 provides a means whereby all authenticationFailure traps may
 be disabled.

 Note that it is strongly recommended that this object be
 stored in non-volatile memory so that it remains constant
 across re-initializations of the network management system."
 */


class AGENTPP_DECL snmpEnableAuthenTraps: public MibLeaf {

public:
	snmpEnableAuthenTraps();
	virtual ~snmpEnableAuthenTraps();

	static snmpEnableAuthenTraps* instance;
	long			get_state();
	virtual bool    	value_ok(const Vbx&);
};


/**********************************************************************
 * 
 *  class snmpGroup
 *
 **********************************************************************/


class AGENTPP_DECL snmpGroup: public MibGroup {

public:
	snmpGroup();
};
#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
