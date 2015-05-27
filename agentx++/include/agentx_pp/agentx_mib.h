/*_############################################################################
  _## 
  _##  agentx_mib.h  
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


#ifndef _agentx_mib_h
#define _agentx_mib_h


#include <agent_pp/mib.h>

#include <agent_pp/snmp_textual_conventions.h>
#include <agent_pp/notification_originator.h>
#include <agentx_pp/agentx_master.h>


#define oidAgentxDefaultTimeout          "1.3.6.1.2.1.74.1.1.1.0"
#define oidAgentxMasterAgentXVer         "1.3.6.1.2.1.74.1.1.2.0"
#define oidAgentxConnTableLastChange     "1.3.6.1.2.1.74.1.2.1.0"
#define oidAgentxConnectionTable         "1.3.6.1.2.1.74.1.2.2"
#define oidAgentxConnectionEntry         "1.3.6.1.2.1.74.1.2.2.1"
#define oidAgentxConnIndex               "1.3.6.1.2.1.74.1.2.2.1.1"
#define colAgentxConnIndex               "1"
#define oidAgentxConnOpenTime            "1.3.6.1.2.1.74.1.2.2.1.2"
#define colAgentxConnOpenTime            "2"
#define oidAgentxConnTransportDomain     "1.3.6.1.2.1.74.1.2.2.1.3"
#define colAgentxConnTransportDomain     "3"
#define oidAgentxConnTransportAddress    "1.3.6.1.2.1.74.1.2.2.1.4"
#define colAgentxConnTransportAddress    "4"
#define oidAgentxSessionTableLastChange  "1.3.6.1.2.1.74.1.3.1.0"
#define oidAgentxSessionTable            "1.3.6.1.2.1.74.1.3.2"
#define oidAgentxSessionEntry            "1.3.6.1.2.1.74.1.3.2.1"
#define oidAgentxSessionIndex            "1.3.6.1.2.1.74.1.3.2.1.1"
#define colAgentxSessionIndex            "1"
#define oidAgentxSessionObjectID         "1.3.6.1.2.1.74.1.3.2.1.2"
#define colAgentxSessionObjectID         "2"
#define oidAgentxSessionDescr            "1.3.6.1.2.1.74.1.3.2.1.3"
#define colAgentxSessionDescr            "3"
#define oidAgentxSessionAdminStatus      "1.3.6.1.2.1.74.1.3.2.1.4"
#define colAgentxSessionAdminStatus      "4"
#define oidAgentxSessionOpenTime         "1.3.6.1.2.1.74.1.3.2.1.5"
#define colAgentxSessionOpenTime         "5"
#define oidAgentxSessionAgentXVer        "1.3.6.1.2.1.74.1.3.2.1.6"
#define colAgentxSessionAgentXVer        "6"
#define oidAgentxSessionTimeout          "1.3.6.1.2.1.74.1.3.2.1.7"
#define colAgentxSessionTimeout          "7"
#define oidAgentxRegistrationTableLastChange "1.3.6.1.2.1.74.1.4.1.0"
#define oidAgentxRegistrationTable       "1.3.6.1.2.1.74.1.4.2"
#define oidAgentxRegistrationEntry       "1.3.6.1.2.1.74.1.4.2.1"
#define oidAgentxRegIndex                "1.3.6.1.2.1.74.1.4.2.1.1"
#define colAgentxRegIndex                "1"
#define oidAgentxRegContext              "1.3.6.1.2.1.74.1.4.2.1.2"
#define colAgentxRegContext              "2"
#define oidAgentxRegStart                "1.3.6.1.2.1.74.1.4.2.1.3"
#define colAgentxRegStart                "3"
#define oidAgentxRegRangeSubId           "1.3.6.1.2.1.74.1.4.2.1.4"
#define colAgentxRegRangeSubId           "4"
#define oidAgentxRegUpperBound           "1.3.6.1.2.1.74.1.4.2.1.5"
#define colAgentxRegUpperBound           "5"
#define oidAgentxRegPriority             "1.3.6.1.2.1.74.1.4.2.1.6"
#define colAgentxRegPriority             "6"
#define oidAgentxRegTimeout              "1.3.6.1.2.1.74.1.4.2.1.7"
#define colAgentxRegTimeout              "7"
#define oidAgentxRegInstance             "1.3.6.1.2.1.74.1.4.2.1.8"
#define colAgentxRegInstance             "8"

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif



/**
 *  agentxSessionAdminStatus
 *
"The administrative (desired) status of the session. Setting
 the value to 'down(2)' closes the subagent session (with c.reason
 set to 'reasonByManager').
 "
 */


