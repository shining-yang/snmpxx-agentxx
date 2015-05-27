/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - agentpp_test_mib.cpp  
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
#ifdef ___never_defined___
//--AgentGen END

#ifndef _agentpp_test_mib_cpp
#define _agentpp_test_mib_cpp

#include <agentpp_test_mib.h>
#include <agent_pp/module_includes.h>

//--AgentGen BEGIN=_INCLUDE
#endif
#endif

#ifndef _agentpp_test_mib_cpp
#define _agentpp_test_mib_cpp

#include <agentpp_test_mib.h>
//--AgentGen END


#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

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
 
agentppTestTimeout* agentppTestTimeout::instance = 0;


agentppTestTimeout::agentppTestTimeout():
    MibLeaf(oidAgentppTestTimeout, READWRITE, new Gauge32()){
	// This leaf object is a singleton. In order to access it use
	// the static pointer agentppTestTimeout::instance.
	instance = this;
	//--AgentGen BEGIN=agentppTestTimeout::agentppTestTimeout
	//--AgentGen END

}

agentppTestTimeout::~agentppTestTimeout()
{

	//--AgentGen BEGIN=agentppTestTimeout::~agentppTestTimeout
	//--AgentGen END

	// clear singleton reference
	agentppTestTimeout::instance = 0;
}

void agentppTestTimeout::get_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppTestTimeout::get_request
	//--AgentGen END
	MibLeaf::get_request(req, ind);
}

void agentppTestTimeout::set_state(unsigned long l)
{
	//--AgentGen BEGIN=agentppTestTimeout::set_state
	//--AgentGen END
	*((Gauge32*)value) = l;
}


int agentppTestTimeout::set(const Vbx& vb)
{
	//--AgentGen BEGIN=agentppTestTimeout::set
	unsigned long timeout;
	vb.get_value(timeout);
	struct timeval tvptr;
	tvptr.tv_sec  = timeout/1000;    // wait up to sec seconds
	tvptr.tv_usec = (timeout%1000)*1000;

	select(0, NULL, NULL, NULL, &tvptr);
	//--AgentGen END
	return MibLeaf::set(vb);
}

bool agentppTestTimeout::value_ok(const Vbx& vb)
{
	unsigned long v;
	vb.get_value(v);
	if (!(((v >= 0ul) && (v <= 1000000ul))))
		 return FALSE;
	//--AgentGen BEGIN=agentppTestTimeout::value_ok
	//--AgentGen END
	return TRUE;
}

int agentppTestTimeout::prepare_set_request(Request* req, int& ind)
{
	int status;
	if ((status = MibLeaf::prepare_set_request(req, ind)) !=
	    SNMP_ERROR_SUCCESS) return status;

	//--AgentGen BEGIN=agentppTestTimeout::prepare_set_request
	//--AgentGen END
	return SNMP_ERROR_SUCCESS;
}

//--AgentGen BEGIN=agentppTestTimeout
//--AgentGen END



// Columnar Objects


/**
 *  agentppTestSharedTableCreationTime
 *
 * "The date and time when this row has been created."
 */
 
agentppTestSharedTableCreationTime::agentppTestSharedTableCreationTime(const Oidx& id):
    DateAndTime(id, READONLY, VMODE_DEFAULT)
{
	//--AgentGen BEGIN=agentppTestSharedTableCreationTime::agentppTestSharedTableCreationTime
	//--AgentGen END

}

agentppTestSharedTableCreationTime::~agentppTestSharedTableCreationTime()
{

	//--AgentGen BEGIN=agentppTestSharedTableCreationTime::~agentppTestSharedTableCreationTime
	//--AgentGen END

}

MibEntryPtr agentppTestSharedTableCreationTime::clone()
{
	MibEntryPtr other = new agentppTestSharedTableCreationTime(oid);
	((agentppTestSharedTableCreationTime*)other)->replace_value(value->clone());
	((agentppTestSharedTableCreationTime*)other)->set_reference_to_table(my_table);
	//--AgentGen BEGIN=agentppTestSharedTableCreationTime::clone
	//--AgentGen END
	return other;
}


//--AgentGen BEGIN=agentppTestSharedTableCreationTime
//--AgentGen END




