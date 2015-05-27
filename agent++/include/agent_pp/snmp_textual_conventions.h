/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - snmp_textual_conventions.h  
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


#ifndef _snmp_textual_convention_h_
#define _snmp_textual_convention_h_

#include <agent_pp/agent++.h>
#include <agent_pp/mib.h>

#define oidSnmpSetSerialNo "1.3.6.1.6.3.1.1.6.1.0"

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


const index_info iSnmpAdminString[1] = 
{ { sNMP_SYNTAX_OCTETS, TRUE, 1, 32 } };


/*---------------------- class snmpDisplayString ---------------------*/

/**
 * The class snmpDisplayString implements the textual convention 
 * DisplayString. It may also be used to implement the SnmpAdminString
 * textual convention.
 *
 * @author Frank Fock
 * @version 3.2
 */

class AGENTPP_DECL SnmpDisplayString: public MibLeaf {

 public:
	/**
	 * Construct a DisplayString with initial value.
	 * 
	 * @param oid
	 *    the oid of the leaf or the column of the columnar object.
	 * @param access
	 *    the max-access rights for the object.
	 * @param octetstr
	 *    a pointer to an OctetStr containing the initial value for
	 *    the object.
	 */
	SnmpDisplayString(const Oidx&, mib_access, NS_SNMP OctetStr*);
	/**
	 * Construct a DisplayString with initial value.
	 * 
	 * @param oid
	 *    the oid of the leaf or the column of the columnar object.
	 * @param access
	 *    the max-access rights for the object.
	 * @param octetstr
	 *    a pointer to an OctetStr containing the initial value for
	 *    the object.
	 * @param hasdefault
	 *    if TRUE treat the inital value as default value (this 
	 *    applies to columnar objects only)
	 */
	SnmpDisplayString(const Oidx&, mib_access, NS_SNMP OctetStr*, bool);
	/**
	 * Construct a DisplayString with initial value.
	 * 
	 * @param oid
	 *    the oid of the leaf or the column of the columnar object.
	 * @param access
	 *    the max-access rights for the object.
	 * @param octetstr
	 *    a pointer to an OctetStr containing the initial value for
	 *    the object.
	 * @param hasdefault
	 *    if TRUE treat the inital value as default value (this 
	 *    applies to columnar objects only)
	 * @param min_size
	 *    the minimum size of the DisplayString.
	 * @param max_size
	 *    the maxmimum size of the DisplayString.
	 */
	SnmpDisplayString(const Oidx&, mib_access, NS_SNMP OctetStr*, bool, 
			  int, int);
	virtual ~SnmpDisplayString();

	virtual MibEntryPtr	clone();
	virtual bool    	value_ok(const Vbx&);
	virtual int		prepare_set_request(Request*, int&);
 protected:
	unsigned int max_size;
	unsigned int min_size;
};


/*---------------------- class snmpEngineID ---------------------*/

/**
 * The class snmpEngineID implements the textual convention 
 * SnmpEngineID. Description from SNMP-FRAMEWORK-MIB:
 *
 * An SNMP engine's administratively-unique identifier.
 * Objects of this type are for identification, not for
 * addressing, even though it is possible that an
 * address may have been used in the generation of
 * a specific value.
 *
 * The value for this object may not be all zeros or
 * all 'ff'H or the empty (zero length) string.
 *
 * The initial value for this object may be configured
 * via an operator console entry or via an algorithmic
 * function.  In the latter case, the following
 * example algorithm is recommended.
 *
 * In cases where there are multiple engines on the
 * same system, the use of this algorithm is NOT
 * appropriate, as it would result in all of those
 * engines ending up with the same ID value.
 *
 * 1) The very first bit is used to indicate how the
 *    rest of the data is composed.
 *
 *    0 - as defined by enterprise using former methods
 *        that existed before SNMPv3. See item 2 below.
 *
 *    1 - as defined by this architecture, see item 3
 *        below.
 *
 *    Note that this allows existing uses of the
 *    engineID (also known as AgentID [RFC1910]) to
 *    co-exist with any new uses.
 *
 * 2) The snmpEngineID has a length of 12 octets.
 *
 *    The first four octets are set to the binary
 *    equivalent of the agent's SNMP management
 *    private enterprise number as assigned by the
 *    Internet Assigned Numbers Authority (IANA).
 *    For example, if Acme Networks has been assigned
 *    { enterprises 696 }, the first four octets would
 *    be assigned '000002b8'H.
 *
 *    The remaining eight octets are determined via
 *    one or more enterprise-specific methods. Such
 *    methods must be designed so as to maximize the
 *    possibility that the value of this object will
 *    be unique in the agent's administrative domain.
 *    For example, it may be the IP address of the SNMP
 *    entity, or the MAC address of one of the
 *    interfaces, with each address suitably padded
 *    with random octets.  If multiple methods are
 *    defined, then it is recommended that the first
 *    octet indicate the method being used and the
 *    remaining octets be a function of the method.
 *
 * 3) The length of the octet strings varies.
 *
 *    The first four octets are set to the binary
 *    equivalent of the agent's SNMP management
 *    private enterprise number as assigned by the
 *    Internet Assigned Numbers Authority (IANA).
 *    For example, if Acme Networks has been assigned
 *    { enterprises 696 }, the first four octets would
 *    be assigned '000002b8'H.
 *
 *    The very first bit is set to 1. For example, the
 *    above value for Acme Networks now changes to be
 *    '800002b8'H.
 *
 *    The fifth octet indicates how the rest (6th and
 *    following octets) are formatted. The values for
 *    the fifth octet are:
 *
 *      0     - reserved, unused.
 *
 *      1     - IPv4 address (4 octets)
 *              lowest non-special IP address
 *
 *      2     - IPv6 address (16 octets)
 *              lowest non-special IP address
 *
 *      3     - MAC address (6 octets)
 *              lowest IEEE MAC address, canonical
 *              order
 *
 *      4     - Text, administratively assigned
 *              Maximum remaining length 27
 *
 *      5     - Octets, administratively assigned
 *              Maximum remaining length 27
 *
 *      6-127 - reserved, unused
 *
 *    127-255 - as defined by the enterprise
 *              Maximum remaining length 27
 *
 * @author Frank Fock
 * @version 3.3
 */

