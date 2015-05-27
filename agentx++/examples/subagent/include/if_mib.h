/*_############################################################################
  _## 
  _##  if_mib.h  
  _## 
  _##
  _##  AgentX++ 2
  _##  -------------------------------------
  _##  Copyright (C) 2000-2013 - Frank Fock
  _##  
  _##  Use of this software is subject to the license agreement you received
  _##  with this software and which can be downloaded from 
  _##  http://www.agentpp.com
  _##
  _##  This is licensed software and may not be used in a commercial
  _##  environment, except for evaluation purposes, unless a valid
  _##  license has been purchased.
  _##  
  _##########################################################################*/


#ifndef _if_mib_h
#define _if_mib_h

#include <agent_pp/mib.h>

#include <agent_pp/snmp_textual_conventions.h>
#include <agent_pp/notification_originator.h>
#include <agentx_pp/agentx_subagent.h>

#ifndef IF_MIB_SHARED_TABLES
#define IF_MIB_SHARED_TABLES
#endif


#define oidIfNumber                      "1.3.6.1.2.1.2.1.0"
#define oidIfTable                       "1.3.6.1.2.1.2.2"
#define oidIfEntry                       "1.3.6.1.2.1.2.2.1"
#define oidIfIndex                       "1.3.6.1.2.1.2.2.1.1"
#define colIfIndex                       "1"
#define oidIfDescr                       "1.3.6.1.2.1.2.2.1.2"
#define colIfDescr                       "2"
#define oidIfType                        "1.3.6.1.2.1.2.2.1.3"
#define colIfType                        "3"
#define oidIfMtu                         "1.3.6.1.2.1.2.2.1.4"
#define colIfMtu                         "4"
#define oidIfSpeed                       "1.3.6.1.2.1.2.2.1.5"
#define colIfSpeed                       "5"
#define oidIfPhysAddress                 "1.3.6.1.2.1.2.2.1.6"
#define colIfPhysAddress                 "6"
#define oidIfAdminStatus                 "1.3.6.1.2.1.2.2.1.7"
#define colIfAdminStatus                 "7"
#define oidIfOperStatus                  "1.3.6.1.2.1.2.2.1.8"
#define colIfOperStatus                  "8"
#define oidIfLastChange                  "1.3.6.1.2.1.2.2.1.9"
#define colIfLastChange                  "9"
#define oidIfInOctets                    "1.3.6.1.2.1.2.2.1.10"
#define colIfInOctets                    "10"
#define oidIfInUcastPkts                 "1.3.6.1.2.1.2.2.1.11"
#define colIfInUcastPkts                 "11"
#define oidIfInNUcastPkts                "1.3.6.1.2.1.2.2.1.12"
#define colIfInNUcastPkts                "12"
#define oidIfInDiscards                  "1.3.6.1.2.1.2.2.1.13"
#define colIfInDiscards                  "13"
#define oidIfInErrors                    "1.3.6.1.2.1.2.2.1.14"
#define colIfInErrors                    "14"
#define oidIfInUnknownProtos             "1.3.6.1.2.1.2.2.1.15"
#define colIfInUnknownProtos             "15"
#define oidIfOutOctets                   "1.3.6.1.2.1.2.2.1.16"
#define colIfOutOctets                   "16"
#define oidIfOutUcastPkts                "1.3.6.1.2.1.2.2.1.17"
#define colIfOutUcastPkts                "17"
#define oidIfOutNUcastPkts               "1.3.6.1.2.1.2.2.1.18"
#define colIfOutNUcastPkts               "18"
#define oidIfOutDiscards                 "1.3.6.1.2.1.2.2.1.19"
#define colIfOutDiscards                 "19"
#define oidIfOutErrors                   "1.3.6.1.2.1.2.2.1.20"
#define colIfOutErrors                   "20"
#define oidIfOutQLen                     "1.3.6.1.2.1.2.2.1.21"
#define colIfOutQLen                     "21"
#define oidIfSpecific                    "1.3.6.1.2.1.2.2.1.22"
#define colIfSpecific                    "22"
#define oidIfXTable                      "1.3.6.1.2.1.31.1.1"
#define oidIfXEntry                      "1.3.6.1.2.1.31.1.1.1"
#define oidIfName                        "1.3.6.1.2.1.31.1.1.1.1"
#define colIfName                        "1"
#define oidIfInMulticastPkts             "1.3.6.1.2.1.31.1.1.1.2"
#define colIfInMulticastPkts             "2"
#define oidIfInBroadcastPkts             "1.3.6.1.2.1.31.1.1.1.3"
#define colIfInBroadcastPkts             "3"
#define oidIfOutMulticastPkts            "1.3.6.1.2.1.31.1.1.1.4"
#define colIfOutMulticastPkts            "4"
#define oidIfOutBroadcastPkts            "1.3.6.1.2.1.31.1.1.1.5"
#define colIfOutBroadcastPkts            "5"
#define oidIfHCInOctets                  "1.3.6.1.2.1.31.1.1.1.6"
#define colIfHCInOctets                  "6"
#define oidIfHCInUcastPkts               "1.3.6.1.2.1.31.1.1.1.7"
#define colIfHCInUcastPkts               "7"
#define oidIfHCInMulticastPkts           "1.3.6.1.2.1.31.1.1.1.8"
#define colIfHCInMulticastPkts           "8"
#define oidIfHCInBroadcastPkts           "1.3.6.1.2.1.31.1.1.1.9"
#define colIfHCInBroadcastPkts           "9"
#define oidIfHCOutOctets                 "1.3.6.1.2.1.31.1.1.1.10"
#define colIfHCOutOctets                 "10"
#define oidIfHCOutUcastPkts              "1.3.6.1.2.1.31.1.1.1.11"
#define colIfHCOutUcastPkts              "11"
#define oidIfHCOutMulticastPkts          "1.3.6.1.2.1.31.1.1.1.12"
#define colIfHCOutMulticastPkts          "12"
#define oidIfHCOutBroadcastPkts          "1.3.6.1.2.1.31.1.1.1.13"
#define colIfHCOutBroadcastPkts          "13"
#define oidIfLinkUpDownTrapEnable        "1.3.6.1.2.1.31.1.1.1.14"
#define colIfLinkUpDownTrapEnable        "14"
#define oidIfHighSpeed                   "1.3.6.1.2.1.31.1.1.1.15"
#define colIfHighSpeed                   "15"
#define oidIfPromiscuousMode             "1.3.6.1.2.1.31.1.1.1.16"
#define colIfPromiscuousMode             "16"
#define oidIfConnectorPresent            "1.3.6.1.2.1.31.1.1.1.17"
#define colIfConnectorPresent            "17"
#define oidIfAlias                       "1.3.6.1.2.1.31.1.1.1.18"
#define colIfAlias                       "18"
#define oidIfCounterDiscontinuityTime    "1.3.6.1.2.1.31.1.1.1.19"
#define colIfCounterDiscontinuityTime    "19"
#define oidIfStackTable                  "1.3.6.1.2.1.31.1.2"
#define oidIfStackEntry                  "1.3.6.1.2.1.31.1.2.1"
#define oidIfStackHigherLayer            "1.3.6.1.2.1.31.1.2.1.1"
#define colIfStackHigherLayer            "1"
#define oidIfStackLowerLayer             "1.3.6.1.2.1.31.1.2.1.2"
#define colIfStackLowerLayer             "2"
#define oidIfStackStatus                 "1.3.6.1.2.1.31.1.2.1.3"
#define colIfStackStatus                 "3"
#define oidIfTestTable                   "1.3.6.1.2.1.31.1.3"
#define oidIfTestEntry                   "1.3.6.1.2.1.31.1.3.1"
#define oidIfTestId                      "1.3.6.1.2.1.31.1.3.1.1"
#define colIfTestId                      "1"
#define oidIfTestStatus                  "1.3.6.1.2.1.31.1.3.1.2"
#define colIfTestStatus                  "2"
#define oidIfTestType                    "1.3.6.1.2.1.31.1.3.1.3"
#define colIfTestType                    "3"
#define oidIfTestResult                  "1.3.6.1.2.1.31.1.3.1.4"
#define colIfTestResult                  "4"
#define oidIfTestCode                    "1.3.6.1.2.1.31.1.3.1.5"
#define colIfTestCode                    "5"
#define oidIfTestOwner                   "1.3.6.1.2.1.31.1.3.1.6"
#define colIfTestOwner                   "6"
#define oidIfRcvAddressTable             "1.3.6.1.2.1.31.1.4"
#define oidIfRcvAddressEntry             "1.3.6.1.2.1.31.1.4.1"
#define oidIfRcvAddressAddress           "1.3.6.1.2.1.31.1.4.1.1"
#define colIfRcvAddressAddress           "1"
#define oidIfRcvAddressStatus            "1.3.6.1.2.1.31.1.4.1.2"
#define colIfRcvAddressStatus            "2"
#define oidIfRcvAddressType              "1.3.6.1.2.1.31.1.4.1.3"
#define colIfRcvAddressType              "3"
#define oidIfTableLastChange             "1.3.6.1.2.1.31.1.5.0"
#define oidIfStackLastChange             "1.3.6.1.2.1.31.1.6.0"
#define oidLinkDown                      "1.3.6.1.6.3.1.1.5.3"
#define oidLinkUp                        "1.3.6.1.6.3.1.1.5.4"


