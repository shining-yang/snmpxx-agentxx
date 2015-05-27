/*_############################################################################
  _## 
  _##  agentx_index.cpp  
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
#include <agentx_pp/agentx_index.h>

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

// static const char *loggerModuleName = "agentx++.agentx_index";

/*--------------------- class AgentXIndex -----------------------*/

AgentXIndex::AgentXIndex(const Vbx& v): MibStaticEntry(v) 
{
}

AgentXIndex::AgentXIndex(AgentXIndex& other): MibStaticEntry(other)
{
	OidListCursor<AgentXIndexEntry> cur;
	for (cur.init(&other.allocatedValues); cur.get(); cur.next()) {
		allocatedValues.add(new AgentXIndexEntry(*cur.get()));
	}
}

AgentXIndex::~AgentXIndex()
{
}

Oidx* AgentXIndex::get_index_value(const Vbx& ind) 
{
	Oidx* newIndex = new Oidx();

	switch (ind.get_syntax()) {
	case sNMP_SYNTAX_INT32: {
		long l = 0;
		ind.get_value(l); 
		if (l < 0) return 0;
		*newIndex = (unsigned long)l;
		break;
	}
	case sNMP_SYNTAX_TIMETICKS:
	case sNMP_SYNTAX_GAUGE32: {
		unsigned long l = 0;
		ind.get_value(l);
		*newIndex = l;
		break;
	}
	case sNMP_SYNTAX_OCTETS: 
	case sNMP_SYNTAX_BITS:
	case sNMP_SYNTAX_OPAQUE: {
		OctetStr v;
		ind.get_value(v);
	  	*newIndex = Oidx::from_string(v, FALSE);
		break;
	}
	case sNMP_SYNTAX_IPADDR: {
		IpAddress v;
		ind.get_value(v);
	  	*newIndex += v;
		break;
	}
	case sNMP_SYNTAX_OID: {
		ind.get_value(*newIndex);
		break;
	}
	default:
		return 0;
	}
	return newIndex;
}

int AgentXIndex::allocate(u_int sid, const Vbx& ind, bool test) 
{
	if (test) {
		if (ind.get_syntax() != get_syntax())
			return AGENTX_INDEX_WRONG_TYPE;
		Oidx* newIndex = get_index_value(ind);
		if (!newIndex)
			return AGENTX_INDEX_WRONG_TYPE;
		// syntax must be one of SnmpInt32, SnmpUInt32, 
		// Oidx, or OctetStr
		if (allocatedValues.find(newIndex)) {
			delete newIndex;
			return AGENTX_INDEX_ALREADY_ALLOCATED;
		}
		delete newIndex;
		return AGENTX_OK;
	}
	else {
		Oidx* newIndex = get_index_value(ind);
		if (newIndex) {
			SnmpSyntax* s = ind.clone_value();
			allocatedValues.add(
			       new AgentXIndexEntry(sid, *newIndex, *s)); 
			delete s;
		}
		delete newIndex;
		return AGENTX_OK;
	}
}

bool AgentXIndex::is_simple_syntax()
{
	switch (get_syntax()) {
	case sNMP_SYNTAX_INT32:
	case sNMP_SYNTAX_TIMETICKS:
	case sNMP_SYNTAX_GAUGE32: 
		return TRUE;
	}	
	return FALSE;
}

int AgentXIndex::release(u_int sid, const Vbx& ind, bool test) 
{
	Oidx* newIndex = get_index_value(ind);
	AgentXIndexEntry* i = allocatedValues.find(newIndex); 
	if ((!i) || (i->get_session_id() != sid)) {
		delete newIndex;
		return AGENTX_INDEX_NOT_ALLOCATED;
	}
	else {
		if (!test) {
			allocatedValues.remove(i);
			if (is_simple_syntax()) usedValues.add(i);
			else delete i;
		}
		delete newIndex;
	}
	return AGENTX_OK;
}


int AgentXIndex::new_index(u_int sid, Vbx& ind, bool test)
{
	if (!is_simple_syntax()) return AGENTX_INDEX_NONE_AVAILABLE;
	if (test) {
		unsigned long next = 1;
		MibStaticEntry* e = allocatedValues.last();
		if (e) {
			next = e->get_oid()[0];
			next++;
		}
		Oidx nextOID;
		nextOID += next;
		while (usedValues.find(&nextOID)) {
			next++;
			nextOID = "";
			nextOID += next;
		}
		switch (get_syntax()) {
		case sNMP_SYNTAX_INT32: {
			long l = (long)next;
			ind.set_value(l);
			break;
		}
		case sNMP_SYNTAX_TIMETICKS:
		case sNMP_SYNTAX_GAUGE32: {
			ind.set_value(next);
			break;
		}
		}
	}
	Oidx* newIndex = get_index_value(ind);
	if (!newIndex) return AGENTX_INDEX_NONE_AVAILABLE;
	if (!test) {
		SnmpSyntax* s = ind.clone_value();
		allocatedValues.add(new AgentXIndexEntry(sid, *newIndex, *s)); 
		delete s;
	}
	delete newIndex;
	return AGENTX_OK;
}

