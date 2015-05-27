/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - agentpp_test_mib.h  
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
   

//--AgentGen BEGIN=_BEGIN
//--AgentGen END

#ifndef _agentpp_test_mib_h
#define _agentpp_test_mib_h

#include <agent_pp/mib.h>
#include <agent_pp/mib_complex_entry.h>
#include <agent_pp/snmp_textual_conventions.h>
#include <snmp_pp/log.h>
#include <agent_pp/notification_originator.h>


// Scalars
#define oidAgentppTestTimeout             "1.3.6.1.4.1.4976.6.3.1.1.0"

// Columns
#define oidAgentppTestSharedTableCreationTime       "1.3.6.1.4.1.4976.6.3.1.3.1.2"
#define colAgentppTestSharedTableCreationTime       "2"
#define oidAgentppTestSharedTableDelay    "1.3.6.1.4.1.4976.6.3.1.3.1.3"
#define colAgentppTestSharedTableDelay    "3"
#define oidAgentppTestSharedTableSession  "1.3.6.1.4.1.4976.6.3.1.3.1.4"
#define colAgentppTestSharedTableSession  "4"
#define oidAgentppTestSharedTableRowStatus    "1.3.6.1.4.1.4976.6.3.1.3.1.5"
#define colAgentppTestSharedTableRowStatus    "5"

#define oidAgentppTestRowCreation         "1.3.6.1.4.1.4976.6.3.1.4.1.2"
#define colAgentppTestRowCreation         "2"

#define oidAgentppTestSparseCol1          "1.3.6.1.4.1.4976.6.3.1.5.1.2"
#define colAgentppTestSparseCol1          "2"
#define oidAgentppTestSparseCol2          "1.3.6.1.4.1.4976.6.3.1.5.1.3"
#define colAgentppTestSparseCol2          "3"
#define oidAgentppTestSparseCol3          "1.3.6.1.4.1.4976.6.3.1.5.1.4"
#define colAgentppTestSparseCol3          "4"
#define oidAgentppTestSparseRowStatus     "1.3.6.1.4.1.4976.6.3.1.5.1.5"
#define colAgentppTestSparseRowStatus     "5"


// Tables
#define oidAgentppTestSharedEntry         "1.3.6.1.4.1.4976.6.3.1.3.1"
#define nAgentppTestSharedTableCreationTime      0
#define cAgentppTestSharedTableCreationTime      2
#define nAgentppTestSharedTableDelay     1
#define cAgentppTestSharedTableDelay     3
#define nAgentppTestSharedTableSession   2
#define cAgentppTestSharedTableSession   4
#define nAgentppTestSharedTableRowStatus   3
#define cAgentppTestSharedTableRowStatus   5

#define oidAgentppTestSessionsEntry       "1.3.6.1.4.1.4976.6.3.1.4.1"
#define nAgentppTestRowCreation          0
#define cAgentppTestRowCreation          2

#define oidAgentppTestSparseEntry         "1.3.6.1.4.1.4976.6.3.1.5.1"
#define nAgentppTestSparseCol1           0
#define cAgentppTestSparseCol1           2
#define nAgentppTestSparseCol2           1
#define cAgentppTestSparseCol2           3
#define nAgentppTestSparseCol3           2
#define cAgentppTestSparseCol3           4
#define nAgentppTestSparseRowStatus      3
#define cAgentppTestSparseRowStatus      5


// Notifications


// Win32 Users: The following ifdef block is the standard 
// way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL
// are compiled with the AGENTPP_TEST_MIB_EXPORTS
// symbol defined on the command line. This symbol should
// not be defined on any project that uses this DLL. 
// This way any other project whose source files include
// this file see AGENTPP_TEST_MIB_API 
// functions as being imported from a DLL, whereas this DLL
// sees symbols defined with this macro as being exported.
#ifndef AGENTPP_TEST_MIB_DECL
	#if defined (WIN32) && defined (AGENTPP_TEST_MIB_DLL)
		#ifdef AGENTPP_TEST_MIB_EXPORTS
			#define AGENTPP_TEST_MIB_DECL __declspec(dllexport)
		#else
			#define AGENTPP_TEST_MIB_DECL __declspec(dllimport)
		#endif
	#else
		#define AGENTPP_TEST_MIB_DECL
	#endif
