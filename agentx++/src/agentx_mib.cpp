/*_############################################################################
  _## 
  _##  agentx_mib.cpp  
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

#include <agentx_pp/agentx_def.h>

#include <agentx_pp/agentx_mib.h>
#include <agent_pp/system_group.h>

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


// static const char *loggerModuleName = "agentx++.agentx_mib";

    
/**
 *  agentxSessionAdminStatus
 *
 */

mib_access agentxSessionAdminStatus::myaccess = READWRITE;

agentxSessionAdminStatus::agentxSessionAdminStatus(const Oidx& id):
   MibLeaf(id, myaccess, new SnmpInt32())
{
}

agentxSessionAdminStatus::~agentxSessionAdminStatus()
{
    if (get_state() == 2) {
		((agentxSessionEntry*)my_table)->
		    master->close_session(my_row->get_index()[0],
					  AGENTX_REASON_BY_MANAGER);
    }
}

void agentxSessionAdminStatus::set_writable(bool writable) 
{
	if (writable)
		myaccess = READWRITE;
	else 
		myaccess = READONLY;
}

MibEntryPtr agentxSessionAdminStatus::clone()
{
	MibEntryPtr other = new agentxSessionAdminStatus(oid);
	((agentxSessionAdminStatus*)other)->replace_value(value->clone());
	((agentxSessionAdminStatus*)other)->set_reference_to_table(my_table);
	return other;
}

void agentxSessionAdminStatus::get_request(Request* req, int ind)
{
	// place instrumentation code (manipulating "value") here
	MibLeaf::get_request(req, ind);
}

long agentxSessionAdminStatus::get_state()
{
	return (long)*((SnmpInt32*)value);
}

void agentxSessionAdminStatus::set_state(long l)
{
	*((SnmpInt32*)value) = l;
}

int agentxSessionAdminStatus::set(const Vbx& vb)
{
	long v;
	vb.get_value(v);
	if (v == 2) {
		((agentxSessionEntry*)my_table)->deferred_delete(my_row);
	}
//	((agentxSessionEntry*)my_table)->updated();
	return MibLeaf::set(vb);
}

bool agentxSessionAdminStatus::value_ok(const Vbx& vb)
{
	long v;
	vb.get_value(v);
	if ((v != 1)
	     && (v != 2)) return FALSE;
	if ((!((agentxSessionEntry*)my_table)->master) 
	    && (v == 2)) return FALSE;
	return TRUE;
}

int agentxSessionAdminStatus::prepare_set_request(Request* req, int& ind)
{
	int status;
	if (get_state() == 2) 
	    return SNMP_ERROR_INCONSIST_VAL;
	if ((status = MibLeaf::prepare_set_request(req, ind)) !=
	    SNMP_ERROR_SUCCESS) return status;
	return SNMP_ERROR_SUCCESS;
}






/**
 *  agentxConnectionEntry
 *
 */

agentxConnectionEntry* agentxConnectionEntry::instance = 0;
u_int agentxConnectionEntry::next_connection = 1;

const index_info indAgentxConnectionEntry[1] = {
	{ sNMP_SYNTAX_INT, FALSE, 1, 1 } };

agentxConnectionEntry::agentxConnectionEntry(TimeStamp* lc):
   TimeStampTable(oidAgentxConnectionEntry, indAgentxConnectionEntry, 1, lc)
{
	instance = this;

	add_col(new MibLeaf(colAgentxConnOpenTime, 
			    READONLY, new TimeTicks(0)));
	add_col(new MibLeaf(colAgentxConnTransportDomain, 
			    READONLY, new Oid()));
	add_col(new MibLeaf(colAgentxConnTransportAddress,
			    READONLY, new OctetStr()));
}

agentxConnectionEntry::~agentxConnectionEntry()
{
}

void agentxConnectionEntry::set_row(MibTableRow* r, u_int p0, const Oidx& p1,
				    const OctetStr& p2)
{
	r->get_nth(0)->replace_value(new TimeTicks(p0));
	r->get_nth(1)->replace_value(new Oid(p1));
	r->get_nth(2)->replace_value(new OctetStr(p2));
}

u_int agentxConnectionEntry::add(const Oidx& td, const OctetStr& addr)
TS_SYNCHRONIZED(
{
	Oidx ind;
	ind += next_connection++;
	set_row(add_row(ind), sysUpTime::get(), td, addr);
	return (u_int)ind[0];
})

void agentxConnectionEntry::remove(u_int id) 
TS_SYNCHRONIZED(
{
	Oidx ind;
	ind += id;
	remove_row(ind);
})

/**
 *  agentxSessionEntry
 *
 */

agentxSessionEntry* agentxSessionEntry::instance = 0;

const index_info indAgentxSessionEntry[2] = {
	{ sNMP_SYNTAX_INT, FALSE, 1, 1 },
	 { sNMP_SYNTAX_INT, FALSE, 1, 1 } };

agentxSessionEntry::agentxSessionEntry(TimeStamp* lc):
   TimeStampTable(oidAgentxSessionEntry, indAgentxSessionEntry, 2, lc)
{
	// This table object is a singleton. In order to access it use
	// the static pointer agentxSessionEntry::instance.
	instance = this;

	master = 0;

	add_col(new MibLeaf(colAgentxSessionObjectID, 
			    READONLY, new Oid()));
	add_col(new MibLeaf(colAgentxSessionDescr,
			    READONLY, new OctetStr()));
	add_col(new agentxSessionAdminStatus(colAgentxSessionAdminStatus));
	add_col(new MibLeaf(colAgentxSessionOpenTime,
			    READONLY, new TimeTicks(0)));
	add_col(new MibLeaf(colAgentxSessionAgentXVer, 
			    READONLY, new SnmpInt32(1)));
	add_col(new MibLeaf(colAgentxSessionTimeout,
			    READONLY, new SnmpInt32(5)));
}

