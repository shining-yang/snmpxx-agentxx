/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - cmd_exe_mib.cpp  
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

#include <stdlib.h>

#include <agent_pp/system_group.h>
#include <snmp_pp/log.h>

#include "cmd_exe_mib.h"

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

static const char* loggerModuleName = "agent++.cmd_exe_mib";

void CmdThread::run() 
{
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	LOG("CmdExeMib: starting command thread");
	LOG_END;

	MibTableRow* row = (MibTableRow*)row_ptr;
	cmdExecutionCmdEntry::instance->start_synch();

	OctetStr cmd;
	((cmdExecutionCmdName*)row->get_nth(0))->get_value(cmd);
	((cmdExecutionCmdStatus*)row->get_nth(1))->set_state(2);
	((cmdExecutionCmdRunTime*)row->get_nth(2))->start();

	OctetStr cmdline;
	cmdExecutionCmdConfigEntry::instance->start_synch();
	cmdline = cmdExecutionCmdConfigEntry::instance->get_command_line(cmd);
	cmdExecutionCmdConfigEntry::instance->end_synch();
	
	if (cmdline.len() == 0) {
		((cmdExecutionCmdStatus*)row->get_nth(1))->set_state(4);
		((cmdExecutionCmdRunTime*)row->get_nth(2))->end();
		cmdExecutionCmdEntry::instance->end_synch();
		return;
	}
	cmdline += " > ";
	cmdline += Mib::instance->get_persistent_objects_path();
	cmdline += "exec_output.";
	cmdline += row->get_index().get_printable();
	
	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 8);
	LOG("Execution command (cmdline)");
	LOG(cmdline.get_printable());
	LOG_END;

	cmdExecutionCmdEntry::instance->end_synch();
	
	int err = system(cmdline.get_printable());
	
	cmdExecutionCmdEntry::instance->start_synch();
	((cmdExecutionCmdStatus*)row->get_nth(1))->set_state((err!=0) ? 4:3);
	((cmdExecutionCmdRunTime*)row->get_nth(2))->end();
	((cmdExecutionCmdRowStatus*)row->get_nth(3))->set_state(rowNotInService);
	cmdExecutionCmdEntry::instance->end_synch();
	
	cmdExecutionOutputEntry::instance->start_synch();

	FILE *f;
	char *buf;
	long size, bytes;

	OctetStr fname;
	fname += Mib::instance->get_persistent_objects_path();
	fname += "exec_output.";
	fname += row->get_index().get_printable();
	
	if ((f = fopen(fname.get_printable(), "r")) == 0) {
		cmdExecutionOutputEntry::instance->end_synch();
		return;
	}
	
	size  = AgentTools::file_size(f);
	if (size>0) {
		buf   = new char[size+2];
		bytes = fread(buf, sizeof(char), size, f);
		buf[size+1] = 0;

		if (bytes == size) {

			long i = 1;
			char* ptr = buf;
			char* nl = buf;
			while ((ptr<buf+size) && (nl)) {
			  nl = strchr(ptr, '\n');
			  OctetStr* line = new OctetStr();
			  if (nl == 0) {
				*line = ptr;
			  }
			  else {
				char* l = new char[nl-ptr+1];
				strncpy(l, ptr, nl-ptr);
				l[nl-ptr] = 0;
				*line = l;
				ptr = nl+1;
			  }
			  Oidx index(row->get_index());
			  index += i;
			  i++;
			  MibTableRow* r = 
			    cmdExecutionOutputEntry::instance->add_row(index);
			  r->get_nth(0)->replace_value(line);

			  LOG_BEGIN(loggerModuleName, DEBUG_LOG | 9);
			  LOG("Added ouput (line)(output)");
			  LOG(index.get_printable());
			  LOG(line->get_printable());
			  LOG_END;
			}
		}
		delete[] buf;
	}
	cmdExecutionOutputEntry::instance->end_synch();

	fclose(f);
	remove(fname.get_printable());
}