//--AgentGen BEGIN=_INCLUDE
#include <agentx_pp/agentx_subagent.h>

#define MAX_IF_TYPES 161		/* From IANAifType-MIB */
#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif
//--AgentGen END


/**
 *  ifNumber
 *
 * "The number of network interfaces (regardless of their
 * current state) present on this system."
 */


class ifNumber: public MibLeaf {

public:
	ifNumber();
	virtual ~ifNumber();

	static ifNumber* instance;

	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);

//--AgentGen BEGIN=ifNumber
//--AgentGen END
};


/**
 *  ifAdminStatus
 *
 * "The desired state of the interface.  The testing(3) state
 * indicates that no operational packets can be passed.  When a
 * managed system initializes, all interfaces start with
 * ifAdminStatus in the down(2) state.  As a result of either
 * explicit management action or per configuration information
 * retained by the managed system, ifAdminStatus is then
 * changed to either the up(1) or testing(3) states (or remains
 * in the down(2) state)."
 */


class ifAdminStatus: public MibLeaf {

public:
	ifAdminStatus(const Oidx&);
	virtual ~ifAdminStatus();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);

//--AgentGen BEGIN=ifAdminStatus
//--AgentGen END
};


/**
 *  ifLinkUpDownTrapEnable
 *
 * "Indicates whether linkUp/linkDown traps should be generated
 * for this interface.

 * By default, this object should have the value enabled(1) for
 * interfaces which do not operate on 'top' of any other
 * interface (as defined in the ifStackTable), and disabled(2)
 * otherwise."
 */