int AgentXIndex::any_index(u_int sid, Vbx& ind, bool test)
{
	if (!is_simple_syntax()) return AGENTX_INDEX_NONE_AVAILABLE;
	if (test) {
		unsigned long next = 1;
		MibStaticEntry* e = 0;
		if (usedValues.size()>0) {
			e = usedValues.first();
			next = e->get_oid()[0];
		}
		else {
			e = allocatedValues.last();
			if (e) {
				next = e->get_oid()[0];
				next++;
			} 
		}
		switch (get_syntax()) {
		case sNMP_SYNTAX_INT32: {
		        long l = next;
			ind.set_value(l);
			break;
		}
		case sNMP_SYNTAX_TIMETICKS:
		case sNMP_SYNTAX_GAUGE32: {
			ind.set_value(next);
			break;
		}
		default: {
			return AGENTX_INDEX_NONE_AVAILABLE;
		}
		}
	}
	Oidx* newIndex = get_index_value(ind);
	if (!newIndex) return AGENTX_INDEX_NONE_AVAILABLE;
	if (!test) {
		if (usedValues.size() > 0) {
			AgentXIndexEntry* e = usedValues.first();
			usedValues.remove(e);
			delete e;
		}
		SnmpSyntax* s = ind.clone_value();
		allocatedValues.add(new AgentXIndexEntry(sid, *newIndex, *s));
		delete s;
	}
	delete newIndex;
	return AGENTX_OK;
}

void AgentXIndex::remove_session(u_int sid) 
{
	OidListCursor<AgentXIndexEntry> cur;
	List<Oidx> victims;
	for (cur.init(&allocatedValues); cur.get(); cur.next()) {
		if (cur.get()->get_session_id() == sid)
			victims.add(new Oidx(*cur.get()->key()));
	}
	ListCursor<Oidx> cv;
	for (cv.init(&victims); cv.get(); cv.next()) {
		allocatedValues.remove(cv.get());
	}
}


/*--------------------- class AgentXIndexDB -----------------------*/


AgentXIndexDB::~AgentXIndexDB()
{
}

bool AgentXIndexDB::is_valid_syntax(SmiUINT32 s) 
{
	switch (s) {
	case sNMP_SYNTAX_INT32:
	case sNMP_SYNTAX_TIMETICKS:
	case sNMP_SYNTAX_GAUGE32: 
	case sNMP_SYNTAX_OCTETS: 
	case sNMP_SYNTAX_BITS:
	case sNMP_SYNTAX_OPAQUE: 
	case sNMP_SYNTAX_IPADDR: 
	case sNMP_SYNTAX_OID: 
		return TRUE;
	default:
		return FALSE;
	}
}

int AgentXIndexDB::allocate(u_int sid, const Vbx& ind, bool test) 
{
	Lock l(*this);
	Oidx o(ind.get_oid());
	AgentXIndex* i = indexes.find(&o);
	if (!i) {
		if (test)
			return (is_valid_syntax(ind.get_syntax())) ?
			  AGENTX_OK : AGENTX_INDEX_WRONG_TYPE;
		else
			return indexes.add(new AgentXIndex(ind))->
			  allocate(sid, ind, test);
	}
	return i->allocate(sid, ind, test);
}

int AgentXIndexDB::release(u_int sid, const Vbx& ind, bool test) 
{
	Lock l(*this);
	Oidx o(ind.get_oid());
	AgentXIndex* i = indexes.find(&o);
	if (!i) return AGENTX_INDEX_NOT_ALLOCATED;
	return i->release(sid, ind, test);
}


int AgentXIndexDB::new_index(u_int sid, Vbx& ind, bool test)
{
	Lock l(*this);
	Oidx o(ind.get_oid());
	AgentXIndex* i = indexes.find(&o);
	if (!i) {
		if (test)
			if (is_valid_syntax(ind.get_syntax())) { 
				AgentXIndex index(ind);
				return index.new_index(sid, ind, test);
			}
			else return AGENTX_INDEX_WRONG_TYPE;
		else
			return indexes.add(new AgentXIndex(ind))->
			  new_index(sid, ind, test);
	}
	else {
		return i->new_index(sid, ind, test);
	}
}

int AgentXIndexDB::any_index(u_int sid, Vbx& ind, bool test)
{
	Lock l(*this);
	Oidx o(ind.get_oid());
	AgentXIndex* i = indexes.find(&o);
	if (!i) {
		if (test)
			if (is_valid_syntax(ind.get_syntax())) { 
				AgentXIndex index(ind);
				return index.any_index(sid, ind, test);
			}
			else return AGENTX_INDEX_WRONG_TYPE;
		else
			return indexes.add(new AgentXIndex(ind))->
			  any_index(sid, ind, test);
	}
	else {
		return i->any_index(sid, ind, test);
	}
}

void AgentXIndexDB::remove_session(u_int sid) 
{
	Lock l(*this);
	OidListCursor<AgentXIndex> cur;
	for (cur.init(&indexes); cur.get(); cur.next()) {
		cur.get()->remove_session(sid);
	}
}

#ifdef AGENTPP_NAMESPACE
}
#endif