/**
 *  cmdExecutionCmdConfigLine
 *
 */

cmdExecutionCmdConfigLine::cmdExecutionCmdConfigLine(const Oidx& id):
   MibLeaf(id, READCREATE, new OctetStr())
{

}

cmdExecutionCmdConfigLine::~cmdExecutionCmdConfigLine()
{
}

MibEntryPtr cmdExecutionCmdConfigLine::clone()
{
	MibEntryPtr other = new cmdExecutionCmdConfigLine(oid);
	((cmdExecutionCmdConfigLine*)other)->replace_value(value->clone());
	((cmdExecutionCmdConfigLine*)other)->set_reference_to_table(my_table);
	return other;
}

void cmdExecutionCmdConfigLine::get_request(Request* req, int ind)
{
	// place instrumentation code (manipulating "value") here
	MibLeaf::get_request(req, ind);
}

int cmdExecutionCmdConfigLine::set(const Vbx& vb)
{
	// place code for handling operations triggered
	// by this set here
	return MibLeaf::set(vb);
}

bool cmdExecutionCmdConfigLine::value_ok(const Vbx& vb)
{
	// place code to check validity of new values here
	return TRUE;
}






/**
 *  cmdExecutionCmdNextIndex
 *
 */

cmdExecutionCmdNextIndex* cmdExecutionCmdNextIndex::instance = 0;

cmdExecutionCmdNextIndex::cmdExecutionCmdNextIndex():
   MibLeaf("1.3.6.1.4.1.4976.6.1.2.2.1.0", READONLY, new SnmpInt32())
{
	// This leaf object is a singleton. In order to access it use
	// the static pointer cmdExecutionCmdNextIndex::instance.
	instance = this;
}

cmdExecutionCmdNextIndex::~cmdExecutionCmdNextIndex()
{
}

void cmdExecutionCmdNextIndex::get_request(Request* req, int ind)
{
	set_state(cmdExecutionCmdEntry::instance->get_next_avail_index()[0]);
	MibLeaf::get_request(req, ind);
}

long cmdExecutionCmdNextIndex::get_state()
{
	return (long)*((SnmpInt32*)value);
}

void cmdExecutionCmdNextIndex::set_state(long l)
{
	*((SnmpInt32*)value) = l;
}


/**
 *  cmdExecutionCmdName
 *
 */

cmdExecutionCmdName::cmdExecutionCmdName(const Oidx& id):
   MibLeaf(id, READCREATE, new OctetStr(""))
{

}

cmdExecutionCmdName::~cmdExecutionCmdName()
{
}

MibEntryPtr cmdExecutionCmdName::clone()
{
	MibEntryPtr other = new cmdExecutionCmdName(oid);
	((cmdExecutionCmdName*)other)->replace_value(value->clone());
	((cmdExecutionCmdName*)other)->set_reference_to_table(my_table);
	return other;
}

void cmdExecutionCmdName::get_request(Request* req, int ind)
{
	// place instrumentation code (manipulating "value") here
	MibLeaf::get_request(req, ind);
}

int cmdExecutionCmdName::set(const Vbx& vb)
{
	OctetStr name;
	vb.get_value(name);
	if (!cmdExecutionCmdConfigEntry::instance->
	    contains(Oidx::from_string(name))) 
	  return SNMP_ERROR_INCONSIST_VAL;
	return MibLeaf::set(vb);
}

bool cmdExecutionCmdName::value_ok(const Vbx& vb)
{
	return TRUE;
}



/**
 *  cmdExecutionCmdStatus
 *
 */

cmdExecutionCmdStatus::cmdExecutionCmdStatus(const Oidx& id):
   MibLeaf(id, READONLY, new SnmpInt32(1))
{

}

cmdExecutionCmdStatus::~cmdExecutionCmdStatus()
{
}

