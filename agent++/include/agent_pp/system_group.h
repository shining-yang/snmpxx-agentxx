/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - system_group.h  
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

#ifndef system_group_h_
#define system_group_h_

#ifndef WIN32
#include <sys/time.h>
#endif

#include <string.h>
#include <agent_pp/mib.h>
#include <agent_pp/snmp_textual_conventions.h>

#define oidSysGroup		"1.3.6.1.2.1.1"
#define oidSysDescr		"1.3.6.1.2.1.1.1.0"
#define oidSysObjectID		"1.3.6.1.2.1.1.2.0"
#define oidSysUpTime		"1.3.6.1.2.1.1.3.0"
#define oidSysContact		"1.3.6.1.2.1.1.4.0"
#define oidSysName		"1.3.6.1.2.1.1.5.0"
#define oidSysLocation		"1.3.6.1.2.1.1.6.0"
#define oidSysServices		"1.3.6.1.2.1.1.7.0"

#define confSysName		"sysName.leaf"
#define confSysContact		"sysContact.leaf"
#define confSysLocation		"sysLocation.leaf"
#define confSysORTable		"sysORTable.table"

#define oidSysORLastChange       "1.3.6.1.2.1.1.8.0"
#define oidSysORTable            "1.3.6.1.2.1.1.9"
#define oidSysOREntry            "1.3.6.1.2.1.1.9.1"
#define oidSysORIndex            "1.3.6.1.2.1.1.9.1.1"
#define colSysORIndex            "1"
#define oidSysORID               "1.3.6.1.2.1.1.9.1.2"
#define colSysORID               "2"
#define oidSysORDescr            "1.3.6.1.2.1.1.9.1.3"
#define colSysORDescr            "3"
#define oidSysORUpTime           "1.3.6.1.2.1.1.9.1.4"
#define colSysORUpTime           "4"

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


/**********************************************************************
 *
 *  class sysUpTime
 *
 **********************************************************************/

class AGENTPP_DECL sysUpTime: public MibLeaf {

public:
	sysUpTime();

	void		get_request(Request*, int);

	static u_int	get();
	static time_t	get_currentTime();

	virtual bool is_volatile() { return TRUE; }
#ifdef HAVE_CLOCK_GETTIME
        static struct timespec start;
#else        
	static u_int	start;
#endif        
};

/**********************************************************************
 *
 *  class sysDescr
 *
 **********************************************************************/

class AGENTPP_DECL sysDescr: public SnmpDisplayString {

public:
	sysDescr(const char*);
};

/**********************************************************************
 *
 *  class sysObjectID
 *
 **********************************************************************/

class AGENTPP_DECL sysObjectID: public MibLeaf {

public:
	sysObjectID(const Oidx&);
};

/**********************************************************************
 *
 *  class sysServices
 *
 **********************************************************************/

class AGENTPP_DECL sysServices: public MibLeaf {

public:
	sysServices(const int);
};


/**
 *  sysORLastChange
 *
"The value of sysUpTime at the time of the most recent
 change in state or value of any instance of sysORID."
 */


class AGENTPP_DECL sysORLastChange: public MibLeaf {

public:
	sysORLastChange();
	virtual ~sysORLastChange() {};

	static sysORLastChange* instance;

	virtual bool is_volatile() { return TRUE; }
	void       	update();
};

/**
 *  sysOREntry
 *
"An entry (conceptual row) in the sysORTable."
 */


class AGENTPP_DECL sysOREntry: public TimeStampTable {

public:
	sysOREntry(TimeStamp*);
	virtual ~sysOREntry();

	static sysOREntry* instance;

	virtual void       	set_row(MibTableRow* r,
					const Oidx&, const NS_SNMP OctetStr&, int);
	/**
	 * Check whether the receiver contains a row with a given sysORID.
	 *
	 * @param sysORID
	 *    an object identifier.
	 * @return
	 *    a pointer to the found MibTableRow instance, or 0 if such
	 *    an entry does not exist.
	 */
	virtual MibTableRow*    find(const Oidx&);

	virtual bool is_volatile() { return TRUE; }
};


/**********************************************************************
 *
 *  class sysGroup
 *
 **********************************************************************/

class AGENTPP_DECL sysGroup: public MibGroup {

public:

  /**
   * System group
   * Must values for the system group are: system descriptor, system object ID,
   * and the services of the system.
   * Optional values are system contact, system name and system location
   */
	sysGroup(const char *descr, const Oidx &o, const int services,
	         const char *contact = 0, const char *name = 0,
	         const char *location = 0);
};
#ifdef AGENTPP_NAMESPACE
}
#endif
#endif
