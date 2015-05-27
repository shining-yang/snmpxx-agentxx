/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - system_group.cpp  
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
#include <agent_pp/system_group.h>

#ifdef WIN32
#ifdef __BCPLUSPLUS__
#define _timeb timeb
#define _ftime ftime
#endif
#endif

#ifdef AGENTPP_NAMESPACE
using namespace Agentpp;
#endif


/**********************************************************************
 *
 *  class sysUpTime
 *
 **********************************************************************/

#if defined(HAVE_CLOCK_GETTIME)
struct timespec sysUpTime::start = { 0, 0 };
#else
u_int sysUpTime::start = 0;
#endif


sysUpTime::sysUpTime(): MibLeaf(oidSysUpTime, READONLY, new TimeTicks(0))
{
#if defined(HAVE_CLOCK_GETTIME)
        clock_gettime(CLOCK_MONOTONIC, &start);
#else
	start = get();
#endif        
}

time_t sysUpTime::get_currentTime()
{
	time_t now;
	time(&now);
	return now;
}

u_int sysUpTime::get()
{
#ifdef HAVE_CLOCK_GETTIME
        struct timespec tsp;
        clock_gettime(CLOCK_MONOTONIC, &tsp);
        return (((tsp.tv_sec - start.tv_sec) * 100) + 
                ((tsp.tv_nsec - start.tv_nsec) / 10000000)) % MAXUINT32;
#else                
#ifndef _WIN32
	struct timeval ct;
	gettimeofday(&ct, 0);
	return (get_currentTime()*100 + ct.tv_usec/10000) - start;
#else
        struct _timeb tstruct;
        _ftime(&tstruct);
		return ((get_currentTime()*100 + tstruct.millitm/10) - start) % MAXUINT32;
#endif
#endif        
}

void sysUpTime::get_request(Request* req, int ind)
{
	*((TimeTicks*)value) = (unsigned long)get();
	MibLeaf::get_request(req, ind);
}


/**********************************************************************
 *
 *  class sysDescr
 *
 **********************************************************************/

sysDescr::sysDescr(const char* str): SnmpDisplayString(oidSysDescr, READONLY,
						       new OctetStr(str)) { }

/**********************************************************************
 *
 *  class sysObjectID
 *
 **********************************************************************/

sysObjectID::sysObjectID(const Oidx& o): MibLeaf(oidSysObjectID, READONLY,
						 new Oid(o)) { }

/**********************************************************************
 *
 *  class sysServices
 *
 **********************************************************************/

sysServices::sysServices(const int s): MibLeaf(oidSysServices, READONLY,
					       new SnmpInt32(s)) { }


/**
 *  sysOREntry
 *
 */

sysOREntry* sysOREntry::instance = 0;

const index_info indSysOREntry[1] = {
	{ sNMP_SYNTAX_INT, FALSE, 1, 1 } };

sysOREntry::sysOREntry(TimeStamp* lc):
   TimeStampTable(oidSysOREntry, indSysOREntry, 1, lc)
{
	// This table object is a singleton. In order to access it use
	// the static pointer sysOREntry::instance.
	instance = this;

	add_col(new MibLeaf(colSysORID, READONLY, new Oidx()));
	add_col(new MibLeaf(colSysORDescr, READONLY, new OctetStr()));
	add_col(new MibLeaf(colSysORUpTime, READONLY, new TimeTicks()));
}

sysOREntry::~sysOREntry()
{
}

MibTableRow* sysOREntry::find(const Oidx& id)
{
	OidListCursor<MibTableRow> cur;
	for (cur.init(&content); cur.get(); cur.next()) {
		Oidx other;
		cur.get()->get_nth(0)->get_value(other);
		if (other == id) return cur.get();
	}
	return 0;
}

void sysOREntry::set_row(MibTableRow* r, const Oidx& id,
			 const OctetStr& descr, int lastUpdate)
{
	r->get_nth(0)->replace_value(new Oid(id));
	r->get_nth(1)->replace_value(new OctetStr(descr));
	r->get_nth(2)->replace_value(new TimeTicks(lastUpdate));
	updated();
}


/**********************************************************************
 *
 *  class sysGroup
 *
 **********************************************************************/

sysGroup::sysGroup(const char *descr, const Oidx &o, const int services,
                   const char *contact, const char *name,
                   const char *location):
  MibGroup(oidSysGroup, "systemGroup")
{
	add(new sysDescr(descr));
	add(new sysObjectID(o));
	add(new sysUpTime());
	add(new SnmpDisplayString(oidSysContact, READWRITE,
	                          new OctetStr(contact ? contact : "")));
	add(new SnmpDisplayString(oidSysName, READWRITE,
	                          new OctetStr(name ? name : "")));
	add(new SnmpDisplayString(oidSysLocation, READWRITE,
	                          new OctetStr(location ? location : "")));
	add(new sysServices(services));
	TimeStamp* ptr = new TimeStamp(oidSysORLastChange, READONLY,
				       VMODE_NONE);
	add(ptr);
	add(new sysOREntry(ptr));
}

