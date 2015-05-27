/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - agentpp_simulation_mib.h  
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


#ifndef _agentpp_simulation_mib_h
#define _agentpp_simulation_mib_h

#include <agent_pp/agent++.h>
#include <agent_pp/mib.h>
#include <agent_pp/snmp_textual_conventions.h>

#define oidAgentppSimMode                "1.3.6.1.4.1.4976.2.1.1.0"
#define oidAgentppSimDeleteRow           "1.3.6.1.4.1.4976.2.1.2.0"
#define oidAgentppSimDeleteTableContents "1.3.6.1.4.1.4976.2.1.3.0"

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

/**
 *  agentppSimMode
 *
"Setting this object to config(2) enables the
 configuration mode of all simulated MIB objects.
 Each such object instance can then be written or
 created, even though its MAX-ACCESS rights are 
 read-only or read-write."
 */


class AGENTPP_DECL agentppSimMode: public MibLeaf {

 public:
	agentppSimMode();
	virtual ~agentppSimMode();

	static agentppSimMode* instance;

	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual bool    	value_ok(const Vbx&);
	virtual void        	init(NS_SNMP SnmpSyntax*, int);
};

/**
 *  agentppSimDeleteRow
 *
"Setting this object to an object identifier of
 an arbitrary instance of a table's row, will 
 remove the row from the target table. If the 
 object identifier does not denote such a row,
 wrongValue is returned. 
 For example if your table has the oid 1.3.6.1.3.1
 and the the index of that table is an integer,
 then you will have to provide the following oid
 in order to delete the row with index 100: 
 1.3.6.1.3.1.1.1.100"
 */


class AGENTPP_DECL agentppSimDeleteRow: public MibLeaf {

public:
        agentppSimDeleteRow(Mib*);
        virtual ~agentppSimDeleteRow();

        static agentppSimDeleteRow* instance;

        virtual int             commit_set_request(Request*, int);
        virtual int             prepare_set_request(Request*, int&);
 protected:
	Mib*	mib;
};


/**
 *  agentppSimDeleteTableContents
 *
"Setting this object to an object identifier of
 a table will delete all rows of that table. If
 the given object identifier does not denote a
 table known by the simulation agent, 
 wrongValue is returned."
 */


class AGENTPP_DECL agentppSimDeleteTableContents: public MibLeaf {

public:
        agentppSimDeleteTableContents(Mib*);
        virtual ~agentppSimDeleteTableContents();

        static agentppSimDeleteTableContents* instance;

        virtual int             commit_set_request(Request*, int);
        virtual int             prepare_set_request(Request*, int&);
 protected:
	Mib*	mib;
};


class AGENTPP_DECL agentpp_simulation_mib: public MibGroup
{
  public:
	agentpp_simulation_mib(Mib*);
	agentpp_simulation_mib();
	virtual ~agentpp_simulation_mib() { }
};


#ifdef AGENTPP_NAMESPACE
}
#endif


#endif


