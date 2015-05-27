/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - cmd_exe_mib.h  
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


#include <agent_pp/mib.h>
#include <agent_pp/snmp_textual_conventions.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


/**
 *  cmdExecutionCmdConfigName
 *
"The name of the command."
 */


class cmdExecutionCmdConfigName: public MibLeaf {

 public:
	cmdExecutionCmdConfigName(const Oidx&);
	virtual ~cmdExecutionCmdConfigName();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
};


/**
 *  cmdExecutionCmdConfigLine
 *
"The command's command line."
 */


class cmdExecutionCmdConfigLine: public MibLeaf {

 public:
	cmdExecutionCmdConfigLine(const Oidx&);
	virtual ~cmdExecutionCmdConfigLine();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual int        	set(const Vbx&);
	virtual bool    	value_ok(const Vbx&);
};



/**
 *  cmdExecutionCmdNextIndex
 *
"Identifies a hint for the next value of 
 cmdExecutionCmdIndex to be used in a row creation 
 attempt for the cmdExecutionCmdTable table. If new rows 
 can not be created, this object will have a value of 0"
 */


class cmdExecutionCmdNextIndex: public MibLeaf {

 public:
	cmdExecutionCmdNextIndex();
	virtual ~cmdExecutionCmdNextIndex();

	static cmdExecutionCmdNextIndex* instance;

	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);
};


/**
 *  cmdExecutionCmdIndex
 *
"The index for the command."
 */


class cmdExecutionCmdIndex: public MibLeaf {

 public:
	cmdExecutionCmdIndex(const Oidx&);
	virtual ~cmdExecutionCmdIndex();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);
};


/**
 *  cmdExecutionCmdName
 *
"The index of the command in cmdExecutionCmdConfigTable 
 to be executed."
 */


class cmdExecutionCmdName: public MibLeaf {

 public:
	cmdExecutionCmdName(const Oidx&);
	virtual ~cmdExecutionCmdName();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual int        	set(const Vbx&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  cmdExecutionCmdStatus
 *
"The status of the command."
 */


class cmdExecutionCmdStatus: public MibLeaf {

 public:
	cmdExecutionCmdStatus(const Oidx&);
	virtual ~cmdExecutionCmdStatus();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);
	
};


/**
 *  cmdExecutionCmdRunTime
 *
"The execution time of the command."
 */


class cmdExecutionCmdRunTime: public MibLeaf {

 public:
	cmdExecutionCmdRunTime(const Oidx&);
	virtual ~cmdExecutionCmdRunTime();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);

	virtual long		get_state();
	virtual void		set_state(long);
	virtual void            start();
	virtual void		end();
 private:
	time_t  start_time;
	time_t  end_time;
};


/**
 *  cmdExecutionCmdRowStatus
 *
"The row status of this conceptual row. When this row
 is activated the specified command is executed. After
 execution the row status is notInService."
 */


class cmdExecutionCmdRowStatus: public snmpRowStatus {

 public:
	cmdExecutionCmdRowStatus(const Oidx&);
	virtual ~cmdExecutionCmdRowStatus();

	virtual MibEntryPtr	clone();
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int		prepare_set_request(Request*, int&); 
};


/**
 *  cmdExecutionOutputLineNumber
 *
"The line number of the output entry."
 */


class cmdExecutionOutputLineNumber: public MibLeaf {

 public:
	cmdExecutionOutputLineNumber(const Oidx&);
	virtual ~cmdExecutionOutputLineNumber();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);
};


/**
 *  cmdExecutionOutputLine
 *
"The output line."
 */


class cmdExecutionOutputLine: public MibLeaf {

 public:
	cmdExecutionOutputLine(const Oidx&);
	virtual ~cmdExecutionOutputLine();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
};


/**
 *  cmdExecutionCmdConfigEntry
 *
"An entry in the available command table."
 */


class cmdExecutionCmdConfigEntry: public StorageTable {

 public:
	cmdExecutionCmdConfigEntry();
	virtual ~cmdExecutionCmdConfigEntry();

	static cmdExecutionCmdConfigEntry* instance;

	virtual bool		deserialize(char*, int&);
	virtual void        	row_added(MibTableRow*, const Oidx&, MibTable* t=0);
	virtual void        	row_delete(MibTableRow*, const Oidx&, MibTable* t=0);
	virtual void       	set_row(int index, const char* p0, int p1, int p2);
	virtual bool		contains(Oidx);
	virtual NS_SNMP OctetStr	get_command_line(const NS_SNMP OctetStr&);
};


/**
 *  cmdExecutionCmdEntry
 *
"An entry in the command execution table. Each row contains 
 information about an UNIX command that should be executed
 (rowStatus is notInService and status is idle(1))
 or is executing (rowStatus is active and status is 
 running(2)) or has been executed (rowStatus is 
 notInService and status is finished(3))."
 */


class cmdExecutionCmdEntry: public MibTable {
friend class cmdExecutionCmdRowStatus;
 public:
	cmdExecutionCmdEntry();
	virtual ~cmdExecutionCmdEntry();

	static cmdExecutionCmdEntry* instance;

	virtual void        	row_added(MibTableRow*, const Oidx&, MibTable* t=0);
	virtual void        	row_delete(MibTableRow*, const Oidx&, MibTable* t=0);
 protected:
	ThreadPool*		threadPool;
};


/**
 *  cmdExecutionOutputEntry
 *
"An entry of the output list."
 */


class cmdExecutionOutputEntry: public MibTable {

 public:
	cmdExecutionOutputEntry();
	virtual ~cmdExecutionOutputEntry();

	static cmdExecutionOutputEntry* instance;

	virtual void        	row_added(MibTableRow*, const Oidx&, MibTable* t=0);
	virtual void        	row_delete(MibTableRow*, const Oidx&, MibTable* t=0);
	virtual void       	set_row(int index, char* p0);
	virtual void		remove_all(const Oidx&);
};


class command_execution_mib: public MibGroup
{
  public:
	command_execution_mib();
	virtual ~command_execution_mib() { }
};

class CmdThread: public Runnable {
 public:
	CmdThread(MibTableRow* r) { row_ptr = r; }
	virtual ~CmdThread() { }
	virtual void run();
 protected:
	MibTableRow* row_ptr;
};

#ifdef AGENTPP_NAMESPACE
}
#endif


/**
 * cmd_exe_mib.h generated by AgentGen 0.98 Sat May 08 18:46:48 GMT+04:30 1999.
 */