class AGENTPP_DECL SnmpEngineID: public MibLeaf {

 public:
	/**
	 * Construct a SnmpEngineID with initial value.
	 * 
	 * @param oid
	 *    the oid of the leaf or the column of the columnar object.
	 * @param access
	 *    the max-access rights for the object.
	 * @param octetstr
	 *    a pointer to an OctetStr containing the initial value for
	 *    the object.
	 */
	SnmpEngineID(const Oidx&, mib_access, NS_SNMP OctetStr*);

	/**
	 * Construct a SnmpEngineID with initial value.
	 * 
	 * @param oid
	 *    the oid of the leaf or the column of the columnar object.
	 * @param access
	 *    the max-access rights for the object.
	 * @param octetstr
	 *    a pointer to an OctetStr containing the initial value for
	 *    the object.
	 * @param valueMode
	 *    contains an ored value of one or more of the following modes:
	 *    VMODE_DEFAULT: the value of the object pointed by value will 
	 *                   be used as default value, otherwise the managed 
	 *                   object has no default value.
	 *    VMODE_LOCKED:  value is locked (cannot be modified) while
	 *                   the row status of the receiver's row is active(1).
	 *                   This mode has no effect, if the the leaf is a
	 *                   scalar or otherwise if the receiver's row has no
	 *                   snmpRowStatus.  
	 */
        SnmpEngineID(const Oidx&, mib_access, NS_SNMP OctetStr*, int);

	/**
	 * Destructor
	 */
	virtual ~SnmpEngineID();

	virtual MibEntryPtr	clone();
	virtual int		prepare_set_request(Request*, int&);

	/**
	 * Create a default SnmpEngineID of the form
	 * <AGENT++ enterpriseID>+<userText>
	 *
	 * @param userText
	 *    an OcetStr
	 *
	 * @return 
	 *    a SnmpEngineID OctetStr.
	 */
	static NS_SNMP OctetStr create_engine_id(const NS_SNMP OctetStr& userText);

	/**
	 * Create a default SnmpEngineID of the form
	 * <AGENT++ enterpriseID>+<hostname>+<port>
	 *
	 * @param port
	 *    the UDP port the SNMP engine listens on.
	 *
	 * @return 
	 *    a SnmpEngineID OctetStr.
	 */
	static NS_SNMP OctetStr create_engine_id(unsigned short port);

};


/*------------------------ class SnmpTagValue -----------------------*/

/**
 * SnmpTagValue
 *
 * @author Frank Fock
 * @version 3.3
 */


class AGENTPP_DECL SnmpTagValue: public MibLeaf {

 public:
	/**
	 * Construct a SnmpTagValue with initial value.
	 * 
	 * @param oid
	 *    the oid of the leaf or the column of the columnar object.
	 * @param access
	 *    the max-access rights for the object.
	 * @param octetstr
	 *    a pointer to an OctetStr containing the initial value for
	 *    the object.
	 * @param valueMode
	 *    contains an ored value of one or more of the following modes:
	 *    VMODE_DEFAULT: the value of the object pointed by value will 
	 *                   be used as default value, otherwise the managed 
	 *                   object has no default value.
	 *    VMODE_LOCKED:  value is locked (cannot be modified) while
	 *                   the row status of the receiver's row is active(1).
	 *                   This mode has no effect, if the the leaf is a
	 *                   scalar or otherwise if the receiver's row has no
	 *                   snmpRowStatus.  
	 */
	SnmpTagValue(const Oidx&, mib_access, NS_SNMP OctetStr*, int);

	SnmpTagValue(const Oidx&);
	virtual ~SnmpTagValue();

