/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - mib_complex_entry.cpp  
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

#include <agent_pp/mib_complex_entry.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


/*------------------------ class MibComplexEntry ------------------------*/

MibComplexEntry::MibComplexEntry(const Oidx& o, mib_access a): MibEntry(o, a)
{
	upperBound = o.next_peer();
}

MibComplexEntry::MibComplexEntry(const MibComplexEntry& other): MibEntry(other)
{
	upperBound = other.upperBound;
}

MibComplexEntry::~MibComplexEntry()
{
}

/*------------------------ class MibStaticEntry ------------------------*/



/*------------------------ class MibStaticTable ------------------------*/

MibStaticTable::MibStaticTable(const Oidx& o): MibComplexEntry(o, NOACCESS)
{
}

MibStaticTable::MibStaticTable(MibStaticTable& other): 
  MibComplexEntry(other)
{
	OidListCursor<MibStaticEntry> cur;
	for (cur.init(&other.contents); cur.get(); cur.next()) {
		contents.add(new MibStaticEntry(*cur.get()));
	}
}

MibStaticTable::~MibStaticTable()
{
}

void MibStaticTable::add(const MibStaticEntry& entry) 
{
	start_synch();
	Oidx tmpoid(entry.get_oid());
	MibStaticEntry* newEntry = new MibStaticEntry(entry);
	// TODO: added here for backward compatibility, should be removed
	// later
	if (oid.is_root_of(tmpoid)) {
		tmpoid = tmpoid.cut_left(oid.len());
		newEntry->set_oid(tmpoid);
	}
	MibStaticEntry* ptr = contents.find(&tmpoid);
	if (ptr) {
		contents.remove(&tmpoid);
	}
	contents.add(newEntry);
	end_synch();
}

void MibStaticTable::remove(const Oidx& o) 
{
	start_synch();
	Oidx tmpoid(o);
	if (oid.is_root_of(tmpoid)) {
		tmpoid = tmpoid.cut_left(oid.len());
		contents.remove(&tmpoid);
	}
	end_synch();	
}

MibStaticEntry* MibStaticTable::get(const Oidx& o, bool suffixOnly) 
{
	Oidx tmpoid(o);
	if (!suffixOnly) {
		if (!oid.is_root_of(tmpoid))
			return 0;
		tmpoid = tmpoid.cut_left(oid.len());
	}
	return contents.find(&tmpoid);
}

Oidx MibStaticTable::find_succ(const Oidx& o, Request*)
{
	start_synch();
	Oidx tmpoid(o);
	Oidx retval;
	if (tmpoid <= oid) {
		tmpoid = Oidx();
	}
	else if (tmpoid.len() >= oid.len()) {
		tmpoid = tmpoid.cut_left(oid.len());
	}
	else {
		end_synch();
		return retval;
	}
	MibStaticEntry* ptr = contents.find_upper(&tmpoid);
	if ((ptr) && (*ptr->key() == tmpoid)) {
		ptr = contents.find_next(&tmpoid);
	}
	if (ptr) {
		retval = oid;
		retval += ptr->get_oid();
	}
	end_synch();
	return retval;
}

void MibStaticTable::get_request(Request* req, int ind)
{
	Oidx tmpoid(req->get_oid(ind));
	if (oid.is_root_of(tmpoid)) {
		tmpoid = tmpoid.cut_left(oid.len());
	}
	else {
		Vbx vb(req->get_oid(ind));
		vb.set_syntax(sNMP_SYNTAX_NOSUCHOBJECT);
		// error status (v1) will be set by RequestList
		req->finish(ind, vb); 
		return;
	}
	MibStaticEntry* entry = contents.find(&tmpoid);
	if (!entry) {
		Vbx vb(req->get_oid(ind));
		// TODO: This error status is just a guess, we cannot
		// determine exactly whether it is a noSuchInstance or
		// noSuchObject. May be a subclass could do a better 
		// job by knowing more details from the MIB structure?
		if (tmpoid.len() == 0) {
			vb.set_syntax(sNMP_SYNTAX_NOSUCHOBJECT);
		}
		else {
			Oidx columnID;
			columnID = tmpoid[0];
			entry = contents.find_upper(&columnID);
			if (entry)
				vb.set_syntax(sNMP_SYNTAX_NOSUCHINSTANCE);
			else
				vb.set_syntax(sNMP_SYNTAX_NOSUCHOBJECT);
		}
		// error status (v1) will be set by RequestList
		req->finish(ind, vb); 
	}
	else {
		Oidx id(oid);
		id += *entry->key();
		Vbx vb(*entry);
		vb.set_oid(id);
		req->finish(ind, vb);
	}
}

void MibStaticTable::get_next_request(Request* req, int ind)
{
	Oidx tmpoid(req->get_oid(ind));
	if (oid.is_root_of(tmpoid)) {
		tmpoid = tmpoid.cut_left(oid.len());
	}
	else {
		Vbx vb(req->get_oid(ind));
		vb.set_syntax(sNMP_SYNTAX_NOSUCHOBJECT);
		// error status (v1) will be set by RequestList
		req->finish(ind, vb); 
	}
	MibStaticEntry* entry = contents.find_upper(&tmpoid);
	if (!entry) {
		Vbx vb(req->get_oid(ind));
		// TODO: This error status is just a guess, we cannot
		// determine exactly whether it is a noSuchInstance or
		// noSuchObject. May be a subclass could do a better 
		// job by knowing more details from the MIB structure?
		vb.set_syntax(sNMP_SYNTAX_NOSUCHINSTANCE);
		// error status (v1) will be set by RequestList
		req->finish(ind, vb); 
	}
	else {
		Oidx id(oid);
		id += *entry->key();
		Vbx vb(*entry);
		vb.set_oid(id);
		req->finish(ind, vb);
	}
}

#ifdef AGENTPP_NAMESPACE
}
#endif

