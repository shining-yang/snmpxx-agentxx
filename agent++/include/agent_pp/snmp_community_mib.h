/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - snmp_community_mib.h  
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


#ifndef _snmp_community_mib_h
#define _snmp_community_mib_h


#include <agent_pp/mib.h>
#include <agent_pp/snmp_textual_conventions.h>
#include <agent_pp/notification_originator.h>
#include <agent_pp/snmp_target_mib.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

#define oidSnmpCommunityTable            "1.3.6.1.6.3.18.1.1"
#define oidSnmpCommunityEntry            "1.3.6.1.6.3.18.1.1.1"
#define oidSnmpCommunityIndex            "1.3.6.1.6.3.18.1.1.1.1"
#define oidSnmpCommunityName             "1.3.6.1.6.3.18.1.1.1.2"
#define oidSnmpCommunitySecurityName     "1.3.6.1.6.3.18.1.1.1.3"
#define oidSnmpCommunityContextEngineID  "1.3.6.1.6.3.18.1.1.1.4"
#define oidSnmpCommunityContextName      "1.3.6.1.6.3.18.1.1.1.5"
#define oidSnmpCommunityTransportTag     "1.3.6.1.6.3.18.1.1.1.6"
#define oidSnmpCommunityStorageType      "1.3.6.1.6.3.18.1.1.1.7"
#define oidSnmpCommunityStatus           "1.3.6.1.6.3.18.1.1.1.8"
#define oidSnmpTargetAddrExtTable        "1.3.6.1.6.3.18.1.2"
#define oidSnmpTargetAddrExtEntry        "1.3.6.1.6.3.18.1.2.1"
#define oidSnmpTargetAddrTMask           "1.3.6.1.6.3.18.1.2.1.1"
#define oidSnmpTargetAddrMMS             "1.3.6.1.6.3.18.1.2.1.2"
#define oidSnmpTrapAddress               "1.3.6.1.6.3.18.1.3.0"
#define oidSnmpTrapCommunity             "1.3.6.1.6.3.18.1.4.0"

#ifdef _SNMPv3

/**
 *  snmpCommunityName
 *
"The community string for which a row in this table
 represents a configuration."
 */


/**
 *  snmpCommunitySecurityName
 *
"A human readable string representing the corresponding
 value of snmpCommunityName in a Security Model
 independent format."
 */


/**
 *  snmpCommunityContextEngineID
 *
"The contextEngineID indicating the location of the
 context in which management information is accessed
 when using the community string specified by the
 corresponding instance of snmpCommunityName.

 The default value is the snmpEngineID of the entity in
 which this object is instantiated."
 */


/**
 *  snmpCommunityContextName
 *
"The context in which management information is accessed
 when using the community string specified by the corresponding
 instance of snmpCommunityName."
 */



/**
 *  snmpCommunityTransportTag
 *
"This object specifies a set of transport endpoints
 from which a command responder application will accept
 management requests. If a management request containing
 this community is received on a transport endpoint other
 than the transport endpoints identified by this object,
 the request is deemed unauthentic.

 The transports identified by this object are specified
 in the snmpTargetAddrTable. Entries in that table
 whose snmpTargetAddrTagList contains this tag value
 are identified.

 If the value of this object has zero-length, transport
 endpoints are not checked when authenticating messages
 containing this community string."
 */


/**
 *  snmpCommunityStorageType
 *
"The storage type for this conceptual row in the
 snmpCommunityTable. Conceptual rows having the value
 'permanent' need not allow write-access to any
 columnar object in the row."
 */


/**
 *  snmpCommunityStatus
 *
"The status of this conceptual row in the snmpCommunityTable.

 An entry in this table is not qualified for activation
 until instances of all corresponding columns have been
 initialized, either through default values, or through
 Set operations. The snmpCommunityName and
 snmpCommunitySecurityName objects must be explicitly set.

 There is no restriction on setting columns in this table
 when the value of snmpCommunityStatus is active(1)."
 */