	virtual MibEntryPtr	clone();
	virtual bool    	value_ok(const Vbx&);
	virtual int		prepare_set_request(Request*, int&);
	static bool		is_delimiter(char); 
};


/*---------------------- class snmpTargetAddrTagList ---------------------*/

/**
 * SnmpTagList
 *
 * This object contains a list of tag values.
 *
 * @author Frank Fock
 * @version 3.3
 */


class AGENTPP_DECL SnmpTagList: public MibLeaf {

 public:
	/**
	 * Construct a SnmpTagList with initial value.
	 * 
	 * @param oid
	 *    the oid of the leaf or the column of the columnar object.
	 * @param access
	 *    the max-access rights for the object.
	 * @param octetstr
	 *    a pointer to an OctetStr containing the initial value for
	 *    the object.
	 * @param valueMode
	 *    contains an ored value of one or more of the following modes:
	 *    VMODE_DEFAULT: the value of the object pointed by value will 
	 *                   be used as default value, otherwise the managed 
	 *                   object has no default value.
	 *    VMODE_LOCKED:  value is locked (cannot be modified) while
	 *                   the row status of the receiver's row is active(1).
	 *                   This mode has no effect, if the the leaf is a
	 *                   scalar or otherwise if the receiver's row has no
	 *                   snmpRowStatus.  
	 */
	SnmpTagList(const Oidx&, mib_access, NS_SNMP OctetStr*, int);

	SnmpTagList(const Oidx&);
	virtual ~SnmpTagList();

	virtual MibEntryPtr	clone();
	virtual bool    	value_ok(const Vbx&);
	virtual bool		contains(const char*); 
};


/*--------------------------- class TestAndIncr -------------------------*/

/**
 *  TestAndIncr
 *
 * Represents integer-valued information used for atomic
 * operations.  When the management protocol is used to specify
 * that an object instance having this syntax is to be
 * modified, the new value supplied via the management protocol
 * must precisely match the value presently held by the
 * instance.  If not, the management protocol set operation
 * fails with an error of `inconsistentValue'.  Otherwise, if
 * the current value is the maximum value of 2^31-1 (2147483647
 * decimal), then the value held by the instance is wrapped to
 * zero; otherwise, the value held by the instance is
 * incremented by one.  (Note that regardless of whether the
 * management protocol set operation succeeds, the variable-
 * binding in the request and response PDUs are identical.)
 *
 * The value of the ACCESS clause for objects having this
 * syntax is either `read-write' or `read-create'.  When an
 * instance of a columnar object having this syntax is created,
 * any value may be supplied via the management protocol.
 *
 * When the network management portion of the system is re-
 * initialized, the value of every object instance having this
 * syntax must either be incremented from its value prior to
 * the re-initialization, or (if the value prior to the re-
 * initialization is unknown) be set to a pseudo-randomly
 * generated value."
 *
 * This object for example 
 * is used to facilitate modification of table entries in 
 * the SNMP-TARGET-MIB module by multiple managers. In 
 * particular, it is useful when modifying the value of the 
 * snmpTargetAddrTagList object.
 *
 * The procedure for modifying the snmpTargetAddrTagList
 * object is as follows:
 *
 * 1. Retrieve the value of snmpTargetSpinLock and
 * of snmpTargetAddrTagList.
 *
 * 2. Generate a new value for snmpTargetAddrTagList.
 *
 * 3. Set the value of snmpTargetSpinLock to the
 * retrieved value, and the value of
 * snmpTargetAddrTagList to the new value. If
 * the set fails for the snmpTargetSpinLock
 * object, go back to step 1."
 */

class AGENTPP_DECL TestAndIncr: public MibLeaf {

 public:
	TestAndIncr(const Oidx&);
	virtual ~TestAndIncr();

	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual bool    	value_ok(const Vbx&);
	virtual int		prepare_set_request(Request*, int&);

	virtual bool		is_volatile() { return TRUE; }
};


/*--------------------------- class StorageType -------------------------*/

/**
 * The StorageType class describes the memory realization of a 
 * conceptual row.  A row which is volatile(2) is lost upon reboot.  
 * A row which is either nonVolatile(3), permanent(4) or readOnly(5), 
 * is backed up by stable storage.  A row which is permanent(4)
 * can be changed but not deleted.  A row which is readOnly(5)
 * cannot be changed nor deleted.
 *
 * If the value of an object with this syntax is either
 * permanent(4) or readOnly(5), it cannot be modified.
 * Conversely, if the value is either other(1), volatile(2) or
 * nonVolatile(3), it cannot be modified to be permanent(4) or
 * readOnly(5).
 * 
 * Every usage of this textual convention is required to
 * specify the columnar objects which a permanent(4) row must
 * at a minimum allow to be writable.
 *
 * @version 3.5.16
 */

class AGENTPP_DECL StorageType: public MibLeaf {

public:
	/**
	 * Construct a storageType instance.
	 * 
	 * @param o
	 *   The storageType column subidentifier.
	 * @param default
	 *   A default storageType value, should be nonVolatile(3)
	 */
	StorageType(const Oidx&, int);