#endif

//--AgentGen BEGIN=_INCLUDE
//--AgentGen END

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


// Scalar Objects



/**
 *  agentppTestTimeout
 *
 * "Setting this object will be delayed by the given
 * amount of milliseconds. That is, by setting this
 * object to 1000 the corresponding response to
 * that SET request will be delayed by one second."
 */
 
class AGENTPP_TEST_MIB_DECL agentppTestTimeout: public MibLeaf
{
	
public:
	agentppTestTimeout();
	virtual ~agentppTestTimeout();

	static agentppTestTimeout* instance;
	
	virtual void       	get_request(Request*, int);
	virtual void       	set_state(unsigned long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);

//--AgentGen BEGIN=agentppTestTimeout
//--AgentGen END
};


// Columnar Objects



/**
 *  agentppTestSharedTableCreationTime
 *
 * "The date and time when this row has been created."
 */
 
class AGENTPP_TEST_MIB_DECL agentppTestSharedTableCreationTime: public DateAndTime
{
	
public:
	agentppTestSharedTableCreationTime(const Oidx&);
	virtual ~agentppTestSharedTableCreationTime();

	virtual MibEntryPtr	clone();

//--AgentGen BEGIN=agentppTestSharedTableCreationTime
//--AgentGen END
};




/**
 *  agentppTestSharedTableDelay
 *
 * "The number of 1/100 seconds that a request to this
 * row will be delayed before it is processed."
 */
 
class AGENTPP_TEST_MIB_DECL agentppTestSharedTableDelay: public MibLeaf
{
	
public:
	agentppTestSharedTableDelay(const Oidx&);
	virtual ~agentppTestSharedTableDelay();

	virtual MibEntryPtr	clone();

//--AgentGen BEGIN=agentppTestSharedTableDelay
//--AgentGen END
};




/**
 *  agentppTestSharedTableSession
 *
 * "This object denotes the AgentX session ID of the
 * session on whose behalf this row has been created."
 */
 
class AGENTPP_TEST_MIB_DECL agentppTestSharedTableSession: public MibLeaf
{
	
public:
	agentppTestSharedTableSession(const Oidx&);
	virtual ~agentppTestSharedTableSession();

	virtual MibEntryPtr	clone();

//--AgentGen BEGIN=agentppTestSharedTableSession
//--AgentGen END
};




/**
 *  agentppTestSharedTableRowStatus
 *
 * "The row status of the row."
 */
 
class AGENTPP_TEST_MIB_DECL agentppTestSharedTableRowStatus: public snmpRowStatus
{
	
public:
	agentppTestSharedTableRowStatus(const Oidx&);
	virtual ~agentppTestSharedTableRowStatus();

	virtual MibEntryPtr	clone();
	enum labels {
		      e_active = 1,
		      e_notInService = 2,
		      e_notReady = 3,
		      e_createAndGo = 4,
		      e_createAndWait = 5,
		      e_destroy = 6	};

//--AgentGen BEGIN=agentppTestSharedTableRowStatus
//--AgentGen END
};




/**
 *  agentppTestRowCreation
 *
 * "This object can be set to the index of a new row
 * in the agentppTestSharedTable. If a row with the
 * set index already exists, this object will return zero,
 * otherwise it will return the last value set."
 */
 
class AGENTPP_TEST_MIB_DECL agentppTestRowCreation: public MibLeaf
{
	
public:
	agentppTestRowCreation(const Oidx&);
	virtual ~agentppTestRowCreation();

	virtual MibEntryPtr	clone();

//--AgentGen BEGIN=agentppTestRowCreation
//--AgentGen END
};




/**
 *  agentppTestSparseCol1
 *
 * "By setting this object to its current value the object
 * becomes notAccessible allowing testing of
 * sparse table implementation."
 */
 
class AGENTPP_TEST_MIB_DECL agentppTestSparseCol1: public MibLeaf
{
	
public:
	agentppTestSparseCol1(const Oidx&);
	virtual ~agentppTestSparseCol1();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);

