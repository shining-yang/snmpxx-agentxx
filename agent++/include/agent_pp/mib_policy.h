/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - mib_policy.h  
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



#ifndef _mib_policy_h_
#define _mib_policy_h_

#include <agent_pp/agent++.h>
#include <agent_pp/mib.h>
#include <agent_pp/snmp_target_mib.h>
#include <agent_pp/snmp_notification_mib.h>
#include <agent_pp/v3_mib.h>
#include <agent_pp/mib_complex_entry.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

struct table_size_def {
	Oidx		tableID;
	unsigned int	sizeLimit;
	table_size_def(const Oidx& o, unsigned int i): tableID(o), sizeLimit(i)
	  { }
};

/*----------------------- class MibTablePolicy -------------------------*/

/**
 * The MibTableSizePolicy class implements a policy for MibTable
 * size limits. 
 * 
 * @author Frank Fock
 * @version 3.5
 */

class AGENTPP_DECL MibTableSizePolicy: public MibTableVoter {

 public:
	/**
	 * Create an empty MibTable size policy. This will accept any
	 * table sizes for any tables.
	 */
	MibTableSizePolicy(): defaultLimit(0) { }

	/**
	 * Create a MibTable size policy using the given size restriction
	 * array. Tables with an OID not listed in the array will be
	 * restricted to the given default size.
	 *
	 * @param tableSizeRestrictions
	 *    an array of table_size_def structures. Each entry defines
	 *    the maximum number of rows that can be created for the
	 *    specified table through SNMP requests.
	 * @param size
	 *    the size of the above table.
	 * @param defaultTableSize
	 *    the maximum table size for tables not explicitly listed
	 *    in the above array. The value 0 will disable any default
	 *    restriction.
	 */
	MibTableSizePolicy(const table_size_def*, unsigned int,
			   unsigned int);

        virtual ~MibTableSizePolicy() { }

	virtual int is_transition_ok(MibTable*, MibTableRow*, const Oidx&,
				     int, int);

	/**
	 * Set a MibTable size policy.
	 *
	 * @param tableOID
	 *    the OID of a MibTable object.
	 * @param maxNumberOfRows
	 *    the upper limit for the number of rows that may exists
	 *    in the specified table. If this limit is exceeded row creation
	 *    SNMP requests will be rejected with a 'resourceUnavail'
	 *    error.
	 */   
	virtual void set_policy(const Oidx&, unsigned int);

	/**
	 * Remove a size restriction from the policy table.
	 *
	 * @param tableOID
	 *    the OID of a MibTable object.
	 */
	virtual void remove_policy(const Oidx&);

	/**
	 * Register the given MibTableSizePolicy instance wiht all
	 * tables currently known by the given Mib instance.
	 *
	 * @param mib
	 *    a pointer to a Mib instance that has already tables
	 *    registered.
	 * @param sizePolicy
	 *    a pointer to a MibTableSizePolicy instance.
	 */
	static void register_policy(MibContext*, MibTableSizePolicy*);

 protected:
	OidList<MibStaticEntry> policies;
	unsigned int		defaultLimit;
};

#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