	/**
	 * Return whether the row of the receiver is volatile or permanent.
	 *
	 * @return TRUE, if the row is volatile, FALSE otherwise.
	 */
	virtual bool	row_is_volatile();

	/**
	 * Return the receiver's value.
	 *
	 * @return A integer value between 1 and 5.
	 */
	virtual long get_state();

	/**
	 * Sets the type of this storage type. Allowed values are 
	 * in range 1..5.
	 * @param storageType
	 *    a long integer value between 1 and 5.
	 */
	virtual void set_state(long); 

	virtual MibEntryPtr	clone();
	virtual bool    	value_ok(const Vbx&);
};


/*--------------------------- class StorageTable ------------------------*/

/**
 * The StorageTable class represents tables that have a StorageType
 * row. It provides the functionality to store the table's content
 * permanently (on disk) and retrieve it again when the table is
 * instanciated again.
 *
 * @author Frank Fock
 * @version 3.5.22
 */ 

class AGENTPP_DECL StorageTable: public MibTable {
  friend class Mib;		// needs access to find_next() and get()
  friend class snmpRowStatus;	// needs access to _ready_for_service()
public:

	/** 
	 * Copy constructor.
	 */
	StorageTable(const StorageTable&);

	/**
	 * Constructor with object identifier with single index object
	 * like string or oid with implied length.
	 *
	 * @param o 
	 *    the object identifier of the table, which has to be
	 *    the oid of the the SMI table entry object (table.1).
	 */
	StorageTable(const Oidx&);

	/**
	 * Constructor with object identifier and single index object's
	 * index length (meassured in subidentifiers).
	 *
	 * @param o 
	 *    the object identifier of the table, which has to be
	 *    the oid of the the SMI table entry object (table.1).
	 * @param ilen
	 *    the length of the index meassured in subidentifiers.
	 *    0 represents a variable length string or oid index object.
	 *    -1 represents a variable length string or oid index object
	 *    with implied length. 
	 */
	StorageTable(const Oidx&, unsigned int);

	/**
	 * Constructor with object identifier and given fixed index length.
	 * Additionally the automatic index object initialization can be
	 * activated.  
	 *
	 * @param o - The object identifier of the table, which has to be
	 *            the oid of the the SMI table entry object (table.1).
	 * @param ilen - The length of the index meassured in subidentifiers.
	 * @param a - If TRUE the automatic index object initialization is
	 *            activated.
	 */
	StorageTable(const Oidx&, unsigned int, bool);

	/**
	 * Constructor with object identifier and given index structure.
	 * In AGENT++ the structure of a table's index is given by an array
	 * of integer values. Each element represents an object of the index.
	 * A value of -1 represents a implied variable length string or oid.
	 * A value of 0 a variable length string or oid index object.
	 * A value greater than 0 determines the length of the index object 
	 * meassured in subidentifiers. 
	 * 
	 * @param o
	 *    the object identifier of the table, which has to be
	 *    the oid of the the SMI table entry object (table.1).
	 * @param index
	 *    an integer array. The length of the array corresponds to
	 *    the number of objects in the INDEX clause.
	 * @param indexLength
	 *    number of elements in the index array.
	 */
	StorageTable(const Oidx&, const index_info*, unsigned int);

	/**
	 * Destructor.
	 */
	virtual		~StorageTable();

	/**
	 * Add the StorageType column to the table.
	 * 
	 * @param col - A pointer to a storageType object which should be  
	 *              added to the columnar objects of the receiver.
	 * @note A table can only have one storageType columnar object.   
	 */ 
	virtual void	add_storage_col(StorageType*);

	/**
	 * Set storage type of the specified row.
	 * 
	 * @param row
	 *    a pointer to a MibTableRow instance of this table.
	 * @param storageType
	 *    a storage type value (1-5).
	 */
	void		set_storage_type(MibTableRow*, int); 

	/**
	 * Removes all rows that are not permanent and not read-only.
	 */
	virtual void	reset();

 protected:
	/**
	 * Check whether the given row should be serialized or not.
	 * Return TRUE if the given row has a volatile class StorageType. 
	 *
	 * @param row
	 *    a MibTableRow instance.
	 * @return
	 *    TRUE if the row should be stored persistently, FALSE otherwise
	 */
	virtual bool		is_persistent(MibTableRow*);

private:
	int	storage_type;
};
	
/*---------------------- class SnmpInt32MinMax ---------------------*/

/**
 * SnmpInt32MinMax
 *
 * This object contains a integer value. The range of possible values
 * can be set.
 *
 * @author Jochen Katz
 * @version 3.3
 */

class AGENTPP_DECL SnmpInt32MinMax : public MibLeaf
{
 public:

	/**
	 * Construct a SnmpInt32MinMax with initial value.
	 * 
	 * @param oid
	 *    the oid of the leaf or the column of the columnar object.
	 * @param access
	 *    the max-access rights for the object.
	 * @param intvalue
	 *    an integer value.
	 * @param valueMode
	 *    contains an ored value of one or more of the following modes:
	 *    VMODE_DEFAULT: the value of the object pointed by value will 
	 *                   be used as default value, otherwise the managed 
	 *                   object has no default value.
	 *    VMODE_LOCKED:  value is locked (cannot be modified) while
	 *                   the row status of the receiver's row is active(1).
	 *                   This mode has no effect, if the the leaf is a
	 *                   scalar or otherwise if the receiver's row has no
	 *                   snmpRowStatus.  
	 * @param min
	 *    the minimum value the receiver may be set to.
	 * @param max
	 *    the maximum value the receiver may be set to.
	 */
	SnmpInt32MinMax(const Oidx&, mib_access, const int, int, int, int);
	/**
	 * Construct a SnmpIn32MinMax without initial value (will be 0).
	 * 
	 * @param oid
	 *    the oid of the leaf or the column of the columnar object.
	 * @param access
	 *    the max-access rights for the object.
	 * @param min
	 *    the minimum value the receiver may be set to.
	 * @param max
	 *    the maximum value the receiver may be set to.
	 */
	SnmpInt32MinMax(const Oidx&, mib_access,                 int, int);
	
	virtual bool value_ok(const Vbx&);
	MibEntryPtr     clone();

	int		get_state();
	void		set_state(int);
 private:
	int min, max;
};

/*---------------------- class OctetStrMinMax ---------------------*/

/**
 * OctetStrMinMax
 *
 * This object contains a integer value. The range of possible values
 * can be set.
 *
 * @author Jochen Katz
 * @version 3.3
 */

class AGENTPP_DECL OctetStrMinMax : public MibLeaf
{
 public:

	/**
	 * Construct a OctetStrMinMax with initial value.
	 * 
	 * @param oid
	 *    the oid of the leaf or the column of the columnar object.
	 * @param access
	 *    the max-access rights for the object.
	 * @param string
	 *    a pointer to an OctetStr instance.
	 * @param valueMode
	 *    contains an ored value of one or more of the following modes:
	 *    VMODE_DEFAULT: the value of the object pointed by value will 
	 *                   be used as default value, otherwise the managed 
	 *                   object has no default value.
	 *    VMODE_LOCKED:  value is locked (cannot be modified) while
	 *                   the row status of the receiver's row is active(1).
	 *                   This mode has no effect, if the the leaf is a
	 *                   scalar or otherwise if the receiver's row has no
	 *                   snmpRowStatus.  
	 * @param min
	 *    the minimum length of the receiver's value.
	 * @param max
	 *    the maximum length of the receiver's value.
	 */
	OctetStrMinMax(const Oidx&, mib_access, NS_SNMP OctetStr*, int, u_int, u_int);
	/**
	 * Construct a SnmpIn32MinMax without initial value (will be 0).
	 * 
	 * @param oid
	 *    the oid of the leaf or the column of the columnar object.
	 * @param access
	 *    the max-access rights for the object.
	 * @param min
	 *    the minimum length of the receiver's value.
	 * @param max
	 *    the maximum length of the receiver's value.
	 */
	OctetStrMinMax(const Oidx&, mib_access,                 u_int, u_int);
	
	virtual int		prepare_set_request(Request*, int&);
	MibEntryPtr     clone();

	/**
	 * Return the integer value of the receiver.
	 *
	 * @return 
	 *    an int.
	 */
	int		get();
	
 protected:
	unsigned int min, max;
};

/*------------------------ class SnmpSecurityModel -----------------------*/

/**
 * SnmpSecurityModel
 *
 * An identifier that uniquely identifies a
 * securityModel of the Security Subsystem within the
 * SNMP Management Architecture.
 *
 * The values for securityModel are allocated as
 * follows:
 *
 * - The zero value is reserved.
 * - Values between 1 and 255, inclusive, are reserved
 *   for standards-track Security Models and are
 *   managed by the Internet Assigned Numbers Authority
 *   (IANA).
 * - Values greater than 255 are allocated to
 *   enterprise-specific Security Models.  An
 *   enterprise-specific securityModel value is defined
 *   to be:
 *
 *   enterpriseID * 256 + security model within
 *   enterprise
 *
 *   For example, the fourth Security Model defined by
 *   the enterprise whose enterpriseID is 1 would be
 *   260.
 *
 * This scheme for allocation of securityModel
 * values allows for a maximum of 255 standards-
 * based Security Models, and for a maximum of
 * 255 Security Models per enterprise.
 *
 * It is believed that the assignment of new
 * securityModel values will be rare in practice
 * because the larger the number of simultaneously
 * utilized Security Models, the larger the
 * chance that interoperability will suffer.
 * Consequently, it is believed that such a range
 * will be sufficient.  In the unlikely event that
 * the standards committee finds this number to be
 * insufficient over time, an enterprise number
 * can be allocated to obtain an additional 255
 * possible values.
 *
 * Note that the most significant bit must be zero;
 * hence, there are 23 bits allocated for various
 * organizations to design and define non-standard
 * securityModels.  This limits the ability to
 * define new proprietary implementations of Security
 * Models to the first 8,388,608 enterprises.
 *
 * It is worthwhile to note that, in its encoded
 * form, the securityModel value will normally
 * require only a single byte since, in practice,
 * the leftmost bits will be zero for most messages
 * and sign extension is suppressed by the encoding
 * rules.
 *
 * As of this writing, there are several values
 * of securityModel defined for use with SNMP or
 * reserved for use with supporting MIB objects.
 * They are as follows:
 *
 *     0  reserved for 'any'
 *     1  reserved for SNMPv1
 *     2  reserved for SNMPv2c
 *     3  User-Based Security Model (USM)
 *
 * @author Frank Fock
 * @version 3.3
 */