class ifLinkUpDownTrapEnable: public MibLeaf {

public:
	ifLinkUpDownTrapEnable(const Oidx&);
	virtual ~ifLinkUpDownTrapEnable();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);

//--AgentGen BEGIN=ifLinkUpDownTrapEnable
//--AgentGen END
};


/**
 *  ifPromiscuousMode
 *
 * "This object has a value of false(2) if this interface only
 * accepts packets/frames that are addressed to this station.
 * This object has a value of true(1) when the station accepts
 * all packets/frames transmitted on the media.  The value
 * true(1) is only legal on certain types of media.  If legal,
 * setting this object to a value of true(1) may require the
 * interface to be reset before becoming effective.

 * The value of ifPromiscuousMode does not affect the reception
 * of broadcast and multicast packets/frames by the interface."
 */


class ifPromiscuousMode: public MibLeaf {

public:
	ifPromiscuousMode(const Oidx&);
	virtual ~ifPromiscuousMode();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);

//--AgentGen BEGIN=ifPromiscuousMode
//--AgentGen END
};


/**
 *  ifAlias
 *
 * "This object is an 'alias' name for the interface as
 * specified by a network manager, and provides a non-volatile
 * 'handle' for the interface.

 * On the first instantiation of an interface, the value of
 * ifAlias associated with that interface is the zero-length
 * string.  As and when a value is written into an instance of
 * ifAlias through a network management set operation, then the
 * agent must retain the supplied value in the ifAlias instance
 * associated with the same interface for as long as that
 * interface remains instantiated, including across all re-
 * initializations/reboots of the network management system,
 * including those which result in a change of the interface's
 * ifIndex value.

 * An example of the value which a network manager might store
 * in this object for a WAN interface is the (Telco's) circuit
 * number/identifier of the interface.

 * Some agents may support write-access only for interfaces
 * having particular values of ifType.  An agent which supports
 * write access to this object is required to keep the value in
 * non-volatile storage, but it may limit the length of new
 * values depending on how much storage is already occupied by
 * the current values for other interfaces."
 */