MibEntryPtr cmdExecutionCmdStatus::clone()
{
	MibEntryPtr other = new cmdExecutionCmdStatus(oid);
	((cmdExecutionCmdStatus*)other)->replace_value(value->clone());
	((cmdExecutionCmdStatus*)other)->set_reference_to_table(my_table);
	return other;
}

void cmdExecutionCmdStatus::get_request(Request* req, int ind)
{
	// place instrumentation code (manipulating "value") here
	MibLeaf::get_request(req, ind);
}

long cmdExecutionCmdStatus::get_state()
{
	return (long)*((SnmpInt32*)value);
}

void cmdExecutionCmdStatus::set_state(long l)
{
	*((SnmpInt32*)value) = l;
}



/**
 *  cmdExecutionCmdRunTime
 *
 */

cmdExecutionCmdRunTime::cmdExecutionCmdRunTime(const Oidx& id):
   MibLeaf(id, READONLY, new TimeTicks(0))
{
	start_time = 0;
	end_time = 0;
}

cmdExecutionCmdRunTime::~cmdExecutionCmdRunTime()
{
}

MibEntryPtr cmdExecutionCmdRunTime::clone()
{
	MibEntryPtr other = new cmdExecutionCmdRunTime(oid);
	((cmdExecutionCmdRunTime*)other)->replace_value(value->clone());
	((cmdExecutionCmdRunTime*)other)->set_reference_to_table(my_table);
	return other;
}

void cmdExecutionCmdRunTime::get_request(Request* req, int ind)
{
	if (start_time > 0) {
		if (end_time > 0) {
			set_state((end_time-start_time)*100);
		}
		else set_state((sysUpTime::get_currentTime()-start_time)*100);
	}
	MibLeaf::get_request(req, ind);
}

long cmdExecutionCmdRunTime::get_state()
{
	return (long)*((TimeTicks*)value);
}

void cmdExecutionCmdRunTime::set_state(long l)
{
	*((TimeTicks*)value) = l;
}

void cmdExecutionCmdRunTime::start() 
{ 
	start_time = sysUpTime::get_currentTime();
	end_time = 0;
}

void cmdExecutionCmdRunTime::end() 
{ 
	end_time = sysUpTime::get_currentTime();
}



/**
 *  cmdExecutionCmdRowStatus
 *
 */

cmdExecutionCmdRowStatus::cmdExecutionCmdRowStatus(const Oidx& id):
   snmpRowStatus(id, READCREATE)
{

}

cmdExecutionCmdRowStatus::~cmdExecutionCmdRowStatus()
{
}

MibEntryPtr cmdExecutionCmdRowStatus::clone()
{
	MibEntryPtr other = new cmdExecutionCmdRowStatus(oid);
	((cmdExecutionCmdRowStatus*)other)->replace_value(value->clone());
	((cmdExecutionCmdRowStatus*)other)->set_reference_to_table(my_table);
	return other;
}

long cmdExecutionCmdRowStatus::get_state()
{
	return (long)*((SnmpInt32*)value);
}

void cmdExecutionCmdRowStatus::set_state(long l)
{
	*((SnmpInt32*)value) = l;
}

int cmdExecutionCmdRowStatus::prepare_set_request(Request* req, int& ind) 
{
	Vbx vb = req->get_value(ind);
	unsigned int l;
	if (vb.get_value(l) != SNMP_CLASS_SUCCESS)
		return SNMP_ERROR_WRONG_TYPE;
	
	switch (l) {
	case rowNotInService: {
		if (((cmdExecutionCmdStatus*)
		     my_row->get_nth(1))->get_state() == 2)
			return SNMP_ERROR_INCONSIST_VAL;
		break;
	}
	case rowDestroy: {
		if (((cmdExecutionCmdStatus*)
		     my_row->get_nth(1))->get_state() == 2)
			return SNMP_ERROR_INCONSIST_VAL;
		break;
	}
	}
	return SNMP_ERROR_SUCCESS;
}