/**
 *  agentppTestSharedTableDelay
 *
 * "The number of 1/100 seconds that a request to this
 * row will be delayed before it is processed."
 */
 
agentppTestSharedTableDelay::agentppTestSharedTableDelay(const Oidx& id):
    MibLeaf(id, READCREATE, new SnmpInt32(0), VMODE_DEFAULT){
	//--AgentGen BEGIN=agentppTestSharedTableDelay::agentppTestSharedTableDelay
	//--AgentGen END

}

agentppTestSharedTableDelay::~agentppTestSharedTableDelay()
{

	//--AgentGen BEGIN=agentppTestSharedTableDelay::~agentppTestSharedTableDelay
	//--AgentGen END

}

MibEntryPtr agentppTestSharedTableDelay::clone()
{
	MibEntryPtr other = new agentppTestSharedTableDelay(oid);
	((agentppTestSharedTableDelay*)other)->replace_value(value->clone());
	((agentppTestSharedTableDelay*)other)->set_reference_to_table(my_table);
	//--AgentGen BEGIN=agentppTestSharedTableDelay::clone
	//--AgentGen END
	return other;
}


//--AgentGen BEGIN=agentppTestSharedTableDelay
//--AgentGen END




/**
 *  agentppTestSharedTableSession
 *
 * "This object denotes the AgentX session ID of the
 * session on whose behalf this row has been created."
 */
 
agentppTestSharedTableSession::agentppTestSharedTableSession(const Oidx& id):
    MibLeaf(id, READONLY, new Gauge32()){
	//--AgentGen BEGIN=agentppTestSharedTableSession::agentppTestSharedTableSession
	//--AgentGen END

}

agentppTestSharedTableSession::~agentppTestSharedTableSession()
{

	//--AgentGen BEGIN=agentppTestSharedTableSession::~agentppTestSharedTableSession
	//--AgentGen END

}

MibEntryPtr agentppTestSharedTableSession::clone()
{
	MibEntryPtr other = new agentppTestSharedTableSession(oid);
	((agentppTestSharedTableSession*)other)->replace_value(value->clone());
	((agentppTestSharedTableSession*)other)->set_reference_to_table(my_table);
	//--AgentGen BEGIN=agentppTestSharedTableSession::clone
	//--AgentGen END
	return other;
}


//--AgentGen BEGIN=agentppTestSharedTableSession
//--AgentGen END




/**
 *  agentppTestSharedTableRowStatus
 *
 * "The row status of the row."
 */
 
agentppTestSharedTableRowStatus::agentppTestSharedTableRowStatus(const Oidx& id):
    snmpRowStatus(id, READCREATE)
{
	//--AgentGen BEGIN=agentppTestSharedTableRowStatus::agentppTestSharedTableRowStatus
	//--AgentGen END

}

agentppTestSharedTableRowStatus::~agentppTestSharedTableRowStatus()
{

	//--AgentGen BEGIN=agentppTestSharedTableRowStatus::~agentppTestSharedTableRowStatus
	//--AgentGen END

}

MibEntryPtr agentppTestSharedTableRowStatus::clone()
{
	MibEntryPtr other = new agentppTestSharedTableRowStatus(oid);
	((agentppTestSharedTableRowStatus*)other)->replace_value(value->clone());
	((agentppTestSharedTableRowStatus*)other)->set_reference_to_table(my_table);
	//--AgentGen BEGIN=agentppTestSharedTableRowStatus::clone
	//--AgentGen END
	return other;
}


//--AgentGen BEGIN=agentppTestSharedTableRowStatus
//--AgentGen END




/**
 *  agentppTestRowCreation
 *
 * "This object can be set to the index of a new row
 * in the agentppTestSharedTable. If a row with the
 * set index already exists, this object will return zero,
 * otherwise it will return the last value set."
 */
 
agentppTestRowCreation::agentppTestRowCreation(const Oidx& id):
    MibLeaf(id, READWRITE, new Gauge32(0), VMODE_DEFAULT){
	//--AgentGen BEGIN=agentppTestRowCreation::agentppTestRowCreation
	//--AgentGen END

}

