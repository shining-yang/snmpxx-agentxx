/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - snmp_notification_mib.h  
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

#ifndef _snmp_notification_mib_h_
#define _snmp_notification_mib_h_

#include <agent_pp/mib.h>
#include <agent_pp/snmp_textual_conventions.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


/**
 *  snmpNotifyType
 *
"This object determines the type of notification to
 be generated for entries in the snmpTargetAddrTable
 selected by the corresponding instance of
 snmpNotifyTag. This value is only used when
 generating notifications, and is ignored when
 using the snmpTargetAddrTable for other purposes.

 If the value of this object is trap(1), then any
 messages generated for selected rows will contain
 Unconfirmed-Class PDUs.

 If the value of this object is inform(2), then any
 messages generated for selected rows will contain
 Confirmed-Class PDUs.

 Note that if an SNMP entity only supports
 generation of Unconfirmed-Class PDUs (and not
 Confirmed-Class PDUs), then this object may be
 read-only."
 */


/**
 *  snmpNotifyRowStatus
 *
"The status of this conceptual row.

 To create a row in this table, a manager must
 set this object to either createAndGo(4) or
 createAndWait(5)."
 */


/**
 *  snmpNotifyFilterProfileName
 *
"The name of the filter profile to be used when generating
 notifications using the corresponding entry in the
 snmpTargetAddrTable."
 */



/**
 *  snmpNotifyFilterProfileRowStatus
 *
"The status of this conceptual row.

 To create a row in this table, a manager must
 set this object to either createAndGo(4) or
 createAndWait(5).

 Until instances of all corresponding columns are
 appropriately configured, the value of the
 corresponding instance of the
 snmpNotifyFilterProfileRowStatus column is 'notReady'.

 In particular, a newly created row cannot be made
 active until the corresponding instance of
 snmpNotifyFilterProfileName has been set."
 */


/**
 *  snmpNotifyFilterMask
 *
"The bit mask which, in combination with the corresponding
 instance of snmpNotifyFilterSubtree, defines a family of
 subtrees which are included in or excluded from the
 filter profile.

 Each bit of this bit mask corresponds to a
 sub-identifier of snmpNotifyFilterSubtree, with the
 most significant bit of the i-th octet of this octet
 string value (extended if necessary, see below)
 corresponding to the (8*i - 7)-th sub-identifier, and
 the least significant bit of the i-th octet of this
 octet string corresponding to the (8*i)-th
 sub-identifier, where i is in the range 1 through 16.

 Each bit of this bit mask specifies whether or not
 the corresponding sub-identifiers must match when
 determining if an OBJECT IDENTIFIER matches this
 family of filter subtrees; a '1' indicates that an
 exact match must occur; a '0' indicates 'wild card',
 i.e., any sub-identifier value matches.

 Thus, the OBJECT IDENTIFIER X of an object instance
 is contained in a family of filter subtrees if, for
 each sub-identifier of the value of
 snmpNotifyFilterSubtree, either:

 the i-th bit of snmpNotifyFilterMask is 0, or

 the i-th sub-identifier of X is equal to the i-th
 sub-identifier of the value of
 snmpNotifyFilterSubtree.

 If the value of this bit mask is M bits long and
 there are more than M sub-identifiers in the
 corresponding instance of snmpNotifyFilterSubtree,
 then the bit mask is extended with 1's to be the
 required length.

 Note that when the value of this object is the
 zero-length string, this extension rule results in
 a mask of all-1's being used (i.e., no 'wild card'),
 and the family of filter subtrees is the one
 subtree uniquely identified by the corresponding
 instance of snmpNotifyFilterSubtree."
 */


/**
 *  snmpNotifyFilterType
 *
"This object indicates whether the family of filter subtrees
 defined by this entry are included in or excluded from a
 filter. A more detailed discussion of the use of this
 object can be found in section 6. of [SNMP-APPL]."
 */


/**
 *  snmpNotifyFilterRowStatus
 *
"The status of this conceptual row.

 To create a row in this table, a manager must
 set this object to either createAndGo(4) or
 createAndWait(5)."
 */


/**
 * snmpNotifyEntry
 *
 * An entry in this table selects a set of management targets
 * which should receive notifications, as well as the type of
 * notification which should be sent to each selected
 * management target.
 *
 * Entries in the snmpNotifyTable are created and
 * deleted using the snmpNotifyRowStatus object.
 *
 * @version 3.4
 */


class AGENTPP_DECL snmpNotifyEntry: public StorageTable {

 public:
	snmpNotifyEntry();
	virtual ~snmpNotifyEntry();

	/**
	 * Add a new entry to the reciever. The new entry is
	 * used to specify target addresses to send notifications
	 * to.
	 *
	 * @param name
	 *    a unique identifier used to index this table.
	 * @param tag
	 *    a tag value used to select entries in 
	 *    snmpTargetAddrTable.
	 * @param type
	 *    specifies whether to generate a Trap PDU (1)
	 *    or an Inform-PDU (2).
	 * @return
	 *    a pointer to the newly created entry or 0 if
	 *    a row with index name already exists.
	 */
	MibTableRow* add_entry(const NS_SNMP OctetStr&,
			       const NS_SNMP OctetStr&,
			       const int);

	static snmpNotifyEntry* instance;
};


/**
 *  snmpNotifyFilterProfileEntry
 *
"An entry in this table indicates the name of the filter
 profile to be used when generating notifications using
 the corresponding entry in the snmpTargetParamsTable.

 Entries in the snmpNotifyFilterProfileTable are created
 and deleted using the snmpNotifyFilterProfileRowStatus
 object."
 */


class AGENTPP_DECL snmpNotifyFilterProfileEntry: public StorageTable {

 public:
	snmpNotifyFilterProfileEntry();
	virtual ~snmpNotifyFilterProfileEntry();

	static snmpNotifyFilterProfileEntry* instance;
};


/**
 *  snmpNotifyFilterEntry
 *
"An element of a filter profile.

 Entries in the snmpNotifyFilterTable are created and
 deleted using the snmpNotifyFilterRowStatus object."
 */


class AGENTPP_DECL snmpNotifyFilterEntry: public StorageTable {

 public:
	snmpNotifyFilterEntry();
	virtual ~snmpNotifyFilterEntry();

	static snmpNotifyFilterEntry* instance;

	/**
	 * Checks whether an object identifier passes the notification
	 * filter specified by the given profile name (a pointer into
	 * the snmpNotifyFilterTable).
	 *
	 * @param target
	 *    an object identifier representing an index value of the
	 *    snmpTargetParamsTable. 
	 * @param oid
	 *    an object identifier to check. 
	 * @param vbs
	 *    an array of Vbx instances whose OIDs should be checked.
	 * @param vb_count
	 *    the number of the above variable bindings.
	 * @return 
	 *    TRUE if the object id passes the filter or if no appropiate
	 *    filter exists, in any other case return FALSE.
	 */
	static bool passes_filter(const Oidx&, 
				     const Oidx&,
				     const Vbx*,
				     unsigned int);
};


class AGENTPP_DECL snmp_notification_mib: public MibGroup
{
  public:
	snmp_notification_mib();
	virtual ~snmp_notification_mib() { }
};

#ifdef AGENTPP_NAMESPACE
}
#endif


#endif