class AGENTPP_DECL SnmpSecurityModel: public SnmpInt32MinMax {

 public:
	SnmpSecurityModel(const Oidx&, mib_access, int, int);
	virtual ~SnmpSecurityModel();

	virtual MibEntryPtr	clone();
};


/*------------------------ class SnmpSecurityLevel -----------------------*/

/**
 * SnmpSecurityLevel
 *
 * A Level of Security at which SNMP messages can be
 * sent or with which operations are being processed;
 * in particular, one of:
 *
 *  noAuthNoPriv - without authentication and
 *                 without privacy,
 *  authNoPriv   - with authentication but
 *                 without privacy,
 *  authPriv     - with authentication and
 *                 with privacy.
 *
 * These three values are ordered such that
 * noAuthNoPriv is less than authNoPriv and
 * authNoPriv is less than authPriv.
 *
 * @author Frank Fock
 * @version 3.3
 */


class AGENTPP_DECL SnmpSecurityLevel: public SnmpInt32MinMax {

 public:
	SnmpSecurityLevel(const Oidx&, mib_access, int, int);
	virtual ~SnmpSecurityLevel();

	virtual MibEntryPtr	clone();
};


/*------------------ class SnmpMessageProcessingModel -------------------*/

/**
 * SnmpMessageProcessingModel
 *
 * An identifier that uniquely identifies a Message
 * Processing Model of the Message Processing
 * Subsystem within a SNMP Management Architecture.
 * The values for messageProcessingModel are
 * allocated as follows:
 *
 * - Values between 0 and 255, inclusive, are
 *   reserved for standards-track Message Processing
 *   Models and are managed by the Internet Assigned
 *   Numbers Authority (IANA).
 *
 * - Values greater than 255 are allocated to
 *   enterprise-specific Message Processing Models.
 *   An enterprise messageProcessingModel value is
 *   defined to be:
 *
 *   enterpriseID * 256 +
 *        messageProcessingModel within enterprise
 *
 *   For example, the fourth Message Processing Model
 *   defined by the enterprise whose enterpriseID
 *   is 1 would be 260.
 *
 * This scheme for allocating messageProcessingModel
 * values allows for a maximum of 255 standards-
 * based Message Processing Models, and for a
 * maximum of 255 Message Processing Models per
 * enterprise.
 *
 * It is believed that the assignment of new
 * messageProcessingModel values will be rare
 * in practice because the larger the number of
 * simultaneously utilized Message Processing Models,
 * the larger the chance that interoperability
 * will suffer. It is believed that such a range
 * will be sufficient.  In the unlikely event that
 * the standards committee finds this number to be
 * insufficient over time, an enterprise number
 * can be allocated to obtain an additional 256
 * possible values.
 *
 * Note that the most significant bit must be zero;
 * hence, there are 23 bits allocated for various
 * organizations to design and define non-standard
 * messageProcessingModels.  This limits the ability
 * to define new proprietary implementations of
 * Message Processing Models to the first 8,388,608
 * enterprises.
 *
 * It is worthwhile to note that, in its encoded
 * form, the messageProcessingModel value will
 * normally require only a single byte since, in
 * practice, the leftmost bits will be zero for
 * most messages and sign extension is suppressed
 * by the encoding rules.
 *
 * As of this writing, there are several values of
 * messageProcessingModel defined for use with SNMP.
 * They are as follows:
 *
 *     0  reserved for SNMPv1
 *     1  reserved for SNMPv2c
 *     2  reserved for SNMPv2u and SNMPv2*
 *     3  reserved for SNMPv3
 *
 * @author Frank Fock
 * @version 3.3
 */


class AGENTPP_DECL SnmpMessageProcessingModel: public SnmpInt32MinMax {

 public:
	SnmpMessageProcessingModel(const Oidx&, mib_access, int, int);
	virtual ~SnmpMessageProcessingModel();

	virtual MibEntryPtr	clone();
};

/*---------------------- class SnmpAdminString ---------------------*/