agentppTestRowCreation::~agentppTestRowCreation()
{

	//--AgentGen BEGIN=agentppTestRowCreation::~agentppTestRowCreation
	//--AgentGen END

}

MibEntryPtr agentppTestRowCreation::clone()
{
	MibEntryPtr other = new agentppTestRowCreation(oid);
	((agentppTestRowCreation*)other)->replace_value(value->clone());
	((agentppTestRowCreation*)other)->set_reference_to_table(my_table);
	//--AgentGen BEGIN=agentppTestRowCreation::clone
	//--AgentGen END
	return other;
}


//--AgentGen BEGIN=agentppTestRowCreation
//--AgentGen END




/**
 *  agentppTestSparseCol1
 *
 * "By setting this object to its current value the object
 * becomes notAccessible allowing testing of
 * sparse table implementation."
 */
 
agentppTestSparseCol1::agentppTestSparseCol1(const Oidx& id):
    MibLeaf(id, READCREATE, new SnmpInt32(1), VMODE_DEFAULT){
	//--AgentGen BEGIN=agentppTestSparseCol1::agentppTestSparseCol1
	//--AgentGen END

}

agentppTestSparseCol1::~agentppTestSparseCol1()
{

	//--AgentGen BEGIN=agentppTestSparseCol1::~agentppTestSparseCol1
	//--AgentGen END

}

MibEntryPtr agentppTestSparseCol1::clone()
{
	MibEntryPtr other = new agentppTestSparseCol1(oid);
	((agentppTestSparseCol1*)other)->replace_value(value->clone());
	((agentppTestSparseCol1*)other)->set_reference_to_table(my_table);
	//--AgentGen BEGIN=agentppTestSparseCol1::clone
	//--AgentGen END
	return other;
}

void agentppTestSparseCol1::get_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppTestSparseCol1::get_request
	//--AgentGen END
	MibLeaf::get_request(req, ind);
}

long agentppTestSparseCol1::get_state()
{
	//--AgentGen BEGIN=agentppTestSparseCol1::get_state
	//--AgentGen END
	return (long)*((SnmpInt32*)value);
}
	
void agentppTestSparseCol1::set_state(long l)
{
	//--AgentGen BEGIN=agentppTestSparseCol1::set_state
	//--AgentGen END
	*((SnmpInt32*)value) = l;
}
	

int agentppTestSparseCol1::set(const Vbx& vb)
{
	//--AgentGen BEGIN=agentppTestSparseCol1::set
	SnmpInt32 v;
	vb.get_value(v);
	if (v == get_state())
	    set_access(NOACCESS);
	//--AgentGen END
	return MibLeaf::set(vb);
}

bool agentppTestSparseCol1::value_ok(const Vbx& vb)
{
	//--AgentGen BEGIN=agentppTestSparseCol1::value_ok
	//--AgentGen END
	return TRUE;
}

int agentppTestSparseCol1::prepare_set_request(Request* req, int& ind)
{
	int status;
	if ((status = MibLeaf::prepare_set_request(req, ind)) !=
	    SNMP_ERROR_SUCCESS) return status;

	//--AgentGen BEGIN=agentppTestSparseCol1::prepare_set_request
	//--AgentGen END
	return SNMP_ERROR_SUCCESS;
}

//--AgentGen BEGIN=agentppTestSparseCol1
//--AgentGen END




/**
 *  agentppTestSparseCol2
 *
 * "By setting this object to its current value the object
 * becomes notAccessible allowing testing of
 * sparse table implementation."
 */
 
agentppTestSparseCol2::agentppTestSparseCol2(const Oidx& id):
    MibLeaf(id, READCREATE, new Gauge32(2), VMODE_DEFAULT){
	//--AgentGen BEGIN=agentppTestSparseCol2::agentppTestSparseCol2
	//--AgentGen END

}

agentppTestSparseCol2::~agentppTestSparseCol2()
{

	//--AgentGen BEGIN=agentppTestSparseCol2::~agentppTestSparseCol2
	//--AgentGen END

}

