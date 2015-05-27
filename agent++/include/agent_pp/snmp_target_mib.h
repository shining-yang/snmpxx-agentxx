/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - snmp_target_mib.h  
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


#ifndef _snmp_target_mib_h_
#define _snmp_target_mib_h_

#include <agent_pp/mib.h>
#include <agent_pp/snmp_textual_conventions.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


/**
 *  snmpTargetAddrTDomain
 *
"This object indicates the transport type of the address
 contained in the snmpTargetAddrTAddress object."
 */


class AGENTPP_DECL snmpTargetAddrTDomain: public MibLeaf {

 public:
	snmpTargetAddrTDomain(const Oidx&);
	virtual ~snmpTargetAddrTDomain();

	virtual MibEntryPtr	clone();
	virtual bool    	value_ok(const Vbx&);
	virtual int		get_state();
};


/**
 *  snmpTargetAddrTAddress
 *
"This object contains a transport address. The format of
 this address depends on the value of the
 snmpTargetAddrTDomain object."
 */

class AGENTPP_DECL snmpTargetAddrTAddress: public MibLeaf {

 public:
	snmpTargetAddrTAddress(const Oidx&);
	snmpTargetAddrTAddress(const Oidx&, mib_access, NS_SNMP OctetStr*, int);
	virtual ~snmpTargetAddrTAddress();

	virtual MibEntryPtr	clone();
	virtual int	   	prepare_set_request(Request*, int&);

	/**
	 * @note The caller has to delete the returned pointer.
	 */
	virtual NS_SNMP UdpAddress*	getUdpAddress();
};


/**
 *  snmpTargetAddrTimeout
 *
"This object should reflect the expected maximum round
 trip time for communicating with the transport address
 defined by this row. When a message is sent to this
 address, and a response (if one is expected) is not
 received within this time period, an implementation
 may assume that the response will not be delivered.

 Note that the time interval that an application waits
 for a response may actually be derived from the value
 of this object. The method for deriving the actual time
 interval is implementation dependent. One such method
 is to derive the expected round trip time based on a
 particular retransmission algorithm and on the number
 of timeouts which have occurred. The type of message may
 also be considered when deriving expected round trip
 times for retransmissions. For example, if a message is
 being sent with a securityLevel that indicates both
 authentication and privacy, the derived value may be
 increased to compensate for extra processing time spent
 during authentication and encryption processing."
 */



/**
 *  snmpTargetAddrRetryCount
 *
"This object specifies a default number of retries to be
 attempted when a response is not received for a generated
 message. An application may provide its own retry count,
 in which case the value of this object is ignored."
 */



/**
 *  snmpTargetAddrParams
 *
"The value of this object identifies an entry in the
 snmpTargetParamsTable. The identified entry
 contains SNMP parameters to be used when generating
 messages to be sent to this transport address."
 */


class AGENTPP_DECL snmpTargetAddrParams: public MibLeaf {

 public:
	snmpTargetAddrParams(const Oidx&);
	virtual ~snmpTargetAddrParams();

	virtual MibEntryPtr	clone();
	virtual int		prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  snmpTargetAddrRowStatus
 *
"The status of this conceptual row.

 To create a row in this table, a manager must
 set this object to either createAndGo(4) or
 createAndWait(5).

 Until instances of all corresponding columns are
 appropriately configured, the value of the
 corresponding instance of the snmpTargetAddrRowStatus
 column is 'notReady'.

 In particular, a newly created row cannot be made
 active until the corresponding instances of
 snmpTargetAddrTDomain, snmpTargetAddrTAddress, and
 snmpTargetAddrParams have all been set.