class ifAlias: public SnmpDisplayString {

public:
	ifAlias(const Oidx&);
	virtual ~ifAlias();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);

//--AgentGen BEGIN=ifAlias
//--AgentGen END
};


/**
 *  ifStackStatus
 *
 * "The status of the relationship between two sub-layers.

 * Changing the value of this object from 'active' to
 * 'notInService' or 'destroy' will likely have consequences up
 * and down the interface stack.  Thus, write access to this
 * object is likely to be inappropriate for some types of
 * interfaces, and many implementations will choose not to
 * support write-access for any type of interface."
 */


class ifStackStatus: public snmpRowStatus {

public:
	ifStackStatus(const Oidx&);
	virtual ~ifStackStatus();

	virtual MibEntryPtr	clone();
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);

//--AgentGen BEGIN=ifStackStatus
//--AgentGen END
};


/**
 *  ifTestId
 *
 * "This object identifies the current invocation of the
 * interface's test."
 */


class ifTestId: public MibLeaf {

public:
	ifTestId(const Oidx&);
	virtual ~ifTestId();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);

//--AgentGen BEGIN=ifTestId
//--AgentGen END
};


/**
 *  ifTestStatus
 *
 * "This object indicates whether or not some manager currently
 * has the necessary 'ownership' required to invoke a test on
 * this interface.  A write to this object is only successful
 * when it changes its value from 'notInUse(1)' to 'inUse(2)'.
 * After completion of a test, the agent resets the value back
 * to 'notInUse(1)'."
 */


class ifTestStatus: public MibLeaf {

public:
	ifTestStatus(const Oidx&);
	virtual ~ifTestStatus();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);

//--AgentGen BEGIN=ifTestStatus
//--AgentGen END
};


/**
 *  ifTestType
 *
 * "A control variable used to start and stop operator-
 * initiated interface tests.  Most OBJECT IDENTIFIER values
 * assigned to tests are defined elsewhere, in association with
 * specific types of interface.  However, this document assigns
 * a value for a full-duplex loopback test, and defines the
 * special meanings of the subject identifier:

 *     noTest  OBJECT IDENTIFIER ::= { 0 0 }

 * When the value noTest is written to this object, no action
 * is taken unless a test is in progress, in which case the
 * test is aborted.  Writing any other value to this object is
 * only valid when no test is currently in progress, in which
 * case the indicated test is initiated.

 * When read, this object always returns the most recent value
 * that ifTestType was set to.  If it has not been set since
 * the last initialization of the network management subsystem
 * on the agent, a value of noTest is returned."
 */