MibEntryPtr agentppTestSparseCol2::clone()
{
	MibEntryPtr other = new agentppTestSparseCol2(oid);
	((agentppTestSparseCol2*)other)->replace_value(value->clone());
	((agentppTestSparseCol2*)other)->set_reference_to_table(my_table);
	//--AgentGen BEGIN=agentppTestSparseCol2::clone
	//--AgentGen END
	return other;
}

void agentppTestSparseCol2::get_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppTestSparseCol2::get_request
	//--AgentGen END
	MibLeaf::get_request(req, ind);
}

unsigned long agentppTestSparseCol2::get_state()
{
	//--AgentGen BEGIN=agentppTestSparseCol2::get_state
	//--AgentGen END
	return (unsigned long)*((Gauge32*)value);
}

void agentppTestSparseCol2::set_state(unsigned long l)
{
	//--AgentGen BEGIN=agentppTestSparseCol2::set_state
	//--AgentGen END
	*((Gauge32*)value) = l;
}


int agentppTestSparseCol2::set(const Vbx& vb)
{
	//--AgentGen BEGIN=agentppTestSparseCol2::set
	Gauge32 v;
	vb.get_value(v);
	if (v == get_state())
	    set_access(NOACCESS);
	//--AgentGen END
	return MibLeaf::set(vb);
}

bool agentppTestSparseCol2::value_ok(const Vbx& vb)
{
	//--AgentGen BEGIN=agentppTestSparseCol2::value_ok
	//--AgentGen END
	return TRUE;
}

int agentppTestSparseCol2::prepare_set_request(Request* req, int& ind)
{
	int status;
	if ((status = MibLeaf::prepare_set_request(req, ind)) !=
	    SNMP_ERROR_SUCCESS) return status;

	//--AgentGen BEGIN=agentppTestSparseCol2::prepare_set_request
	//--AgentGen END
	return SNMP_ERROR_SUCCESS;
}

//--AgentGen BEGIN=agentppTestSparseCol2
//--AgentGen END




/**
 *  agentppTestSparseCol3
 *
 * "By setting this object to its current value the object
 * becomes notAccessible allowing testing of
 * sparse table implementation."
 */
 
agentppTestSparseCol3::agentppTestSparseCol3(const Oidx& id):
    MibLeaf(id, READCREATE, new OctetStr("3"), VMODE_DEFAULT){
	//--AgentGen BEGIN=agentppTestSparseCol3::agentppTestSparseCol3
	//--AgentGen END

}

agentppTestSparseCol3::~agentppTestSparseCol3()
{

	//--AgentGen BEGIN=agentppTestSparseCol3::~agentppTestSparseCol3
	//--AgentGen END

}

MibEntryPtr agentppTestSparseCol3::clone()
{
	MibEntryPtr other = new agentppTestSparseCol3(oid);
	((agentppTestSparseCol3*)other)->replace_value(value->clone());
	((agentppTestSparseCol3*)other)->set_reference_to_table(my_table);
	//--AgentGen BEGIN=agentppTestSparseCol3::clone
	//--AgentGen END
	return other;
}

void agentppTestSparseCol3::get_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppTestSparseCol3::get_request
	//--AgentGen END
	MibLeaf::get_request(req, ind);
}

OctetStr agentppTestSparseCol3::get_state() {
	//--AgentGen BEGIN=agentppTestSparseCol3::get_state
	//--AgentGen END
	return *((OctetStr*)value);
}
void agentppTestSparseCol3::set_state(const OctetStr& s)
{
	//--AgentGen BEGIN=agentppTestSparseCol3::set_state
	//--AgentGen END
	*((OctetStr*)value) = s;
}


int agentppTestSparseCol3::set(const Vbx& vb)
{
	//--AgentGen BEGIN=agentppTestSparseCol3::set
	OctetStr v;
	vb.get_value(v);
	if (v == get_state())
	    set_access(NOACCESS);
	//--AgentGen END
	return MibLeaf::set(vb);
}

bool agentppTestSparseCol3::value_ok(const Vbx& vb)
{
	//--AgentGen BEGIN=agentppTestSparseCol3::value_ok
	//--AgentGen END
	return TRUE;
}

