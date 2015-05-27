/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - mib_policy.cpp  
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

#include <agent_pp/mib_policy.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

MibTableSizePolicy::MibTableSizePolicy(const table_size_def* r,
				       unsigned int sz,
				       unsigned int def): 
  defaultLimit(def)
{
	for (unsigned int i=0; i<sz; i++) {		
		MibStaticEntry* e = 
		  new MibStaticEntry(r[i].tableID,
				     Gauge32(r[i].sizeLimit));
		policies.add(e);
	}
}

void MibTableSizePolicy::set_policy(const Oidx& tid, unsigned int l)
{
	Oidx id(tid);
	MibStaticEntry* e = policies.find(&id);
	if (e) {
		e->set_value(Gauge32(l));
	}
	else {
		policies.add(new MibStaticEntry(id, Gauge32(l)));
	}
}

void MibTableSizePolicy::remove_policy(const Oidx& tid) 
{
	Oidx id(tid);
	policies.remove(&id);
}

int MibTableSizePolicy::is_transition_ok(MibTable* t, 
					 MibTableRow*,
					 const Oidx&,
					 int curState,
					 int newState)
{
	if ((curState == rowEmpty) && 
	    (newState != rowDestroy)) {
		MibStaticEntry* e = policies.find(t->key());
		int limit = defaultLimit;
		if (e) {
			Gauge32 g;
			e->get_value(g);
			limit = (int)g;
		}
		if ((limit > 0) && (t->size() >= limit)) 
			return SNMP_ERROR_RESOURCE_UNAVAIL;
		return SNMP_ERROR_SUCCESS;
	}
	return SNMP_ERROR_SUCCESS;
}

void MibTableSizePolicy::register_policy(MibContext* context, 
					 MibTableSizePolicy* p)
{
	OidListCursor<MibEntry> cur(context->get_content());
	for (; cur.get(); cur.next()) {
		if (cur.get()->type() == AGENTPP_TABLE) {
			if (p->defaultLimit > 0) {
				((MibTable*)cur.get())->add_voter(p);
			}
			else if (p->policies.find(cur.get()->key())) {
				((MibTable*)cur.get())->add_voter(p);
			}
		}
	}
}


#ifdef AGENTPP_NAMESPACE
}
#endif