agentxSessionEntry::~agentxSessionEntry()
{
	master = 0;
}

void agentxSessionEntry::set_row(MibTableRow* r, const Oidx& p0, 
				 const OctetStr& p1, int p2, 
				 u_int p3, u_char p4, u_char p5)
{
	r->get_nth(0)->replace_value(new Oid(p0));
	r->get_nth(1)->replace_value(new OctetStr(p1));
	r->get_nth(2)->replace_value(new SnmpInt32(p2));
	r->get_nth(3)->replace_value(new TimeTicks(p3));
	r->get_nth(4)->replace_value(new SnmpInt32(p4));
	r->get_nth(5)->replace_value(new SnmpInt32(p5));
}

void agentxSessionEntry::add(u_int cid, u_int id, 
			     const Oidx& o, const OctetStr& descr,
			     u_char vers, u_char timeout)
TS_SYNCHRONIZED(
{
	Oidx ind;
	ind += cid;
	ind += id;
	set_row(add_row(ind), o, descr, 1, sysUpTime::get(), vers, timeout);
})

void agentxSessionEntry::remove(u_int cid, u_int id)
TS_SYNCHRONIZED(
{
	Oidx ind;
	ind += cid;
	ind += id;
	MibTableRow* r = find_index(ind);
	// do not delete row if session is being closed by SNMP request 
	if ((r) && 
	    (((agentxSessionAdminStatus*)r->get_nth(2))->get_state() != 2))
	    remove_row(ind);
})

void agentxSessionEntry::deferred_delete(MibTableRow* r)
{
	notready_rows.add(r);
}


/**
 *  agentxRegistrationEntry
 *
 */

agentxRegistrationEntry* agentxRegistrationEntry::instance = 0;
u_int agentxRegistrationEntry::next_reg = 1;

const index_info indAgentxRegistrationEntry[3] = {
	{ sNMP_SYNTAX_INT, FALSE, 1, 1 },
	 { sNMP_SYNTAX_INT, FALSE, 1, 1 },
	 { sNMP_SYNTAX_INT, FALSE, 1, 1 } };

agentxRegistrationEntry::agentxRegistrationEntry(TimeStamp* lc):
   TimeStampTable(oidAgentxRegistrationEntry, 
		  indAgentxRegistrationEntry, 3, lc)
{
	instance = this;

	add_col(new MibLeaf(colAgentxRegContext, READONLY, new OctetStr()));
	add_col(new MibLeaf(colAgentxRegStart, READONLY, new Oid()));
	add_col(new MibLeaf(colAgentxRegRangeSubId, 
			    READONLY, new Gauge32(0)));
	add_col(new MibLeaf(colAgentxRegUpperBound, READONLY, new Gauge32(0)));
	add_col(new MibLeaf(colAgentxRegPriority, READONLY, new Gauge32(0)));
	add_col(new MibLeaf(colAgentxRegTimeout, READONLY, new SnmpInt32(0)));
	add_col(new MibLeaf(colAgentxRegInstance, READONLY, new SnmpInt32(0)));
}

agentxRegistrationEntry::~agentxRegistrationEntry()
{
}

void agentxRegistrationEntry::set_row(MibTableRow* r, const OctetStr& context, 
				      const Oidx& start, u_int range,
				      u_int _upper, u_int priority,
				      u_char timeout, int instanceReg)
{
	r->get_nth(0)->replace_value(new OctetStr(context));
	r->get_nth(1)->replace_value(new Oid(start));
	r->get_nth(2)->replace_value(new Gauge32(range));
	r->get_nth(3)->replace_value(new Gauge32(_upper));
	r->get_nth(4)->replace_value(new Gauge32(priority));
	r->get_nth(5)->replace_value(new SnmpInt32(timeout));
	r->get_nth(6)->replace_value(new SnmpInt32(instanceReg));
}

u_int agentxRegistrationEntry::add(u_int cid, u_int sid,
				   const OctetStr& context, 
				   const Oidx& start, u_int range,
				   u_int _upper, u_int priority,
				   u_char timeout, bool instanceReg)
TS_SYNCHRONIZED(
{
	Oidx ind;
	ind += cid;
	ind += sid;
	ind += next_reg++;
	set_row(add_row(ind), context, start, range, _upper, priority, timeout,
		((instanceReg) ? 1 : 2));
	return ind[2];
})

void agentxRegistrationEntry::remove(u_int cid, u_int sid, u_int rid)
TS_SYNCHRONIZED(
{
	Oidx ind;
	ind += cid;
	ind += sid;
	ind += rid;
	remove_row(ind);
})



agentx_mib::agentx_mib(): MibGroup("1.3.6.1.2.1.74")
{
	add(new MibLeaf(oidAgentxDefaultTimeout, READONLY, 
			new SnmpInt32(AGENTX_DEFAULT_TIMEOUT), 
			VMODE_DEFAULT));
	add(new MibLeaf(oidAgentxMasterAgentXVer, READONLY, new SnmpInt32(1)));

	TimeStamp* ts = 
	  new TimeStamp(oidAgentxConnTableLastChange,
			READONLY, VMODE_NONE);
	add(ts);
	add(new agentxConnectionEntry(ts));

	ts = new TimeStamp(oidAgentxSessionTableLastChange, 
			   READONLY, VMODE_NONE);	
	add(ts);
	add(new agentxSessionEntry(ts));
	ts = new TimeStamp(oidAgentxRegistrationTableLastChange, 
			   READONLY, VMODE_NONE);	
	add(ts);
	add(new agentxRegistrationEntry(ts));
}

#ifdef AGENTPP_NAMESPACE
}
#endif

