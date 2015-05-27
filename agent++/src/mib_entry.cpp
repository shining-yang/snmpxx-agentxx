/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - mib_entry.cpp  
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

#include <agent_pp/mib_entry.h>
#include <agent_pp/tools.h>
#include <snmp_pp/log.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


/*--------------------------------------------------------------------
 *  
 *  class MibEntry 
 * 
 */



/**
 * Default constructor.
 */ 
MibEntry::MibEntry(): oid(""), access(NOACCESS)
{
} 

/**
 * Construct a MibEntry instance with a given object identifier
 * and maximum access rights.
 * 
 * @param o - An object identifier.
 * @param a - The maximum access of the receiver.
 */   
MibEntry::MibEntry(const Oidx& o, mib_access a): oid(o), access(a)
{
}

/**
 * Copy constructor.
 * 
 * @param other - A MibEntry object.
 */
MibEntry::MibEntry(const MibEntry& other)
{
	oid		= other.oid;
	access		= other.access;
}

/**
 * Destructor
 */
MibEntry::~MibEntry() 
{ 
	start_synch(); // exclusively get this object. 
	// never unlock this object again. 

	// explictly call clear() on this list, because it contains pointers
	// to other mib objects which we don't want to delete here 
	notifies.clear();
}

/**
 * Return the type of the receiver MIB node.
 *
 * @return 
 *    One of the following types: AGENTPP_NONE, AGENTPP_PROXY, AGENTPP_LEAF, 
 *    AGENTPP_TABLE, AGENTPP_GROUP, or AGENTPP_COMPLEX.
 */
mib_type MibEntry::type() const 
{
	return AGENTPP_NONE;
}


/**
 * Return a pointer to the key (object identifier) of the receiver.
 *
 * @return A pointer to an object identifier.
 */
OidxPtr MibEntry::key() 
{ 
	return &oid; 
}

int MibEntry::operator<(const MibEntry& other) const
{ 
	return (oid < other.oid); 
}

int MibEntry::operator>(const MibEntry& other) const
{ 
	return (oid > other.oid); 
}

int MibEntry::operator==(const MibEntry& other) const
{ 
	return (oid == other.oid); 
}

/**
 * Return a clone of the receiver.
 *
 * @return A pointer to a clone of the MibEntry object.  
 */
MibEntryPtr MibEntry::clone()
{
	MibEntryPtr aClone = new MibEntry(oid, access);
	ListCursor<MibEntry> cur;
	for (cur.init(&notifies); cur.get(); cur.next())
		aClone->notifies.add(cur.get());
	return aClone;
}

Oidx* MibEntry::max_key()
{
	return &oid;
}

/**
 * Return the maximum access rights for the managed object
 * represented by the receiver node.
 * 
 * @return The maximum access (one of the following values:
 *         NOACCESS, READONLY, READWRITE, or READCREATE)
 */
mib_access MibEntry::get_access()
{
	return access;
}

int MibEntry::operator>(const Oidx& other) const
{ 
	return (oid > other); 
}

int MibEntry::operator<(const Oidx& other) const
{ 
	return (oid < other); 
}

int MibEntry::operator>=(const Oidx& other) const 
{ 
	return (oid >= other); 
}

int MibEntry::operator<=(const Oidx& other) const
{ 
	return (oid <= other); 
}

int MibEntry::operator==(const Oidx& other) const
{ 
	return (oid == other); 
}

void MibEntry::set_oid(const Oidx& o)
{
	oid = o;
}

/**
 * Register an MibEntry object to receive notifications about
 * changes of the receiver node.
 *
 * @param entry - A MibEntry to receive notifications.
 */  
void MibEntry::register_for_notifications(MibEntry* entry)
{
	notifies.add(entry);
}

/**
 * Notify all registered nodes of changes to an object
 * managed by the receiver node.
 *
 * @param o - The object identifier of the object changed.
 * @param change - The type of the change (REMOVE, CREATE, CHANGE,
 *                 or UPDATE)
 */
void MibEntry::notify_change(const Oidx& o, mib_change change)
{
	if (notifies.empty()) return;
	ListCursor<MibEntry> cur;
	for (cur.init(&notifies); cur.get(); cur.next()) 
		cur.get()->change_notification(o, change);
}


void MibEntry::load_from_file(const char* fname)
{
	FILE *f;
	char *buf;
	int size, bytes;

	if ((f = fopen(fname, "rb")) == 0)
		return;
	
	size  = AgentTools::file_size(f);
	if (size>0) {
		buf   = new char[size+1];
		bytes = fread(buf, sizeof(char), size, f);
		if (bytes != size) {
			delete [] buf;
			return;
		}
	       
		deserialize(buf, bytes);
		delete [] buf;
	}
	fclose(f);
}
	
void MibEntry::save_to_file(const char* fname)
{
	FILE *f;
	char *buf = 0;
	int bytes;

	if (serialize(buf, bytes)) {
		if ((f = fopen(fname, "wb")) == 0) {
			delete [] buf;
			return;
		}
		fwrite(buf, sizeof(char), bytes, f);
		fclose(f);
		delete [] buf;
	}
}

bool MibEntry::serialize(char*&, int&)
{
	return FALSE;
}

bool MibEntry::deserialize(char*, int&)
{
	return FALSE;
}

bool MibEntry::is_volatile()
{
	return FALSE;
}


#ifdef AGENTPP_NAMESPACE
}
#endif

