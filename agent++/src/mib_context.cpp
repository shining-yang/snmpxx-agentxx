/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - mib_context.cpp  
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

#include <agent_pp/mib_context.h>
#include <agent_pp/tools.h>
#include <snmp_pp/log.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

static const char *loggerModuleName = "agent++.mib_context";

/*--------------------------- class MibGroup --------------------------*/

MibGroup::MibGroup(const Oidx& o): MibEntry(o, NOACCESS)
{
	persistencyName = 0;
	timeout = 0;
}

MibGroup::MibGroup(const Oidx& o, const OctetStr& p): MibEntry(o, NOACCESS)
{
	persistencyName = new OctetStr(p);
	timeout = 0;
}

MibGroup::~MibGroup()
{
	if (persistencyName) delete persistencyName;
	content.clear();
}

mib_type MibGroup::type() const
{
	return AGENTPP_GROUP;
}

MibEntryPtr MibGroup::add(MibEntryPtr item)
{
	if ((item->type() == AGENTPP_LEAF) &&
	    (item->get_access() == NOACCESS)) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("MibGroup: cannot add not-accessible MIB object (oid) to (group)");
		LOG(item->key()->get_printable());
		LOG(oid.get_printable());
		LOG_END;
		return 0;
	}
	/* This has been removed in v3.5.15 because it had more drawbacks than
	   use.
	// check if item is in subtree of this group
	// if not we assume that the oid of item is a sub id to this group
	if (!(item->key()->in_subtree_of(oid))) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("MibGroup: cannot add MIB object with (oid) to (group)");
		LOG(item->key()->get_printable());
		LOG(oid.get_printable());
		LOG_END;
		return 0;
	}
	*/
	if (item->type() == AGENTPP_GROUP) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("MibGroup: cannot add a MibGroup to a MibGroup");
		LOG_END;
		return 0;
	}

	/* check if the oid already exists */
    ListCursor<MibEntry> cur;
    for (cur.init(&content); cur.get(); cur.next()) {
        if (*cur.get()->key() == *item->key()) {
            LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
            LOG("MibGroup: cannot add MIB object (oid) to (group) twice");
            LOG(item->key()->get_printable());
            LOG(oid.get_printable());
            LOG_END;
            return 0;
        }
    }

	return content.add(item);
}

void MibGroup::remove(const Oidx& o)
{
	ListCursor<MibEntry> cur;
	for (cur.init(&content); cur.get(); cur.next()) {
		if (*cur.get()->key() == o) {
			delete content.remove(cur.get());
			break;
		}
	}
}


ListCursor<MibEntry> MibGroup::get_content()
{
	return ListCursor<MibEntry>(&content);
}

void MibGroup::clearAll()
{
	content.clearAll();
}

void MibGroup::load_from_file(const char* fname)
{
	FILE *f;
	char *buf;
	long size, bytes;
	char header[16];

	if ((f = fopen(fname, "rb")) == 0)
		return;

	size  = AgentTools::file_size(f);
	int n = 0;
	ListCursor<MibEntry> cur;
	for (cur.init(&content); ((cur.get()) && (size>0)); cur.next(), n++) {

		// skip volatile objects
		if (cur.get()->is_volatile()) continue;

		// read ASN.1 sequence header
		bytes = fread(header, sizeof(char), 2, f);
		if (bytes != 2) {
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			LOG("MibGroup: loading persistent data failed, cannot read length (file)(oid)(size)");
			LOG(fname);
			LOG(cur.get()->key()->get_printable());
			LOG(size);
			LOG_END;
			fclose(f);
			return;
		}
		bytes = 0;
		char hlen = header[1];
		int sz = size; // sz must be total amount of data readable
		if (hlen & ASN_LONG_LEN) {
			hlen &= ~ASN_LONG_LEN;	/* turn MSb off */
			bytes = fread(header+2, sizeof(char), hlen, f);
		}
		else hlen = 0;
		unsigned char type = 0;
		asn_parse_header((unsigned char*)header, &sz, &type);
		if ((bytes != hlen) ||
		    (type != (unsigned char)(ASN_SEQUENCE | ASN_CONSTRUCTOR))){
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			LOG("MibGroup: loading from file failed (file)(oid)(n)(header)(type)");
			LOG(fname);
			LOG(cur.get()->key()->get_printable());
			LOG(n);
			LOG(OctetStr((unsigned char*)header, bytes).get_printable());
			LOG(type);
			LOG_END;
			fclose(f);
			return;
		}
		buf   = new char[sz+16];
		memcpy(buf, header, 2+hlen);
		char* cp = buf+2+hlen;
		bytes = fread(cp, sizeof(char), sz, f);
		if (bytes != sz) {
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			LOG("MibGroup: loading from file failed (file)(oid)(n)(read)(size)");
			LOG(fname);
			LOG(cur.get()->key()->get_printable());
			LOG(n);
			LOG(bytes);
			LOG(sz);
			LOG_END;
			delete [] buf;
			fclose(f);
			return;
		}
		int len = sz+2+hlen;
		while ((cur.get()) && (!cur.get()->deserialize(buf, len))) {
			cur.next();
		}
		delete [] buf;
	}
	fclose(f);
}