 The following objects may not be modified while the
 value of this object is active(1):
 - snmpTargetAddrTDomain
 - snmpTargetAddrTAddress
 An attempt to set these objects while the value of
 snmpTargetAddrRowStatus is active(1) will result in
 an inconsistentValue error."
 */


/**
 *  snmpTargetParamsMPModel
 *
"The Message Processing Model to be used when generating
 SNMP messages using this entry."
 */


/**
 *  snmpTargetParamsSecurityModel
 *
"The Security Model to be used when generating SNMP
 messages using this entry. An implementation may
 choose to return an inconsistentValue error if an
 attempt is made to set this variable to a value
 for a security model which the implementation does
 not support."
 */


/**
 *  snmpTargetParamsSecurityName
 *
"The securityName which identifies the Principal on
 whose behalf SNMP messages will be generated using
 this entry."
 */



/**
 *  snmpTargetParamsSecurityLevel
 *
"The Level of Security to be used when generating
 SNMP messages using this entry."
 */

/**
 *  snmpTargetParamsRowStatus
 *
"The status of this conceptual row.

 To create a row in this table, a manager must
 set this object to either createAndGo(4) or
 createAndWait(5).

 Until instances of all corresponding columns are
 appropriately configured, the value of the
 corresponding instance of the snmpTargetParamsRowStatus
 column is 'notReady'.

 In particular, a newly created row cannot be made
 active until the corresponding
 snmpTargetParamsMPModel,
 snmpTargetParamsSecurityModel,
 snmpTargetParamsSecurityName,
 and snmpTargetParamsSecurityLevel have all been set.
 The following objects may not be modified while the
 value of this object is active(1):
 - snmpTargetParamsMPModel
 - snmpTargetParamsSecurityModel
 - snmpTargetParamsSecurityName
 - snmpTargetParamsSecurityLevel
 An attempt to set these objects while the value of
 snmpTargetParamsRowStatus is active(1) will result in
 an inconsistentValue error."
 */

/**
 *  snmpTargetParamsEntry
 *
"A set of SNMP target information.

 Entries in the snmpTargetParamsTable are created and
 deleted using the snmpTargetParamsRowStatus object."
 */


class AGENTPP_DECL snmpTargetParamsEntry: public StorageTable {

 public:
	snmpTargetParamsEntry();
	virtual ~snmpTargetParamsEntry();

	static snmpTargetParamsEntry* instance;

	virtual bool		contains(const NS_SNMP OctetStr&);

#ifdef _SNMPv3
	/**
	 * Get the target params values from the given target
	 * parameters entry.
	 *
	 * @param entry
	 *    a snmpTargetParamsName instance.
	 * @param target
	 *    a UTarget instance where target related
	 *    parameters will be stored in.
	 * @param secLevel
	 *    returns the security level of the entry.
	 * @return
	 *    TRUE if the entry has been found, FALSE otherwise.
	 */
	virtual bool		get_target_params(const NS_SNMP OctetStr&,
						  NS_SNMP UTarget&, int&);
#endif
	/**
	 * Add a new SNMP parameter set to the receiver.
	 *
	 * @param name
	 *    a unique identifier used to index this table. It is also
	 *    used to connect this table to the snmpTargetAddrTable,
	 *    snmpNotifyFilterProfileTable, and the snmpProxyTable.
	 * @param mpModel
	 *    the message processing model to use when generating an
	 *    SNMP message. For example, a value of 3 specifies SNMPv3's
	 *    Message Processing Model should be used.
	 * @param securityModel
	 *    the Security Model to be use when generating SNMP messages.
	 *    For example, a value of 3 specifies the USM.
	 * @param securityName
	 *    identifies the Principal on whose behalf SNMP messages
	 *    will be generated.
	 * @param securityLevel
	 *    identifies the Security Level to be used when generating
	 *    SNMP messages. For example, a value of 1 will specify
	 *    no authentication with no privacy.
	 * @return
	 *    the newly created row or 0 if an entry with name already
	 *    exists.
	 */
	MibTableRow*	add_entry(const NS_SNMP OctetStr&,
				  const int, const int,
				  const NS_SNMP OctetStr&,
				  const int);
};



/**
 * snmpTargetAddrEntry
 *
 * A transport address to be used in the generation
 * of SNMP operations.
 *
 * Entries in the snmpTargetAddrTable are created and
 * deleted using the snmpTargetAddrRowStatus object."
 *
 * @version 3.5.16
 */


class AGENTPP_DECL snmpTargetAddrEntry: public StorageTable {

