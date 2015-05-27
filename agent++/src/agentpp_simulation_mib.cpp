/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - agentpp_simulation_mib.cpp  
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

#include <agent_pp/agentpp_simulation_mib.h>
#include <agent_pp/sim_mib.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

/**
 *  agentppSimMode
 *
 */

agentppSimMode* agentppSimMode::instance = 0;

agentppSimMode::agentppSimMode():
   MibLeaf("1.3.6.1.4.1.4976.2.1.1.0", READWRITE, new SnmpInt32(1), TRUE)
{
	instance = this;
}

agentppSimMode::~agentppSimMode()
{
}

void agentppSimMode::init(SnmpSyntax* v, int m) 
{
	MibLeaf::init(v, m);
	if (value) {
		switch (get_state()) {
		case 1:
			SimMibLeaf::unset_config_mode();
			break;
		case 2:
			SimMibLeaf::set_config_mode();
			break;
		}
	}				
}


long agentppSimMode::get_state()
{
	return (long)*((SnmpInt32*)value);
}

void agentppSimMode::set_state(long l)
{
	*((SnmpInt32*)value) = l;
	switch (l) {
	case 1:
		SimMibLeaf::unset_config_mode();
		break;
	case 2:
		SimMibLeaf::set_config_mode();
		break;
	}
}

int agentppSimMode::set(const Vbx& vb)
{
	long l = 0;
	if (vb.get_value(l) != SNMP_CLASS_SUCCESS)
	    return SNMP_ERROR_WRONG_TYPE;
	switch (l) {
	case 1:
		SimMibLeaf::unset_config_mode();
		break;
	case 2:
		SimMibLeaf::set_config_mode();
		break;
	}
	return MibLeaf::set(vb);
}

bool agentppSimMode::value_ok(const Vbx& vb)
{
	long v;
	if (vb.get_value(v) != SNMP_CLASS_SUCCESS)
	    return FALSE;
	if ((v != 1) && (v != 2)) return FALSE;
	return TRUE;
}

/**
 *  agentppSimDeleteRow
 *
 */

agentppSimDeleteRow* agentppSimDeleteRow::instance = 0;

agentppSimDeleteRow::agentppSimDeleteRow(Mib* m):
   MibLeaf(oidAgentppSimDeleteRow, READWRITE, new Oid())
{
	// This leaf object is a singleton. In order to access it use
	// the static pointer agentppSimDeleteRow::instance.
	instance = this;
	mib = m;
}

agentppSimDeleteRow::~agentppSimDeleteRow()
{
}

int agentppSimDeleteRow::commit_set_request(Request* req, int ind)
{
	Oidx toid;
	Vbx vb(req->get_value(ind));
	vb.get_value(toid);
	
	MibEntryPtr entry = 0;
	int status = 
#ifdef _SNMPv3
	  mib->find_managing_object(mib->get_context(req->get_context()),
				    toid,
				    entry, req);
#else
	  mib->find_managing_object(mib->get_default_context(),
				    toid,
				    entry, req);
#endif	  
	if (status != SNMP_ERROR_SUCCESS) return SNMP_ERROR_WRONG_VALUE;
	if (entry->type() != AGENTPP_TABLE) return SNMP_ERROR_WRONG_VALUE;
	MibTable* table = (MibTable*)entry;

	table->start_synch();
	Oidx index = table->index(toid);
	table->remove_row(index);
	table->end_synch();

	return MibLeaf::commit_set_request(req, ind);
}

int agentppSimDeleteRow::prepare_set_request(Request* req, int& ind)
{
	int status;
	if ((status = MibLeaf::prepare_set_request(req, ind)) !=
	    SNMP_ERROR_SUCCESS) return status;
	Oidx toid;
	Vbx vb(req->get_value(ind));
	if (vb.get_value(toid) != SNMP_CLASS_SUCCESS)
	    return SNMP_ERROR_WRONG_TYPE;
	MibEntryPtr entry = 0;
	status = 
#ifdef _SNMPv3
	  mib->find_managing_object(mib->get_context(req->get_context()),
				    toid,
				    entry, req);
#else
	  mib->find_managing_object(mib->get_default_context(),
				    toid,
				    entry, req);
#endif	  
	if (status != SNMP_ERROR_SUCCESS) return SNMP_ERROR_WRONG_VALUE;
	if (entry->type() != AGENTPP_TABLE) return SNMP_ERROR_WRONG_VALUE;
	MibTable* table = (MibTable*)entry;
	Oidx index = table->index(toid);
	MibTableRow* r = table->find_index(index);
	if (!r) return SNMP_ERROR_WRONG_VALUE;

	return SNMP_ERROR_SUCCESS;
}



/**
 *  agentppSimDeleteTableContents
 *
 */

agentppSimDeleteTableContents* agentppSimDeleteTableContents::instance = 0;

agentppSimDeleteTableContents::agentppSimDeleteTableContents(Mib* m):
   MibLeaf(oidAgentppSimDeleteTableContents, READWRITE, new Oid())
{
	instance = this;
	mib = m;
}

agentppSimDeleteTableContents::~agentppSimDeleteTableContents()
{
}

int agentppSimDeleteTableContents::commit_set_request(Request* req, int ind)
{
	Oidx toid;
	Vbx vb(req->get_value(ind));
	vb.get_value(toid);
	MibEntryPtr entry = 0;
	int status = 
#ifdef _SNMPv3
	  mib->find_managing_object(mib->get_context(req->get_context()),
				    toid,
				    entry, req);
#else
	  mib->find_managing_object(mib->get_default_context(),
				    toid,
				    entry, req);
#endif	  
	if (status != SNMP_ERROR_SUCCESS) return SNMP_ERROR_WRONG_VALUE;
	if (entry->type() != AGENTPP_TABLE) return SNMP_ERROR_WRONG_VALUE;
	
	MibTable* table = (MibTable*)entry;
	entry->start_synch();
	table->clear();
	entry->end_synch();
	
	return MibLeaf::commit_set_request(req, ind);
}


int agentppSimDeleteTableContents::prepare_set_request(Request* req, int& ind)
{
	int status;
	if ((status = MibLeaf::prepare_set_request(req, ind)) !=
	    SNMP_ERROR_SUCCESS) return status;

	Oidx toid;
	Vbx vb(req->get_value(ind));
	if (vb.get_value(toid) != SNMP_CLASS_SUCCESS)
	    return SNMP_ERROR_WRONG_TYPE;
	MibEntryPtr entry = 0;
	status = 
#ifdef _SNMPv3
	  mib->find_managing_object(mib->get_context(req->get_context()),
				    toid,
				    entry, req);
#else
	  mib->find_managing_object(mib->get_default_context(),
				    toid,
				    entry, req);
#endif	  
	if (status != SNMP_ERROR_SUCCESS) return SNMP_ERROR_WRONG_VALUE;
	if (entry->type() != AGENTPP_TABLE) return SNMP_ERROR_WRONG_VALUE;

	return SNMP_ERROR_SUCCESS;
}


agentpp_simulation_mib::agentpp_simulation_mib(Mib* mib): 
  MibGroup("1.3.6.1.4.1.4976.2.1", "agentppSimMIB")
{
	add(new agentppSimMode());
        add(new agentppSimDeleteRow(mib));
        add(new agentppSimDeleteTableContents(mib));
}

agentpp_simulation_mib::agentpp_simulation_mib(): 
  MibGroup("1.3.6.1.4.1.4976.2.1", "agentppSimMIB")
{
	add(new agentppSimMode());
}

#ifdef AGENTPP_NAMESPACE
}
#endif