void MibGroup::save_to_file(const char* fname)
{
	FILE *f;
	char *buf = 0;
	int bytes = 0;

	if ((f = fopen(fname, "wb")) == 0) {
		return;
	}
	ListCursor<MibEntry> cur;
	for (cur.init(&content); cur.get(); cur.next()) {
		// skip volatile objects
		if (cur.get()->is_volatile()) continue;

		if ((cur.get()->serialize(buf, bytes)) && (buf)) {
			fwrite(buf, sizeof(char), bytes, f);
			delete [] buf;
			buf = 0;
		}
	}
      	fclose(f);
}



/*--------------------------- class MibContext --------------------------*/


MibContext::MibContext()
{
	context = "";
	contextKey = Oidx::from_string(context);
	persistencyPath = 0;
}

MibContext::MibContext(const OctetStr& c)
{
	context = c;
	contextKey = Oidx::from_string(c);
	persistencyPath = 0;
}

MibContext::~MibContext()
TS_SYNCHRONIZED(
{
	if (persistencyPath) {
		save_to(*persistencyPath);
		delete persistencyPath;
	}
})

bool MibContext::init_from(const OctetStr& p)
{
	if (persistencyPath)
		delete persistencyPath;
	persistencyPath = new OctetStr(p);
	OidListCursor<MibGroup> cur;
	for (cur.init(&groups); cur.get(); cur.next()) {
		if (cur.get()->is_persistent()) {
			OctetStr path(*persistencyPath);
			path += cur.get()->get_persistency_name();
			path += ".";
			path += context;
			cur.get()->load_from_file(path.get_printable());
		}
	}
	return TRUE;
}

bool MibContext::load_from(const OctetStr& p)
{
	OidListCursor<MibGroup> cur;
	for (cur.init(&groups); cur.get(); cur.next()) {
		if (cur.get()->is_persistent()) {
			// clear old data
			ListCursor<MibEntry> entries =
			    cur.get()->get_content();
			for (;entries.get(); entries.next()) {
			    entries.get()->start_synch();
			    entries.get()->reset();
			}
			// load data from persistent storage
			OctetStr path(p);
			path += cur.get()->get_persistency_name();
			path += ".";
			path += context;
			cur.get()->load_from_file(path.get_printable());

			entries = cur.get()->get_content();
			for (;entries.get(); entries.next()) {
			    entries.get()->end_synch();
			}
		}
	}
	return TRUE;
}

bool MibContext::save_to(const OctetStr& p)
{
	OidListCursor<MibGroup> cur;
	for (cur.init(&groups); cur.get(); cur.next()) {
		if (cur.get()->is_persistent()) {
			OctetStr path(p);
			path += cur.get()->get_persistency_name();
			path += ".";
			path += context;
			cur.get()->save_to_file(path.get_printable());
		}
	}
	return TRUE;
}

OidxPtr MibContext::key()
{
	return &contextKey;
}

int MibContext::find(const Oidx& oid, MibEntryPtr& entry)
TS_SYNCHRONIZED(
{
	Oidx tmpoid(oid);
	MibEntry* e = content.find(&tmpoid);
	if (!e) return sNMP_SYNTAX_NOSUCHOBJECT;
	entry = e;
	return SNMP_ERROR_SUCCESS;
})

MibEntry* MibContext::seek(const Oidx& oid)
TS_SYNCHRONIZED(
{
	Oidx tmpoid(oid);
	return content.seek(&tmpoid);
})