int agentppTestSparseCol3::prepare_set_request(Request* req, int& ind)
{
	int status;
	if ((status = MibLeaf::prepare_set_request(req, ind)) !=
	    SNMP_ERROR_SUCCESS) return status;

	Vb vb(req->get_value(ind));
	OctetStr v;
	vb.get_value(v);
	if (!(((v.len() >= 0) && (v.len() <= 255))))
		 return SNMP_ERROR_WRONG_LENGTH;
	//--AgentGen BEGIN=agentppTestSparseCol3::prepare_set_request
	//--AgentGen END
	return SNMP_ERROR_SUCCESS;
}

//--AgentGen BEGIN=agentppTestSparseCol3
//--AgentGen END




/**
 *  agentppTestSparseRowStatus
 *
 * "Use this column to create a row in the test table."
 */
 
agentppTestSparseRowStatus::agentppTestSparseRowStatus(const Oidx& id):
    snmpRowStatus(id, READCREATE)
{
	//--AgentGen BEGIN=agentppTestSparseRowStatus::agentppTestSparseRowStatus
	//--AgentGen END

}

agentppTestSparseRowStatus::~agentppTestSparseRowStatus()
{

	//--AgentGen BEGIN=agentppTestSparseRowStatus::~agentppTestSparseRowStatus
	//--AgentGen END

}

MibEntryPtr agentppTestSparseRowStatus::clone()
{
	MibEntryPtr other = new agentppTestSparseRowStatus(oid);
	((agentppTestSparseRowStatus*)other)->replace_value(value->clone());
	((agentppTestSparseRowStatus*)other)->set_reference_to_table(my_table);
	//--AgentGen BEGIN=agentppTestSparseRowStatus::clone
	//--AgentGen END
	return other;
}

long agentppTestSparseRowStatus::get_state()
{
	//--AgentGen BEGIN=agentppTestSparseRowStatus::get_state
	//--AgentGen END
	return (long)*((SnmpInt32*)value);
}
	
void agentppTestSparseRowStatus::set_state(long l)
{
	//--AgentGen BEGIN=agentppTestSparseRowStatus::set_state
	//--AgentGen END
	*((SnmpInt32*)value) = l;
}
	

int agentppTestSparseRowStatus::set(const Vbx& vb)
{
	//--AgentGen BEGIN=agentppTestSparseRowStatus::set
	//--AgentGen END
	return snmpRowStatus::set(vb);
}

int agentppTestSparseRowStatus::prepare_set_request(Request* req, int& ind)
{
	int status;
	if ((status = snmpRowStatus::prepare_set_request(req, ind)) !=
	    SNMP_ERROR_SUCCESS) return status;

	//--AgentGen BEGIN=agentppTestSparseRowStatus::prepare_set_request
	//--AgentGen END
	return SNMP_ERROR_SUCCESS;
}

//--AgentGen BEGIN=agentppTestSparseRowStatus
//--AgentGen END



// Tables


/**
 *  agentppTestSharedEntry
 *
 * "A row of a shared table. Each row is allocated and
 * registered in random intervals."
 */
 
agentppTestSharedEntry* agentppTestSharedEntry::instance = 0;

const index_info indAgentppTestSharedEntry[1] = {
	{ sNMP_SYNTAX_INT, FALSE, 1, 1 }};

agentppTestSharedEntry::agentppTestSharedEntry():
   MibTable(oidAgentppTestSharedEntry, indAgentppTestSharedEntry, 1)
{
	// This table object is a singleton. In order to access it use
	// the static pointer agentppTestSharedEntry::instance.
	instance = this;

	add_col(new agentppTestSharedTableCreationTime(colAgentppTestSharedTableCreationTime));
	add_col(new agentppTestSharedTableDelay(colAgentppTestSharedTableDelay));
	add_col(new agentppTestSharedTableSession(colAgentppTestSharedTableSession));
	add_col(new agentppTestSharedTableRowStatus(colAgentppTestSharedTableRowStatus));
	//--AgentGen BEGIN=agentppTestSharedEntry::agentppTestSharedEntry
	//--AgentGen END
}

agentppTestSharedEntry::~agentppTestSharedEntry()
{
	//--AgentGen BEGIN=agentppTestSharedEntry::~agentppTestSharedEntry
	//--AgentGen END
	// clear singleton reference
	agentppTestSharedEntry::instance = 0;
}