/**
 * The class SnmpAdminString implements the textual convention 
 * SnmpAdminString as it is defined in the SNMP-FRAMEWORK-MIB:
 * An octet string containing administrative
 * information, preferably in human-readable form.
 *
 * To facilitate internationalization, this
 * information is represented using the ISO/IEC
 * IS 10646-1 character set, encoded as an octet
 * string using the UTF-8 transformation format
 * described in [RFC2279].
 *
 * Since additional code points are added by
 * amendments to the 10646 standard from time
 * to time, implementations must be prepared to
 * encounter any code point from 0x00000000 to
 * 0x7fffffff.  Byte sequences that do not
 * correspond to the valid UTF-8 encoding of a
 * code point or are outside this range are
 * prohibited.
 *
 * The use of control codes should be avoided.
 *
 * When it is necessary to represent a newline,
 * the control code sequence CR LF should be used.
 *
 * The use of leading or trailing white space should
 * be avoided.
 *
 * For code points not directly supported by user
 * interface hardware or software, an alternative
 * means of entry and display, such as hexadecimal,
 * may be provided.
 *
 * For information encoded in 7-bit US-ASCII,
 * the UTF-8 encoding is identical to the
 * US-ASCII encoding.
 *
 * UTF-8 may require multiple bytes to represent a
 * single character / code point; thus the length
 * of this object in octets may be different from
 * the number of characters encoded.  Similarly,
 * size constraints refer to the number of encoded
 * octets, not the number of characters represented
 * by an encoding.
 *
 * Note that when this TC is used for an object that
 * is used or envisioned to be used as an index, then
 * a SIZE restriction MUST be specified so that the
 * number of sub-identifiers for any object instance
 * does not exceed the limit of 128, as defined by
 * [RFC1905].
 *
 * Note that the size of an SnmpAdminString object is
 * measured in octets, not characters.
 *
 *
 * @author Frank Fock
 * @version 3.3
 */

class AGENTPP_DECL SnmpAdminString: public OctetStrMinMax {

 public:
	/**
	 * Construct a SnmpAdminString with initial value.
	 * 
	 * @param oid
	 *    the oid of the leaf or the column of the columnar object.
	 * @param access
	 *    the max-access rights for the object.
	 * @param octetstr
	 *    a pointer to an OctetStr containing the initial value for
	 *    the object.
	 */
	SnmpAdminString(const Oidx&, mib_access, NS_SNMP OctetStr*);
	/**
	 * Construct a SnmpAdminString with initial value.
	 * 
	 * @param oid
	 *    the oid of the leaf or the column of the columnar object.
	 * @param access
	 *    the max-access rights for the object.
	 * @param octetstr
	 *    a pointer to an OctetStr containing the initial value for
	 *    the object.
	 * @param valueMode
	 *    contains an ored value of one or more of the following modes:
	 *    VMODE_DEFAULT: the value of the object pointed by value will 
	 *                   be used as default value, otherwise the managed 
	 *                   object has no default value.
	 *    VMODE_LOCKED:  value is locked (cannot be modified) while
	 *                   the row status of the receiver's row is active(1).
	 *                   This mode has no effect, if the the leaf is a
	 *                   scalar or otherwise if the receiver's row has no
	 *                   snmpRowStatus.  
	 */
	SnmpAdminString(const Oidx&, mib_access, NS_SNMP OctetStr*, int);
	/**
	 * Construct a SnmpAdminString with initial value.
	 * 
	 * @param oid
	 *    the oid of the leaf or the column of the columnar object.
	 * @param access
	 *    the max-access rights for the object.
	 * @param octetstr
	 *    a pointer to an OctetStr containing the initial value for
	 *    the object.
	 * @param valueMode
	 *    contains an ored value of one or more of the following modes:
	 *    VMODE_DEFAULT: the value of the object pointed by value will 
	 *                   be used as default value, otherwise the managed 
	 *                   object has no default value.
	 *    VMODE_LOCKED:  value is locked (cannot be modified) while
	 *                   the row status of the receiver's row is active(1).
	 *                   This mode has no effect, if the the leaf is a
	 *                   scalar or otherwise if the receiver's row has no
	 *                   snmpRowStatus.  
	 * @param min_size
	 *    the minimum size of the SnmpAdminString.
	 * @param max_size
	 *    the maxmimum size of the SnmpAdminString.
	 */
	SnmpAdminString(const Oidx&, mib_access, NS_SNMP OctetStr*, int, int, int);
	virtual ~SnmpAdminString();

	virtual MibEntryPtr	clone();
	
	/**
	 * Return the AdminString
	 *
	 * @return
	 *    a OctetStr instance representing a SnmpAdminString.
	 */
	NS_SNMP OctetStr		get();
};

/*---------------------- class TimeStamp ---------------------*/

/**
 * This class represents managed objects with syntax TimeTicks
 * that store the value of sysUpTime whenever their update
 * method is called. TimeStamp instances are not serializable.
 *
 *
 * @author Frank Fock
 * @version 3.5.22
 */