int MibContext::find_lower(const Oidx& oid, MibEntryPtr& entry)
TS_SYNCHRONIZED(
{
	Oidx tmpoid(oid);
	MibEntry* e = content.find_lower(&tmpoid);
	if (!e) return sNMP_SYNTAX_NOSUCHOBJECT;
	entry = e;
	return SNMP_ERROR_SUCCESS;
})

int MibContext::find_upper(const Oidx& oid, MibEntryPtr& entry)
TS_SYNCHRONIZED(
{
	Oidx tmpoid(oid);
	MibEntry* e = content.find_upper(&tmpoid);
	if (!e) return sNMP_SYNTAX_NOSUCHOBJECT;
	entry = e;
	return SNMP_ERROR_SUCCESS;
})

MibEntry* MibContext::find_next(const Oidx& oid)
TS_SYNCHRONIZED(
{
	Oidx tmpoid(oid);
	return content.find_next(&tmpoid);
})

OidListCursor<MibEntry>  MibContext::get_content()
{
	return OidListCursor<MibEntry>(&content);
}

OidListCursor<MibGroup>  MibContext::get_groups()
{
	return OidListCursor<MibGroup>(&groups);
}

MibEntry* MibContext::add(MibEntry* item)
{
    ThreadSynchronize _ts_synchronize(*this);
	if ((item->type() == AGENTPP_LEAF) &&
	    (item->get_access() == NOACCESS)) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("MibContext: cannot add not-accessible MIB object (oid) to (context)");
		LOG(item->key()->get_printable());
		LOG(context.get_printable());
		LOG_END;
		return 0;
	}
	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 3);
	LOG("MibContext: adding MIB object (context)(oid)");
	LOG(context.get_printable());
	LOG(item->key()->get_printable());
	LOG_END;

	MibEntryPtr e = content.find(item->key());
	if (e) {
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
		LOG("MibContext: duplicate registration (context)(oid)");
		LOG(context.get_printable());
		LOG(item->key()->get_printable());
		LOG_END;
		return 0;
	}

	if (item->type() == AGENTPP_GROUP) {
	    MibGroup *mg = (MibGroup*)item;
	    if (groups.find(mg->key()))
	    {
	        LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
	        LOG("MibContext: duplicate MibGroup registration (context)(oid)");
	        LOG(context.get_printable());
	        LOG(item->key()->get_printable());
	        LOG_END;
	        return 0;
	    }
		ListCursor<MibEntry> cur(mg->get_content());
		for (; cur.get(); cur.next())
			content.add(cur.get());
		groups.add(mg);
	}
	else {
		return content.add(item);
	}
	return item;
}

MibEntry* MibContext::remove(const Oidx& oid)
TS_SYNCHRONIZED(
{
	Oidx tmpoid(oid);
	MibEntry* victim = content.find(&tmpoid);
	if (victim) return content.remove(victim);
	return 0;
})


MibEntry* MibContext::get(const Oidx& oid)
TS_SYNCHRONIZED(
{
	Oidx tmpoid(oid);
	return content.find(&tmpoid);
})

MibGroup* MibContext::find_group(const Oidx& oid)
{
	Oidx tmpoid(oid);
	return groups.find(&tmpoid);
}

bool MibContext::remove_group(const Oidx& oid)
TS_SYNCHRONIZED(
{
	Oidx tmpoid(oid);
	MibGroup* victim = groups.find(&tmpoid);
	if (victim) {
		ListCursor<MibEntry> cur(victim->get_content());
		for (; cur.get(); cur.next()) {
			MibEntry* v = content.find(cur.get()->key());
			if (v)
				delete content.remove(v);
		}
		delete groups.remove(victim);
		return TRUE;
	}
	return FALSE;
})

MibGroup* MibContext::find_group_of(const Oidx& oid)
{
	Oidx tmpoid(oid);
	OidListCursor<MibGroup> cur;
	for (cur.init(&groups); cur.get(); cur.next()) {
		if (oid.in_subtree_of(*cur.get()->key())) {
			ListCursor<MibEntry> c(cur.get()->get_content());
			for (; c.get(); c.next()) {
				if (*c.get()->key() == oid) return cur.get();
			}
		}
	}
	return 0;
}
#ifdef AGENTPP_NAMESPACE
}
#endif