//--AgentGen BEGIN=agentppTestSparseCol1
//--AgentGen END
};




/**
 *  agentppTestSparseCol2
 *
 * "By setting this object to its current value the object
 * becomes notAccessible allowing testing of
 * sparse table implementation."
 */
 
class AGENTPP_TEST_MIB_DECL agentppTestSparseCol2: public MibLeaf
{
	
public:
	agentppTestSparseCol2(const Oidx&);
	virtual ~agentppTestSparseCol2();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual unsigned long   get_state();
	virtual void       	set_state(unsigned long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);

//--AgentGen BEGIN=agentppTestSparseCol2
//--AgentGen END
};




/**
 *  agentppTestSparseCol3
 *
 * "By setting this object to its current value the object
 * becomes notAccessible allowing testing of
 * sparse table implementation."
 */
 
class AGENTPP_TEST_MIB_DECL agentppTestSparseCol3: public MibLeaf
{
	
public:
	agentppTestSparseCol3(const Oidx&);
	virtual ~agentppTestSparseCol3();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual NS_SNMP OctetStr       	get_state();
	virtual void       	set_state(const NS_SNMP OctetStr&);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);

//--AgentGen BEGIN=agentppTestSparseCol3
//--AgentGen END
};




/**
 *  agentppTestSparseRowStatus
 *
 * "Use this column to create a row in the test table."
 */
 
class AGENTPP_TEST_MIB_DECL agentppTestSparseRowStatus: public snmpRowStatus
{
	
public:
	agentppTestSparseRowStatus(const Oidx&);
	virtual ~agentppTestSparseRowStatus();

	virtual MibEntryPtr	clone();
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	enum labels {
		      e_active = 1,
		      e_notInService = 2,
		      e_notReady = 3,
		      e_createAndGo = 4,
		      e_createAndWait = 5,
		      e_destroy = 6	};

//--AgentGen BEGIN=agentppTestSparseRowStatus
//--AgentGen END
};


// Tables


/**
 *  agentppTestSharedEntry
 *
 * "A row of a shared table. Each row is allocated and
 * registered in random intervals."
 */
 
class AGENTPP_TEST_MIB_DECL agentppTestSharedEntry: public MibTable
{
	
public:
	agentppTestSharedEntry();
	virtual ~agentppTestSharedEntry();

	static agentppTestSharedEntry* instance;
	
//--AgentGen BEGIN=agentppTestSharedEntry
//--AgentGen END
};


/**
 *  agentppTestSessionsEntry
 *
 * "A row of this table is created by each subagent
 * session that implements the AGENTPP-TEST-MIB."
 */
 
class AGENTPP_TEST_MIB_DECL agentppTestSessionsEntry: public MibTable
{
	
public:
	agentppTestSessionsEntry();
	virtual ~agentppTestSessionsEntry();

	static agentppTestSessionsEntry* instance;
	
//--AgentGen BEGIN=agentppTestSessionsEntry
//--AgentGen END
};


/**
 *  agentppTestSparseEntry
 *
 * "A row of a sparese table can be created and
 * deleted via its row status column."
 */
 
class AGENTPP_TEST_MIB_DECL agentppTestSparseEntry: public MibTable
{
	
public:
	agentppTestSparseEntry();
	virtual ~agentppTestSparseEntry();

	static agentppTestSparseEntry* instance;
	
	virtual void        	get_request(Request*, int);
	virtual int         	prepare_set_request(Request*, int&);
//--AgentGen BEGIN=agentppTestSparseEntry
//--AgentGen END
};

// Notifications
#ifdef _SNMPv3
#endif

// Group

class AGENTPP_TEST_MIB_DECL agentpp_test_mib: public MibGroup
{
  public:
        agentpp_test_mib();
        virtual ~agentpp_test_mib() { }
//--AgentGen BEGIN=agentpp_test_mib
//--AgentGen END
};

//--AgentGen BEGIN=_CLASSES
//--AgentGen END


#ifdef AGENTPP_NAMESPACE
}
#endif


//--AgentGen BEGIN=_END
//--AgentGen END

#endif