class AGENTPP_DECL TimeStamp: public MibLeaf 
{
 public:
	/**
	 * Construct a TimeStamp.
	 * 
	 * @param oid
	 *    the oid of the leaf or the column of the columnar object.
	 * @param access
	 *    the max-access rights for the object. (Normally READONLY)
	 * @param valueMode
	 *    contains an ored value of one or more of the following modes:
	 *    VMODE_DEFAULT: the value of the object pointed by value will 
	 *                   be used as default value, otherwise the managed 
	 *                   object has no default value.
	 *    VMODE_LOCKED:  value is locked (cannot be modified) while
	 *                   the row status of the receiver's row is active(1).
	 *                   This mode has no effect, if the the leaf is a
	 *                   scalar or otherwise if the receiver's row has no
	 *                   snmpRowStatus.  
	 */
	TimeStamp(const Oidx&, mib_access, int);
	virtual ~TimeStamp();

	virtual bool		is_volatile() { return TRUE; }

	virtual MibEntryPtr	clone();

	void			update();	
};

/*---------------------- class TimeStampTable ---------------------*/

/**
 * This class represents tables that are associated with a TimeStamp
 * instance that is updated whenever the tables changes.
 *
 * @author Frank Fock
 * @version 3.5.7
 */

class AGENTPP_DECL TimeStampTable: public MibTable 
{
 public:
	/**
	 * Constructor with object identifier and given index structure.
	 * In AGENT++ the structure of a table's index is given by an array
	 * of integer values. Each element represents an object of the index.
	 * A value of -1 represents a implied variable length string or oid.
	 * A value of 0 a variable length string or oid index object.
	 * A value greater than 0 determines the length of the index object 
	 * meassured in subidentifiers. 
	 * 
	 * @param o
	 *    the object identifier of the table, which has to be
	 *    the oid of the the SMI table entry object (table.1).
	 * @param index
	 *    an integer array. The length of the array corresponds to
	 *    the number of objects in the INDEX clause.
	 * @param timeStamp
	 *    a pointer to a TimeStamp instance.
	 */
	TimeStampTable(const Oidx&, const index_info*, unsigned int,
		       TimeStamp*);

	virtual ~TimeStampTable();

	virtual void        	row_added(MibTableRow*, 
					  const Oidx&, MibTable*);
	virtual void        	row_delete(MibTableRow*, 
					   const Oidx&, MibTable*);
	/**
	 * This method should be called whenever the reciever is changed,
	 * except when a row is added or deleted (row_added and row_deleted
	 * already call updated()) 
	 */
	virtual void	       	updated();	

 protected:
	TimeStamp*		lastChange;
};

/*---------------------- class DateAndTime ---------------------*/

/**
 * A date-time specification.
 *
 * field  octets  contents                  range
 * -----  ------  --------                  -----
 *  1      1-2   year*                     0..65536
 *  2       3    month                     1..12
 *  3       4    day                       1..31
 *  4       5    hour                      0..23
 *  5       6    minutes                   0..59
 *  6       7    seconds                   0..60
 *               (use 60 for leap-second)
 *  7       8    deci-seconds              0..9
 *  8       9    direction from UTC        '+' / '-'
 *  9      10    hours from UTC*           0..13
 * 10      11    minutes from UTC          0..59
 *
 * * Notes:
 * - the value of year is in network-byte order
 * - daylight saving time in New Zealand is +13
 *
 * For example, Tuesday May 26, 1992 at 1:30:15 PM EDT would be
 * displayed as:
 *
 *                 1992-5-26,13:30:15.0,-4:0
 *
 * Note that if only local time is known, then timezone
 * information (fields 8-10) is not present.
 *
 * @author Frank Fock
 * @version 3.4.7
 */

class AGENTPP_DECL DateAndTime: public MibLeaf 
{
 public:
	/**
	 * Construct a DateAndTime instance and initialize its value
	 * with the system's date and time.
	 * 
	 * @param oid
	 *    the oid of the leaf or the column of the columnar object.
	 * @param access
	 *    the max-access rights for the object. (Normally READONLY)
	 * @param valueMode
	 *    contains an ored value of one or more of the following modes:
	 *    VMODE_DEFAULT: the value of the object pointed by value will 
	 *                   be used as default value, otherwise the managed 
	 *                   object has no default value.
	 *    VMODE_LOCKED:  value is locked (cannot be modified) while
	 *                   the row status of the receiver's row is active(1).
	 *                   This mode has no effect, if the the leaf is a
	 *                   scalar or otherwise if the receiver's row has no
	 *                   snmpRowStatus.  
	 */
	DateAndTime(const Oidx&, mib_access, int);

	/**
	 * Destructor.
	 */
	virtual ~DateAndTime();

	virtual MibEntryPtr	clone();
	virtual NS_SNMP OctetStr       	get_state();
	virtual void       	set_state(const NS_SNMP OctetStr&);

	virtual bool	is_volatile() { return TRUE; }

	/**
	 * Update current value with the system's date and time.
	 */
	void       	update();
};

#ifdef AGENTPP_NAMESPACE
}
#endif

#endif 