class ifTestType: public MibLeaf {

public:
	ifTestType(const Oidx&);
	virtual ~ifTestType();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);

//--AgentGen BEGIN=ifTestType
//--AgentGen END
};


/**
 *  ifTestOwner
 *
 * "The entity which currently has the 'ownership' required to
 * invoke a test on this interface."
 */


class ifTestOwner: public MibLeaf {

public:
	ifTestOwner(const Oidx&);
	virtual ~ifTestOwner();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);

//--AgentGen BEGIN=ifTestOwner
//--AgentGen END
};


/**
 *  ifRcvAddressStatus
 *
 * "This object is used to create and delete rows in the
 * ifRcvAddressTable."
 */


class ifRcvAddressStatus: public snmpRowStatus {

public:
	ifRcvAddressStatus(const Oidx&);
	virtual ~ifRcvAddressStatus();

	virtual MibEntryPtr	clone();
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);

//--AgentGen BEGIN=ifRcvAddressStatus
//--AgentGen END
};


/**
 *  ifRcvAddressType
 *
 * "This object has the value nonVolatile(3) for those entries
 * in the table which are valid and will not be deleted by the
 * next restart of the managed system.  Entries having the
 * value volatile(2) are valid and exist, but have not been
 * saved, so that will not exist after the next restart of the
 * managed system.  Entries having the value other(1) are valid
 * and exist but are not classified as to whether they will
 * continue to exist after the next restart."
 */


class ifRcvAddressType: public MibLeaf {

public:
	ifRcvAddressType(const Oidx&);
	virtual ~ifRcvAddressType();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);

//--AgentGen BEGIN=ifRcvAddressType
//--AgentGen END
};


/**
 *  ifTableLastChange
 *
 * "The value of sysUpTime at the time of the last creation or
 * deletion of an entry in the ifTable.  If the number of
 * entries has been unchanged since the last re-initialization
 * of the local network management subsystem, then this object
 * contains a zero value."
 */


class ifTableLastChange: public MibLeaf {

public:
	ifTableLastChange();
	virtual ~ifTableLastChange();

	static ifTableLastChange* instance;

	virtual void       	get_request(Request*, int);

//--AgentGen BEGIN=ifTableLastChange
//--AgentGen END
};


/**
 *  ifStackLastChange
 *
 * "The value of sysUpTime at the time of the last change of
 * the (whole) interface stack.  A change of the interface
 * stack is defined to be any creation, deletion, or change in
 * value of any instance of ifStackStatus.  If the interface
 * stack has been unchanged since the last re-initialization of
 * the local network management subsystem, then this object
 * contains a zero value."
 */


class ifStackLastChange: public MibLeaf {

public:
	ifStackLastChange();
	virtual ~ifStackLastChange();

	static ifStackLastChange* instance;

	virtual void       	get_request(Request*, int);

//--AgentGen BEGIN=ifStackLastChange
//--AgentGen END
};


/**
 *  linkDown
 *
 * "A linkDown trap signifies that the SNMP entity, acting in
 * an agent role, has detected that the ifOperStatus object for
 * one of its communication links is about to enter the down
 * state from some other state (but not from the notPresent
 * state).  This other state is indicated by the included value
 * of ifOperStatus."
 */


class linkDown {

public:
	linkDown(SubAgentXMib*);
	virtual ~linkDown();

	virtual void        	generate(Vbx*, int, const NS_SNMP OctetStr&);
protected:
	SubAgentXMib*	my_mib;

//--AgentGen BEGIN=linkDown
	SubAgentXMib* backReference;
//--AgentGen END
};


/**
 *  linkUp
 *
 * "A linkUp trap signifies that the SNMP entity, acting in an
 * agent role, has detected that the ifOperStatus object for
 * one of its communication links left the down state and
 * transitioned into some other state (but not into the
 * notPresent state).  This other state is indicated by the
 * included value of ifOperStatus."
 */


