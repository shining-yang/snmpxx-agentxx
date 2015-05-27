/*_############################################################################
  _## 
  _##  agentpp_test_mib.cpp  
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
 

//--AgentGen BEGIN=_BEGIN
//--AgentGen END

#ifndef _agentpp_test_mib_cpp
#define _agentpp_test_mib_cpp

#include <agentpp_test_mib.h>

//--AgentGen BEGIN=_INCLUDE
//--AgentGen END

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

static const char *loggerModuleName = "agentx++.agentpp_test_mib";


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
}

void agentppTestTimeout::get_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppTestTimeout::get_request
	//--AgentGen END
	MibLeaf::get_request(req, ind);
}

unsigned long agentppTestTimeout::get_state()
{
	//--AgentGen BEGIN=agentppTestTimeout::get_state
	//--AgentGen END
	return (unsigned long)*((Gauge32*)value);
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
        if (timeout > 0) {
                Thread::sleep(timeout);
        }
	//--AgentGen END
	return MibLeaf::set(vb);
}

int agentppTestTimeout::commit_set_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppTestTimeout::commit_set_request
	//--AgentGen END
	return MibLeaf::commit_set_request(req, ind);
}

void agentppTestTimeout::cleanup_set_request(Request* req, int& ind)
{
	//--AgentGen BEGIN=agentppTestTimeout::cleanup_set_request
	//--AgentGen END
	MibLeaf::cleanup_set_request(req, ind);
}

int agentppTestTimeout::undo_set_request(Request* req, int& ind)
{
	//--AgentGen BEGIN=agentppTestTimeout::undo_set_request
	//--AgentGen END
	return MibLeaf::undo_set_request(req, ind);
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

agentppTestIndexAllocStrategy* agentppTestIndexAllocStrategy::instance = 0;


agentppTestIndexAllocStrategy::agentppTestIndexAllocStrategy():
    MibLeaf(oidAgentppTestIndexAllocStrategy, READWRITE, new SnmpInt32(2)){
	// This leaf object is a singleton. In order to access it use
	// the static pointer agentppTestTimeout::instance.
	instance = this;
	//--AgentGen BEGIN=agentppTestIndexAllocStrategy::agentppTestIndexAllocStrategy
	//--AgentGen END

}

agentppTestIndexAllocStrategy::~agentppTestIndexAllocStrategy()
{

	//--AgentGen BEGIN=agentppTestIndexAllocStrategy::~agentppTestIndexAllocStrategy
	//--AgentGen END
}

void agentppTestIndexAllocStrategy::get_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppTestIndexAllocStrategy::get_request
	//--AgentGen END
	MibLeaf::get_request(req, ind);
}

int agentppTestIndexAllocStrategy::get_state()
{
	//--AgentGen BEGIN=agentppTestIndexAllocStrategy::get_state
	//--AgentGen END
	return (int)*((SnmpInt32*)value);
}

void agentppTestIndexAllocStrategy::set_state(int l)
{
	//--AgentGen BEGIN=agentppTestTimeout::set_state
	//--AgentGen END
	*((SnmpInt32*)value) = l;
}


int agentppTestIndexAllocStrategy::set(const Vbx& vb)
{
	//--AgentGen BEGIN=agentppTestTimeout::set
	int strategy;
	vb.get_value(strategy);
        if (strategy > 0) {
           agentx_index_strategy s = static_cast<agentx_index_strategy>(strategy);
           agentppTestSharedEntry::instance->set_index_strategy(s); 
        }
	//--AgentGen END
	return MibLeaf::set(vb);
}

int agentppTestIndexAllocStrategy::commit_set_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppTestTimeout::commit_set_request
	//--AgentGen END
	return MibLeaf::commit_set_request(req, ind);
}

void agentppTestIndexAllocStrategy::cleanup_set_request(Request* req, int& ind)
{
	//--AgentGen BEGIN=agentppTestTimeout::cleanup_set_request
	//--AgentGen END
	MibLeaf::cleanup_set_request(req, ind);
}

int agentppTestIndexAllocStrategy::undo_set_request(Request* req, int& ind)
{
	//--AgentGen BEGIN=agentppTestTimeout::undo_set_request
	//--AgentGen END
	return MibLeaf::undo_set_request(req, ind);
}

bool agentppTestIndexAllocStrategy::value_ok(const Vbx& vb)
{
	int v;
	vb.get_value(v);
	if (!(((v > 0) && (v <= 5))))
		 return FALSE;
	//--AgentGen BEGIN=agentppTestTimeout::value_ok
	//--AgentGen END
	return TRUE;
}

int agentppTestIndexAllocStrategy::prepare_set_request(Request* req, int& ind)
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

void agentppTestSharedTableDelay::get_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppTestSharedTableDelay::get_request
	delay_execution();
	//--AgentGen END
	MibLeaf::get_request(req, ind);
}

long agentppTestSharedTableDelay::get_state()
{
	//--AgentGen BEGIN=agentppTestSharedTableDelay::get_state
	//--AgentGen END
	return (long)*((SnmpInt32*)value);
}
	
void agentppTestSharedTableDelay::set_state(long l)
{
	//--AgentGen BEGIN=agentppTestSharedTableDelay::set_state
	//--AgentGen END
	*((SnmpInt32*)value) = l;
}
	

int agentppTestSharedTableDelay::set(const Vbx& vb)
{
	//--AgentGen BEGIN=agentppTestSharedTableDelay::set
	//--AgentGen END
	return MibLeaf::set(vb);
}

int agentppTestSharedTableDelay::commit_set_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppTestSharedTableDelay::commit_set_request
	delay_execution();
	//--AgentGen END
	return MibLeaf::commit_set_request(req, ind);
}

void agentppTestSharedTableDelay::cleanup_set_request(Request* req, int& ind)
{
	//--AgentGen BEGIN=agentppTestSharedTableDelay::cleanup_set_request
	//--AgentGen END
	MibLeaf::cleanup_set_request(req, ind);
}

int agentppTestSharedTableDelay::undo_set_request(Request* req, int& ind)
{
	//--AgentGen BEGIN=agentppTestSharedTableDelay::undo_set_request
	//--AgentGen END
	return MibLeaf::undo_set_request(req, ind);
}

bool agentppTestSharedTableDelay::value_ok(const Vbx& vb)
{
	long v;
	vb.get_value(v);
	if (!(((v >= 0) && (v <= 6000))))
		 return FALSE;
	//--AgentGen BEGIN=agentppTestSharedTableDelay::value_ok
	//--AgentGen END
	return TRUE;
}

int agentppTestSharedTableDelay::prepare_set_request(Request* req, int& ind)
{
	int status;
	if ((status = MibLeaf::prepare_set_request(req, ind)) !=
	    SNMP_ERROR_SUCCESS) return status;

	//--AgentGen BEGIN=agentppTestSharedTableDelay::prepare_set_request
	//--AgentGen END
	return SNMP_ERROR_SUCCESS;
}

//--AgentGen BEGIN=agentppTestSharedTableDelay
void agentppTestSharedTableDelay::delay_execution() 
{
	unsigned long timeout = get_state()*10;
	if (timeout == 0) return;
#ifndef WIN32
	struct timeval tvptr;
	tvptr.tv_sec  = timeout/1000;    // wait up to sec seconds
	tvptr.tv_usec = (timeout%1000)*1000;

	select(0, NULL, NULL, NULL, &tvptr);
#else
	Sleep(timeout);
#endif
	
}
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
	set_state(rowActive);
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

long agentppTestSharedTableRowStatus::get_state()
{
	//--AgentGen BEGIN=agentppTestSharedTableRowStatus::get_state
	//--AgentGen END
	return (long)*((SnmpInt32*)value);
}
	
void agentppTestSharedTableRowStatus::set_state(long l)
{
	//--AgentGen BEGIN=agentppTestSharedTableRowStatus::set_state
	//--AgentGen END
	*((SnmpInt32*)value) = l;
}
	

int agentppTestSharedTableRowStatus::set(const Vbx& vb)
{
	//--AgentGen BEGIN=agentppTestSharedTableRowStatus::set
	//--AgentGen END
	return snmpRowStatus::set(vb);
}

int agentppTestSharedTableRowStatus::commit_set_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppTestSharedTableRowStatus::commit_set_request
	Vbx vb(req->get_value(ind));
	int l;
	if (vb.get_value(l) != SNMP_CLASS_SUCCESS) return SNMP_ERROR_INCONSIST_VAL;

	switch (l) {
	  case rowCreateAndGo:
	  case rowCreateAndWait:
	    if (!((AgentXSharedTable*)my_table)->
		allocate_index(my_row->get_index())) {
	      return SNMP_ERROR_INCONSIST_VAL;
	    }
	    break;
	} 
	//--AgentGen END
	return snmpRowStatus::commit_set_request(req, ind);
}

void agentppTestSharedTableRowStatus::cleanup_set_request(Request* req, int& ind)
{
	//--AgentGen BEGIN=agentppTestSharedTableRowStatus::cleanup_set_request
	//--AgentGen END
	snmpRowStatus::cleanup_set_request(req, ind);
}

int agentppTestSharedTableRowStatus::undo_set_request(Request* req, int& ind)
{
	//--AgentGen BEGIN=agentppTestSharedTableRowStatus::undo_set_request
	//--AgentGen END
	return snmpRowStatus::undo_set_request(req, ind);
}

int agentppTestSharedTableRowStatus::prepare_set_request(Request* req, int& ind)
{
	int status;
	if ((status = snmpRowStatus::prepare_set_request(req, ind)) !=
	    SNMP_ERROR_SUCCESS) return status;

	//--AgentGen BEGIN=agentppTestSharedTableRowStatus::prepare_set_request
	//--AgentGen END
	return SNMP_ERROR_SUCCESS;
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

void agentppTestRowCreation::get_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppTestRowCreation::get_request
	//--AgentGen END
	MibLeaf::get_request(req, ind);
}

unsigned long agentppTestRowCreation::get_state()
{
	//--AgentGen BEGIN=agentppTestRowCreation::get_state
	//--AgentGen END
	return (unsigned long)*((Gauge32*)value);
}

void agentppTestRowCreation::set_state(unsigned long l)
{
	//--AgentGen BEGIN=agentppTestRowCreation::set_state
	//--AgentGen END
	*((Gauge32*)value) = l;
}


int agentppTestRowCreation::set(const Vbx& vb)
{
	//--AgentGen BEGIN=agentppTestRowCreation::set
	//--AgentGen END
	return MibLeaf::set(vb);
}

int agentppTestRowCreation::commit_set_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppTestRowCreation::commit_set_request
	unsigned long val;
	Vbx vb(req->get_value(ind));
	vb.get_value(val);
	Oidx rowIndex;
	rowIndex += val;
	if (agentppTestSharedEntry::instance->
	    pending_row_ops.find(&rowIndex)) {
	  return SNMP_ERROR_COMITFAIL;	  
	}

	// double check for duplicate row (has already been checked in prepare
	agentppTestSharedEntry::instance->start_synch();
	if (agentppTestSharedEntry::instance->find_index(rowIndex)) {
	  agentppTestSharedEntry::instance->end_synch();
	  return SNMP_ERROR_COMITFAIL;	 
	}
	agentppTestSharedEntry::instance->end_synch();

	// allocate index and then register and add the row in a background thread
	if (!agentppTestSharedEntry::instance->allocate_index(rowIndex)) {
	  *((Gauge32*)value) = 0;
	  return SNMP_ERROR_COMITFAIL;
	}
	else {
	  agentppTestSharedEntry::instance->
	    pending_row_ops.add(new RequestID(req->get_request_id(), rowIndex));
	  // wait until row has been registered at master agent 
	  if (req->wait(AGENTX_DEFAULT_TIMEOUT*1000)) {
	    agentppTestSharedEntry::instance->start_synch();
	    agentppTestSharedEntry::instance->remove_row(rowIndex);
	    agentppTestSharedEntry::instance->end_synch();
	    return SNMP_ERROR_COMITFAIL;
	  }
	}
	//--AgentGen END
	return MibLeaf::commit_set_request(req, ind);
}

void agentppTestRowCreation::cleanup_set_request(Request* req, int& ind)
{
	//--AgentGen BEGIN=agentppTestRowCreation::cleanup_set_request
	//--AgentGen END
	MibLeaf::cleanup_set_request(req, ind);
}

int agentppTestRowCreation::undo_set_request(Request* req, int& ind)
{
	//--AgentGen BEGIN=agentppTestRowCreation::undo_set_request
	//--AgentGen END
	return MibLeaf::undo_set_request(req, ind);
}

bool agentppTestRowCreation::value_ok(const Vbx& vb)
{
	//--AgentGen BEGIN=agentppTestRowCreation::value_ok
	//--AgentGen END
	return TRUE;
}

int agentppTestRowCreation::prepare_set_request(Request* req, int& ind)
{
	int status;
	if ((status = MibLeaf::prepare_set_request(req, ind)) !=
	    SNMP_ERROR_SUCCESS) return status;

	//--AgentGen BEGIN=agentppTestRowCreation::prepare_set_request
	unsigned long val;
	Vbx vb(req->get_value(ind));
	vb.get_value(val);
	Oidx rowIndex;
	rowIndex += val;
	agentppTestSharedEntry::instance->start_synch();
	if (agentppTestSharedEntry::instance->find_index(rowIndex)) {
	  agentppTestSharedEntry::instance->end_synch();
	  return SNMP_ERROR_INCONSIST_VAL;
	}
	agentppTestSharedEntry::instance->end_synch();
	//--AgentGen END
	return SNMP_ERROR_SUCCESS;
}

//--AgentGen BEGIN=agentppTestRowCreation
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

const Oidx indOidsAgentppTestSharedEntry[1] = {
	"1.3.6.1.4.1.4976.6.3.1.3.1"};

agentppTestSharedEntry::agentppTestSharedEntry(const OctetStr& context, SubAgentXMib* mib):
   AgentXSharedTable(oidAgentppTestSharedEntry, indAgentppTestSharedEntry, 1, indOidsAgentppTestSharedEntry, mib, context)
{
	// This table object is a singleton. In order to access it use
	// the static pointer agentppTestSharedEntry::instance.
	instance = this;

	add_col(new DateAndTime(colAgentppTestSharedTableCreationTime, READONLY, VMODE_DEFAULT)
);
	add_col(new agentppTestSharedTableDelay(colAgentppTestSharedTableDelay));
	add_col(new MibLeaf(colAgentppTestSharedTableSession, READONLY, new Gauge32()));
	add_col(new agentppTestSharedTableRowStatus(colAgentppTestSharedTableRowStatus));
	//--AgentGen BEGIN=agentppTestSharedEntry::agentppTestSharedEntry
	replace_col(nAgentppTestSharedTableSession,
		    new MibLeaf(colAgentppTestSharedTableSession, 
				READONLY, 
				new Gauge32(mib->get_session()->get_id()),
				VMODE_DEFAULT));
	    
	//--AgentGen END
}

agentppTestSharedEntry::~agentppTestSharedEntry()
{
	//--AgentGen BEGIN=agentppTestSharedEntry::~agentppTestSharedEntry
	//--AgentGen END
}

void agentppTestSharedEntry::row_added(MibTableRow* row, const Oidx& index, MibTable* src)
{
	// The row 'row' with 'index' has been added to the table.
	//--AgentGen BEGIN=agentppTestSharedEntry::row_added
	Oidx rowIndex(index);
	RequestID* req_id = pending_row_ops.find(&rowIndex);
	if (req_id) {
	  RequestList* req_list = 
	    backReference->get_request_list();
	  if (req_list) {
	    Request* req = req_list->get_request(req_id->get_request_id());
	    if (req) {
	      // signal success
	      LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
	      LOG("agentppTestSharedEntry: row_added: AgentX row successfully registered (index)(request_id)");
	      LOG(rowIndex.get_printable());
	      LOG(req_id->get_request_id());
	      LOG_END;
	      req->notify();
	    }
	  }
	  pending_row_ops.remove(req_id);
	}
	//--AgentGen END
}

void agentppTestSharedEntry::get_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppTestSharedEntry::get_request
	//--AgentGen END
	AgentXSharedTable::get_request(req, ind);
}

int agentppTestSharedEntry::commit_set_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppTestSharedEntry::commit_set_request
	//--AgentGen END
	return AgentXSharedTable::commit_set_request(req, ind);
}

int agentppTestSharedEntry::prepare_set_request(Request* req, int& ind)
{
	//--AgentGen BEGIN=agentppTestSharedEntry::prepare_set_request
	//--AgentGen END
	return AgentXSharedTable::prepare_set_request(req, ind);
}

void agentppTestSharedEntry::cleanup_set_request(Request* req, int& ind)
{
	//--AgentGen BEGIN=agentppTestSharedEntry::cleanup_set_request
	//--AgentGen END
	AgentXSharedTable::cleanup_set_request(req, ind);
}

int agentppTestSharedEntry::undo_set_request(Request* req, int& ind)
{
	//--AgentGen BEGIN=agentppTestSharedEntry::undo_set_request
	//--AgentGen END
	return AgentXSharedTable::undo_set_request(req, ind);
}

void agentppTestSharedEntry::row_delete(MibTableRow* row, const Oidx& index, MibTable* src)
{
	// The row 'row' with 'index' will be deleted.
	//--AgentGen BEGIN=agentppTestSharedEntry::row_delete
	//--AgentGen END
}

void agentppTestSharedEntry::row_init(MibTableRow* row, const Oidx& index, MibTable* src)
{
	// The row 'row' with 'index' has been intialized.
	//--AgentGen BEGIN=agentppTestSharedEntry::row_init
	//--AgentGen END
}

void agentppTestSharedEntry::row_activated(MibTableRow* row, const Oidx& index, MibTable* src)
{
	// The row 'row' with 'index' has been activated.
	//--AgentGen BEGIN=agentppTestSharedEntry::row_activated
	//--AgentGen END
}

void agentppTestSharedEntry::row_deactivated(MibTableRow* row, const Oidx& index, MibTable* src)
{
	// The row 'row' with 'index' has been deactivated.
	//--AgentGen BEGIN=agentppTestSharedEntry::row_deactivated
	//--AgentGen END
}

//--AgentGen BEGIN=agentppTestSharedEntry

void agentppTestSharedEntry::index_allocated(const Oidx& ind, int err, int errind, Vbx*, u_int)
{
	// default behavior is to add the row when allocation succeeded
	if (err == AGENTX_OK) {
		add_row(ind);
	}
}



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

const Oidx indOidsAgentppTestSessionsEntry[1] = {
	"1.3.6.1.4.1.4976.6.3.1.4.1"};

agentppTestSessionsEntry::agentppTestSessionsEntry(const OctetStr& context, SubAgentXMib* mib):
   AgentXSharedTable(oidAgentppTestSessionsEntry, indAgentppTestSessionsEntry, 1, indOidsAgentppTestSessionsEntry, mib, context)
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
}

void agentppTestSessionsEntry::row_added(MibTableRow* row, const Oidx& index, MibTable* src)
{
	// The row 'row' with 'index' has been added to the table.
	//--AgentGen BEGIN=agentppTestSessionsEntry::row_added
	//--AgentGen END
}

void agentppTestSessionsEntry::get_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppTestSessionsEntry::get_request
	//--AgentGen END
	AgentXSharedTable::get_request(req, ind);
}

int agentppTestSessionsEntry::commit_set_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppTestSessionsEntry::commit_set_request
	//--AgentGen END
	return AgentXSharedTable::commit_set_request(req, ind);
}

int agentppTestSessionsEntry::prepare_set_request(Request* req, int& ind)
{
	//--AgentGen BEGIN=agentppTestSessionsEntry::prepare_set_request
	//--AgentGen END
	return AgentXSharedTable::prepare_set_request(req, ind);
}

void agentppTestSessionsEntry::cleanup_set_request(Request* req, int& ind)
{
	//--AgentGen BEGIN=agentppTestSessionsEntry::cleanup_set_request
	//--AgentGen END
	AgentXSharedTable::cleanup_set_request(req, ind);
}

int agentppTestSessionsEntry::undo_set_request(Request* req, int& ind)
{
	//--AgentGen BEGIN=agentppTestSessionsEntry::undo_set_request
	//--AgentGen END
	return AgentXSharedTable::undo_set_request(req, ind);
}

void agentppTestSessionsEntry::row_delete(MibTableRow* row, const Oidx& index, MibTable* src)
{
	// The row 'row' with 'index' will be deleted.
	//--AgentGen BEGIN=agentppTestSessionsEntry::row_delete
	//--AgentGen END
}

void agentppTestSessionsEntry::row_init(MibTableRow* row, const Oidx& index, MibTable* src)
{
	// The row 'row' with 'index' has been intialized.
	//--AgentGen BEGIN=agentppTestSessionsEntry::row_init
	//--AgentGen END
}

void agentppTestSessionsEntry::row_activated(MibTableRow* row, const Oidx& index, MibTable* src)
{
	// The row 'row' with 'index' has been activated.
	//--AgentGen BEGIN=agentppTestSessionsEntry::row_activated
	//--AgentGen END
}

void agentppTestSessionsEntry::row_deactivated(MibTableRow* row, const Oidx& index, MibTable* src)
{
	// The row 'row' with 'index' has been deactivated.
	//--AgentGen BEGIN=agentppTestSessionsEntry::row_deactivated
	//--AgentGen END
}

//--AgentGen BEGIN=agentppTestSessionsEntry
//--AgentGen END

// Notifications

// Group
agentpp_test_mib::agentpp_test_mib(const OctetStr& context, SubAgentXMib* mib):
   MibGroup("1.3.6.1.4.1.4976.6.3", "agentpp_test_mib")
{
	//--AgentGen BEGIN=agentpp_test_mib::agentpp_test_mib
	//--AgentGen END
	add(new agentppTestTimeout());
	add(new agentppTestSharedEntry(context, mib));
	add(new agentppTestSessionsEntry(context, mib));
        add(new agentppTestIndexAllocStrategy());
	//--AgentGen BEGIN=agentpp_test_mib::agentpp_test_mib:post

	Oidx sessionIndex;
	sessionIndex += mib->get_session()->get_id();
	agentppTestSessionsEntry::instance->add_row(sessionIndex);
	    
	//--AgentGen END
}

//--AgentGen BEGIN=agentpp_test_mib
//--AgentGen END




#ifdef AGENTPP_NAMESPACE
}
#endif


//--AgentGen BEGIN=_END
//--AgentGen END

#endif