 public:
	snmpTargetAddrEntry();
	virtual ~snmpTargetAddrEntry();

	static snmpTargetAddrEntry* instance;

	virtual bool		refers_to(NS_SNMP OctetStr&);
	/**
	 * Return the address stored in the given row of
	 * the receiver.
	 * @note Don't use other rows! This method is not
	 * synchronized!
	 * @note The caller has to delete the returned pointer.
	 *
	 * @param row
	 *    a row of snmpTargetAddrEntry.
	 * @return
	 *    a pointer to an Address instance or 0 if
	 *    the domain of row is not supported.
	 */
	virtual NS_SNMP Address*	get_address(MibTableRow*);
#ifdef _SNMPv3
	/**
	 * Return the Target represented by the given address
	 * name.
	 * @note The caller has to delete the returned pointer.
	 *
	 * @param targetAddrName
	 *    a SnmpTargetAddrName.
	 * @param paramsInfo
	 *    the snmpTargetParamsEntry table to be used for
	 *    target parameters lookup.
	 * @param secLevel
	 *    returns the security_level associated with targetAddrName
	 * @return
	 *    a pointer to an UTarget instance or 0 if the
	 *    address name is not valid.
	 */
	virtual NS_SNMP UTarget*	get_target(const NS_SNMP OctetStr&,
					   snmpTargetParamsEntry*, int&);
#endif
	/**
	 * Return the rows that match a given tag. The rows are cloned
	 * and the method is synchronized on the receiver.
	 * @note The caller has to delete the returned pointer.
	 *
	 * @param tag
	 *    an OctetStr instance.
	 * @return
	 *    a (possibly empty) list of rows.
	 */
	List<MibTableRow>*	get_rows_cloned_for_tag(const NS_SNMP OctetStr&);

	void			set_row(MibTableRow* r,
					const Oidx&,
					const NS_SNMP OctetStr&,
					int, int,
					const NS_SNMP OctetStr&,
					const NS_SNMP OctetStr&, int, int);

	/**
	 * Add a new target entry to the receiver.
	 *
	 * @param name
	 *    a unique identifier used to identify the new entry.
	 * @param tdomain
	 *    specifies the transport type of the address defined by
	 *    taddress.
	 * @param taddress
	 *    specifies the target address. The format of this value
	 *    depends on the tdomain value. For example, if tdomain
	 *    specifies UDP (1.3.6.1.6.1.1), then this value will be
	 *    a 6-byte OctetStr, where the first 4 bytes specify the
	 *    IP address, and last 2 bytes specify the UDP port number.
	 * @param taglist
	 *    a list of tag values that tie this table to both the
	 *    snmpNotifyTable and the snmpProxyTable. These tags are
	 *    used to identify target addresses to send notifications
	 *    to and forward messages to.
	 * @param params
	 *    identifies a row in the snmpTargetParamsTable. This in
	 *    effect specifies the SNMP parameters to use when
	 *    generating messages.
	 * @return
	 *    the created row or 0 if a row with name already exists.
	 */
	MibTableRow*	       	add_entry(const NS_SNMP OctetStr&,
					  const Oidx&,
					  const NS_SNMP OctetStr&,
					  const NS_SNMP OctetStr&,
					  const NS_SNMP OctetStr&);

	/**
	 * Check if row can be set active.
	 */
	bool			ready_for_service(Vbx*, int);
};



class AGENTPP_DECL snmp_target_mib: public MibGroup
{
  public:
	snmp_target_mib();
	virtual ~snmp_target_mib() { }
};

#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