/**
 *  snmpTargetAddrTMask
 *
"The mask value associated with an entry in the
 snmpTargetAddrTable. The value of this object must
 have the same length as the corresponding instance of
 snmpTargetAddrTAddress, or must have length 0. An
 attempt to set it to any other value will result in
 an inconsistentValue error.

 The value of this object allows an entry in the
 snmpTargetAddrTable to specify multiple addresses.
 The mask value is used to select which bits of
 a transport address must match bits of the corresponding
 instance of snmpTargetAddrTAddress, in order for the
 transport address to match a particular entry in the
 snmpTargetAddrTable. Bits which are 1 in the mask
 value indicate bits in the transport address which
 must match bits in the snmpTargetAddrTAddress value.
 Bits which are 0 in the mask indicate bits in the
 transport address which need not match. If the
 length of the mask is 0, the mask should be treated
 as if all its bits were 1 and its length were equal
 to the length of the corresponding value of
 snmpTargetAddrTable.

 This object may not be modified while the value of the
 corresponding instance of snmpTargetAddrRowStatus is
 active(1). An attempt to set this object in this case
 will result in an inconsistentValue error."
 */
class AGENTPP_DECL snmpTargetAddrTMask: public snmpTargetAddrTAddress {

public:
	snmpTargetAddrTMask(const Oidx&);
	virtual ~snmpTargetAddrTMask();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);

	/**
	 * @note The caller has to delete the returned pointer.
	 */
	virtual NS_SNMP UdpAddress*	getUdpAddress();
};


/**
 *  snmpTargetAddrMMS
 *
"The maximum message size value associated with an entry
 in the snmpTargetAddrTable."
 */


/**
 *  snmpCommunityEntry
 *
"Information about a particular community string."
 */
class AGENTPP_DECL snmpCommunityEntry: public StorageTable {

public:
	snmpCommunityEntry();
	virtual ~snmpCommunityEntry();

	static snmpCommunityEntry* instance;

	virtual void       	set_row(MibTableRow* r, const NS_SNMP OctetStr& p0,
					const NS_SNMP OctetStr& p1,
					const NS_SNMP OctetStr& p2,
					const NS_SNMP OctetStr& p3,
					const NS_SNMP OctetStr& p4, int p5,
					int p6);
	/**
	 * Get the v3 information for a community.
	 *
	 * @param securityName
	 *    in:  the community name to lookup.
	 *    out: the corresponding securityName.
	 * @param contextEngineId
	 *    in:  ignored.
	 *    out: the contextEngineId for the given community.
	 * @param contextName
	 *    in:  ignored.
	 *    out: the contextName associated with the given community.
	 * @param transportTag
	 *    in:  ignored.
	 *    out: the transportTag associated with the given community.
	 * @return
	 *    TRUE if an entry for the given community could be found,
	 *    FALSE otherwise.
	 */
	virtual bool		get_v3_info(NS_SNMP OctetStr&, NS_SNMP OctetStr&,
					    NS_SNMP OctetStr&, NS_SNMP OctetStr&);

	/**
	 * Get the community for a given securityName/contextEngineId/
	 * contextName combination.
	 *
	 * @param securityName
	 *    a security name. On return it contains the found community.
	 * @param contextEngineId
	 *    a context engine ID.
	 * @param contextName
	 *    a context name.
	 * @return
	 *    TRUE if an appropriate community could be found,
	 *    FALSE otherwise.
	 */
	virtual bool		get_community(NS_SNMP OctetStr&,
					      const NS_SNMP OctetStr&,
					      const NS_SNMP OctetStr&);
};


/**
 *  snmpTargetAddrExtEntry
 *
"Information about a particular mask and mms value."
 */
class AGENTPP_DECL snmpTargetAddrExtEntry: public MibTable {

public:
	snmpTargetAddrExtEntry();
	virtual ~snmpTargetAddrExtEntry();

	static snmpTargetAddrExtEntry* instance;

	virtual void        	row_added(MibTableRow*, const Oidx&,
					  MibTable* s = 0);
	virtual void        	row_delete(MibTableRow*, const Oidx&,
					   MibTable* s = 0);
	virtual void       	set_row(MibTableRow* r, const NS_SNMP OctetStr& p0,
					int p1);
	virtual int        	prepare_set_request(Request*, int&);
#ifdef _SNMPv3
	virtual bool		passes_filter(const NS_SNMP OctetStr&, const NS_SNMP UTarget&);
#endif
	virtual bool		passes_filter(const NS_SNMP OctetStr&,const NS_SNMP OctetStr&);
};


class AGENTPP_DECL snmp_community_mib: public MibGroup
{
  public:
	snmp_community_mib();
	virtual ~snmp_community_mib() { }
	static void	       	add_public();
};
#endif

#ifdef AGENTPP_NAMESPACE
}
#endif

/**
 * snmp_community_mib.h generated by AgentGen 1.1.4 for AGENT++v3
 * Mon Nov 15 23:09:36 GMT+03:30 1999.
 */

#endif