int cmdExecutionCmdRowStatus::set(const Vbx& vb)
{
	unsigned int l;
	if (vb.get_value(l) != SNMP_CLASS_SUCCESS)
		return SNMP_ERROR_WRONG_TYPE;
	
	switch (l) {
	case rowCreateAndGo:
	case rowActive: {
#ifdef _THREADS
	  CmdThread* ct = new CmdThread(my_row);
	  ((cmdExecutionCmdEntry*)my_table)->threadPool->execute(ct);
#else		
#error "Please_compile_with _THREADS defined"
#endif	  
		break;
	}
	case rowNotInService: {
		break;
	}
	case rowDestroy: {
		cmdExecutionOutputEntry::instance->
		  remove_all(my_row->get_index());
		break;
		
	}
	}
	return snmpRowStatus::set(vb);
}



/**
 *  cmdExecutionOutputLine
 *
 */

cmdExecutionOutputLine::cmdExecutionOutputLine(const Oidx& id):
   MibLeaf(id, READONLY, new OctetStr())
{

}

cmdExecutionOutputLine::~cmdExecutionOutputLine()
{
}

MibEntryPtr cmdExecutionOutputLine::clone()
{
	MibEntryPtr other = new cmdExecutionOutputLine(oid);
	((cmdExecutionOutputLine*)other)->replace_value(value->clone());
	((cmdExecutionOutputLine*)other)->set_reference_to_table(my_table);
	return other;
}

void cmdExecutionOutputLine::get_request(Request* req, int ind)
{
	// place instrumentation code (manipulating "value") here
	MibLeaf::get_request(req, ind);
}



/**
 *  cmdExecutionCmdConfigEntry
 *
 */

cmdExecutionCmdConfigEntry* cmdExecutionCmdConfigEntry::instance = 0;

cmdExecutionCmdConfigEntry::cmdExecutionCmdConfigEntry():
   StorageTable("1.3.6.1.4.1.4976.6.1.2.1.1.1", 0, false)
{
	// This table object is a singleton. In order to access it use
	// the static pointer cmdExecutionCmdConfigEntry::instance.
	instance = this;

	add_col(new cmdExecutionCmdConfigLine("2"));
	add_storage_col(new StorageType("3", 3));
	add_col(new snmpRowStatus("4", READCREATE));
}

cmdExecutionCmdConfigEntry::~cmdExecutionCmdConfigEntry()
{
}

bool cmdExecutionCmdConfigEntry::deserialize(char* buf, int& sz)
{
	bool b = MibTable::deserialize(buf, sz);
	if (!b) {
		add_row("2.108.108");
		set_row(0, "ls -la", 3, 1);
	}
	return b;
}

void cmdExecutionCmdConfigEntry::row_added(MibTableRow* row, const Oidx& index, MibTable* t)
{
	// The row 'row' with 'index' has been added to the table.
	// Place any necessary actions here.
}

void cmdExecutionCmdConfigEntry::row_delete(MibTableRow* row, const Oidx& index, MibTable* t)
{
	// The row 'row' with 'index' will be deleted.
	// Place any necessary actions here.
}



void cmdExecutionCmdConfigEntry::set_row(int index, const char* p0, int p1, int p2)
{
	get(0, index)->replace_value(new OctetStr(p0));
	get(1, index)->replace_value(new SnmpInt32(p1));
	get(2, index)->replace_value(new SnmpInt32(p2));
}

bool cmdExecutionCmdConfigEntry::contains(Oidx index) 
{
	OidListCursor<MibTableRow> cur;
	for (cur.init(&content); cur.get(); cur.next()) {
		if (strcmp(cur.get()->get_index().get_printable(),
			   index.get_printable()) == 0) {
			return TRUE;
		}
	}
	return FALSE;
}