class agentxSessionAdminStatus: public MibLeaf {

public:
	agentxSessionAdminStatus(const Oidx&);
	virtual ~agentxSessionAdminStatus();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);

	static void		set_writable(bool);

 protected:
	static mib_access       myaccess;
};




/**
 *  agentxConnectionEntry
 *
"An agentxConnectionEntry contains information describing a
 single AgentX transport connection. A connection may be
 used to support zero or more AgentX sessions. An entry is
 created when a new transport connection is established,
 and is destroyed when the transport connection is terminated.
 "
 */


class agentxConnectionEntry: public TimeStampTable {

public:
	agentxConnectionEntry(TimeStamp*);
	virtual ~agentxConnectionEntry();

	static agentxConnectionEntry* instance;

	virtual void       	set_row(MibTableRow*, u_int, 
					const Oidx&, const NS_SNMP OctetStr&);
	virtual u_int		add(const Oidx&, const NS_SNMP OctetStr&);
	virtual void		remove(u_int);

 protected:
	static u_int	next_connection;
};


/**
 *  agentxSessionEntry
 *
"Information about a single open session between the AgentX
 master agent and a subagent is contained in this entry. An
 entry is created when a new session is successfully established
 and is destroyed either when the subagent transport connection
 has terminated or when the subagent session is closed.
 "
 */


class agentxSessionEntry: public TimeStampTable {
friend class agentxSessionAdminStatus;
public:
	agentxSessionEntry(TimeStamp*);
	virtual ~agentxSessionEntry();

	static agentxSessionEntry* instance;

	virtual void       	set_row(MibTableRow*, const Oidx&, 
					const NS_SNMP OctetStr&, int, u_int,
					u_char, u_char);
	virtual void		add(u_int, u_int, const Oidx&, const NS_SNMP OctetStr&,
				    u_char, u_char);
	virtual void		remove(u_int, u_int);

	void			set_master(MasterAgentXMib* m) { master = m; }
	/**
	 * Deletes a given row when the next Mib::cleanup is called.
	 *
	 * @param row
	 *    a pointer to a MibTablRow instance of this table.
	 */
	void			deferred_delete(MibTableRow*);

 protected:
	MasterAgentXMib*	master;

};


#define AX_REG_CID	1
#define AX_REG_SID	2
#define AX_REG_RID	3


/**
 *  agentxRegistrationEntry
 *
"Contains information for a single registered region. An
 entry is created when a session successfully registers a
 region and is destroyed for any of three reasons: this region
 is unregistered by the session, the session is closed,
 or the subagent connection is closed.
 "
 */

class agentxRegistrationEntry: public TimeStampTable {

public:
	agentxRegistrationEntry(TimeStamp*);
	virtual ~agentxRegistrationEntry();

	static agentxRegistrationEntry* instance;

	virtual void       	set_row(MibTableRow* r, const NS_SNMP OctetStr&, 
					const Oidx&, u_int, u_int, u_int,
					u_char, int);
	virtual u_int		add(u_int, u_int, const NS_SNMP OctetStr&, 
				    const Oidx&, u_int, u_int, u_int,
				    u_char, bool);
	virtual void		remove(u_int, u_int, u_int);
 protected:
	static u_int	next_reg;
};


class AGENTXPP_DECL agentx_mib: public MibGroup
{
  public:
	agentx_mib();
	virtual ~agentx_mib() { }
};

#ifdef AGENTPP_NAMESPACE
}
#endif


#endif