//--AgentGen BEGIN=agentppTestSharedEntry
//--AgentGen END


/**
 *  agentppTestSessionsEntry
 *
 * "A row of this table is created by each subagent
 * session that implements the AGENTPP-TEST-MIB."
 */
 
agentppTestSessionsEntry* agentppTestSessionsEntry::instance = 0;

const index_info indAgentppTestSessionsEntry[1] = {
	{ sNMP_SYNTAX_INT, FALSE, 1, 1 }};

agentppTestSessionsEntry::agentppTestSessionsEntry():
   MibTable(oidAgentppTestSessionsEntry, indAgentppTestSessionsEntry, 1)
{
	// This table object is a singleton. In order to access it use
	// the static pointer agentppTestSessionsEntry::instance.
	instance = this;

	add_col(new agentppTestRowCreation(colAgentppTestRowCreation));
	//--AgentGen BEGIN=agentppTestSessionsEntry::agentppTestSessionsEntry
	//--AgentGen END
}

agentppTestSessionsEntry::~agentppTestSessionsEntry()
{
	//--AgentGen BEGIN=agentppTestSessionsEntry::~agentppTestSessionsEntry
	//--AgentGen END
	// clear singleton reference
	agentppTestSessionsEntry::instance = 0;
}

//--AgentGen BEGIN=agentppTestSessionsEntry
//--AgentGen END


/**
 *  agentppTestSparseEntry
 *
 * "A row of a sparese table can be created and
 * deleted via its row status column."
 */
 
agentppTestSparseEntry* agentppTestSparseEntry::instance = 0;

const index_info indAgentppTestSparseEntry[1] = {
	{ sNMP_SYNTAX_OCTETS, FALSE, 0, 255 }
};

agentppTestSparseEntry::agentppTestSparseEntry():
   MibTable(oidAgentppTestSparseEntry, indAgentppTestSparseEntry, 1)
{
	// This table object is a singleton. In order to access it use
	// the static pointer agentppTestSparseEntry::instance.
	instance = this;

	add_col(new agentppTestSparseCol1(colAgentppTestSparseCol1));
	add_col(new agentppTestSparseCol2(colAgentppTestSparseCol2));
	add_col(new agentppTestSparseCol3(colAgentppTestSparseCol3));
	add_col(new agentppTestSparseRowStatus(colAgentppTestSparseRowStatus));
	//--AgentGen BEGIN=agentppTestSparseEntry::agentppTestSparseEntry
	//--AgentGen END
}

agentppTestSparseEntry::~agentppTestSparseEntry()
{
	//--AgentGen BEGIN=agentppTestSparseEntry::~agentppTestSparseEntry
	//--AgentGen END
	// clear singleton reference
	agentppTestSparseEntry::instance = 0;
}

void agentppTestSparseEntry::get_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppTestSparseEntry::get_request
	//--AgentGen END
	MibTable::get_request(req, ind);
}

int agentppTestSparseEntry::prepare_set_request(Request* req, int& ind)
{
	//--AgentGen BEGIN=agentppTestSparseEntry::prepare_set_request
	//--AgentGen END
	return MibTable::prepare_set_request(req, ind);
}

//--AgentGen BEGIN=agentppTestSparseEntry
//--AgentGen END

// Notifications

// Group
agentpp_test_mib::agentpp_test_mib():
   MibGroup("1.3.6.1.4.1.4976.6.3", "agentpp_test_mib")
{
	//--AgentGen BEGIN=agentpp_test_mib::agentpp_test_mib
	//--AgentGen END
	add(new agentppTestTimeout());
	add(new agentppTestSharedEntry());
	add(new agentppTestSessionsEntry());
	add(new agentppTestSparseEntry());
	//--AgentGen BEGIN=agentpp_test_mib::agentpp_test_mib:post
	//--AgentGen END
}

//--AgentGen BEGIN=agentpp_test_mib
//--AgentGen END



//--AgentGen BEGIN=_CLASSES
//--AgentGen END


#ifdef AGENTPP_NAMESPACE
}
#endif


//--AgentGen BEGIN=_END
//--AgentGen END

#endif