OctetStr cmdExecutionCmdConfigEntry::get_command_line(const OctetStr& command) 
{
	OctetStr cmdline;
	Oidx index(Oidx::from_string(command));
	OidListCursor<MibTableRow> cur;
	for (cur.init(&content); cur.get(); cur.next()) {
		if (((snmpRowStatus*)cur.get()->get_nth(2))->get() ==
		    rowActive) {
		  if (strcmp(cur.get()->get_index().get_printable(),
			     index.get_printable()) == 0) {
		    
		    cur.get()->get_nth(0)->get_value(cmdline);
		    break;
		  }
		}
	}
	return cmdline;
}
	



/**
 *  cmdExecutionCmdEntry
 *
 */

cmdExecutionCmdEntry* cmdExecutionCmdEntry::instance = 0;

cmdExecutionCmdEntry::cmdExecutionCmdEntry():
   MibTable("1.3.6.1.4.1.4976.6.1.2.2.2.1", 1, FALSE)
{
	// This table object is a singleton. In order to access it use
	// the static pointer cmdExecutionCmdEntry::instance.
	instance = this;

	add_col(new cmdExecutionCmdName("2"));
	add_col(new cmdExecutionCmdStatus("3"));
	add_col(new cmdExecutionCmdRunTime("4"));
	add_col(new cmdExecutionCmdRowStatus("5"));

	threadPool = new ThreadPool(2);
}

cmdExecutionCmdEntry::~cmdExecutionCmdEntry()
{
	delete threadPool;
}

void cmdExecutionCmdEntry::row_added(MibTableRow* row, const Oidx& index, MibTable* t)
{
	// The row 'row' with 'index' has been added to the table.
	// Place any necessary actions here.
}

void cmdExecutionCmdEntry::row_delete(MibTableRow* row, const Oidx& index, MibTable* t)
{
	// The row 'row' with 'index' will be deleted.
	// Place any necessary actions here.
}



/**
 *  cmdExecutionOutputEntry
 *
 */

cmdExecutionOutputEntry* cmdExecutionOutputEntry::instance = 0;

cmdExecutionOutputEntry::cmdExecutionOutputEntry():
   MibTable("1.3.6.1.4.1.4976.6.1.2.3.1.1", 0, false)
{
	// This table object is a singleton. In order to access it use
	// the static pointer cmdExecutionOutputEntry::instance.
	instance = this;

	add_col(new cmdExecutionOutputLine("2"));

}

cmdExecutionOutputEntry::~cmdExecutionOutputEntry()
{
}

void cmdExecutionOutputEntry::row_added(MibTableRow* row, const Oidx& index, MibTable* t)
{
	// The row 'row' with 'index' has been added to the table.
	// Place any necessary actions here.
}

void cmdExecutionOutputEntry::row_delete(MibTableRow* row, const Oidx& index, MibTable* t)
{
	// The row 'row' with 'index' will be deleted.
	// Place any necessary actions here.
}



void cmdExecutionOutputEntry::set_row(int index, char* p0)
{
	get(0, index)->replace_value(new OctetStr(p0));
}

void cmdExecutionOutputEntry::remove_all(const Oidx& index) 
{
	OidListCursor<MibTableRow> cur;
	for (cur.init(&content); cur.get(); ) {
		if (cur.get()->get_index()[0] == index[0]) {
			MibTableRow* victim = cur.get();
			cur.next();
			delete content.remove(victim);
		}
		else {
			cur.next();
		}
	}
}

command_execution_mib::command_execution_mib(): 
  MibGroup("1.3.6.1.4.1.4976.6.1.2", "cmdExecutionMIB")
{
	add(new cmdExecutionCmdConfigEntry());
	add(new cmdExecutionCmdNextIndex());
	add(new cmdExecutionCmdEntry());
	add(new cmdExecutionOutputEntry());
}

#ifdef AGENTPP_NAMESPACE
}
#endif