class linkUp {

public:
	linkUp(SubAgentXMib*);
	virtual ~linkUp();

	virtual void        	generate(Vbx*, int, const NS_SNMP OctetStr&);
protected:
	SubAgentXMib*	my_mib;

//--AgentGen BEGIN=linkUp
	SubAgentXMib* backReference;
//--AgentGen END
};


/**
 *  ifEntry
 *
 * "An entry containing management information applicable to a
 * particular interface."
 */


class ifEntry: public AgentXSharedTable {

public:
	ifEntry(const NS_SNMP OctetStr&, SubAgentXMib*);
	virtual ~ifEntry();

	static ifEntry* instance;

	virtual void        	update(Request*);
	virtual MibTableRow*	init_row(const Oidx&, Vbx*); 

//--AgentGen BEGIN=ifEntry
#ifdef linux
	virtual void		index_allocated(const Oidx&, int, int, Vbx* vbs=0, u_int vbs_length=0);
	virtual void	       	do_update();
 protected:
	int			if_type_from_name(const char*name);
	int			if_speed_from_type(int);
	void                    init_if_speeds();
	time_t			lastUpdated;
        static int if_speeds[ MAX_IF_TYPES ];
#endif	
	List<MibTableRow>       newRows;

//--AgentGen END
};


/**
 *  ifXEntry
 *
 * "An entry containing additional management information
 * applicable to a particular interface."
 */


class ifXEntry: public AgentXSharedTable {

public:
	ifXEntry(const NS_SNMP OctetStr&, SubAgentXMib*);
	virtual ~ifXEntry();

	static ifXEntry* instance;

	virtual void        	update(Request*);

//--AgentGen BEGIN=ifXEntry
//--AgentGen END
};


/**
 *  ifStackEntry
 *
 * "Information on a particular relationship between two sub-
 * layers, specifying that one sub-layer runs on 'top' of the
 * other sub-layer.  Each sub-layer corresponds to a conceptual
 * row in the ifTable."
 */


class ifStackEntry: public AgentXSharedTable {

public:
	ifStackEntry(const NS_SNMP OctetStr&, SubAgentXMib*);
	virtual ~ifStackEntry();

	static ifStackEntry* instance;

	virtual void        	update(Request*);

//--AgentGen BEGIN=ifStackEntry
//--AgentGen END
};


/**
 *  ifTestEntry
 *
 * "An entry containing objects for invoking tests on an
 * interface."
 */


class ifTestEntry: public AgentXSharedTable {

public:
	ifTestEntry(const NS_SNMP OctetStr&, SubAgentXMib*);
	virtual ~ifTestEntry();

	static ifTestEntry* instance;

	virtual void        	update(Request*);

//--AgentGen BEGIN=ifTestEntry
//--AgentGen END
};


/**
 *  ifRcvAddressEntry
 *
 * "A list of objects identifying an address for which the
 * system will accept packets/frames on the particular
 * interface identified by the index value ifIndex."
 */


class ifRcvAddressEntry: public AgentXSharedTable {

public:
	ifRcvAddressEntry(const NS_SNMP OctetStr&, SubAgentXMib*);
	virtual ~ifRcvAddressEntry();

	static ifRcvAddressEntry* instance;

	virtual void        	update(Request*);

//--AgentGen BEGIN=ifRcvAddressEntry
//--AgentGen END
};


class if_mib: public MibGroup
{
  public:
	if_mib(const NS_SNMP OctetStr&, SubAgentXMib*);
	virtual ~if_mib() { }

//--AgentGen BEGIN=if_mib
//--AgentGen END

};

//--AgentGen BEGIN=_END
#ifdef AGENTPP_NAMESPACE
}
#endif

//--AgentGen END


/**
 * if_mib.h generated by AgentGen 1.5.2 for AGENT++v3.4 
 * Wed Feb 28 01:09:22 GMT+01:00 2001.
 */

#endif


