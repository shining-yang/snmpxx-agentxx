/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - agentpp_config_mib.cpp  
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

//--AgentGen BEGIN=_BEGIN
#ifdef ___NEVER_DEFINE_THIS___
//--AgentGen END

#ifndef _agentpp_config_mib_cpp
#define _agentpp_config_mib_cpp

#include <agent_pp/agentpp_config_mib.h>
#include <agent_pp/module_includes.h>

//--AgentGen BEGIN=_INCLUDE
#endif
#endif
#ifndef _agentpp_config_mib_cpp
#define _agentpp_config_mib_cpp

#include <agent_pp/agentpp_config_mib.h>
#include <agent_pp/snmp_community_mib.h>
#include <agent_pp/snmp_textual_conventions.h>
#include <snmp_pp/log.h>

#ifdef _SNMPv3
//--AgentGen END


#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

static const char *loggerModuleName = "agent++.agentpp_config_mib";

//--AgentGen BEGIN=_UTIL_CLASSES

#ifndef _NO_THREADS
void OperationTask::run() 
{
    Mib* mib = ((agentppCfgStorageEntry*)initiator->my_table)->get_mib();
    if (!mib) {
	LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
	LOG("agentppCfgStorageEntry: Mib instance not set, cannot start operation");
	LOG_END;
	return;
    }
    MibTableRow* row = initiator->my_row;
    int format = ((agentppCfgStorageFormat*)
		  row->get_nth(nAgentppCfgStorageFormat))->get_state();
    NS_SNMP OctetStr path = ((agentppCfgStoragePath*)
		     row->get_nth(nAgentppCfgStoragePath))->get_state();
    switch (operation) {
	case agentppCfgStorageOperation::e_store: {
	    if (mib->save(format, path)) {
		((TimeStamp*)row->
		 get_nth(nAgentppCfgStorageLastStore))->update();
		LOG_BEGIN(loggerModuleName, INFO_LOG | 1);
		LOG("agentppCfgStorageEntry: config saved successfully (path)");
		LOG(path.get_printable());
		LOG_END;
	    }
	    break;
	}
	case agentppCfgStorageOperation::e_restore: {
	    if (mib->load(format, path)) {
		((TimeStamp*)row->
		 get_nth(nAgentppCfgStorageLastRestore))->update();
		LOG_BEGIN(loggerModuleName, INFO_LOG | 1);
		LOG("agentppCfgStorageEntry: config loaded successfully (path)");
		LOG(path.get_printable());
		LOG_END;
	    }
	    break;
	}
	default: 
	    break;
    }
    initiator->set_state(agentppCfgStorageOperation::e_idle); 
}
#endif
//--AgentGen END

// Scalar Objects


/**
 *  agentppCfgSecSrcAddrValidation
 *
 * "Specifies whether SNMPv1/v2c source address
 * validation via the snmpTargetAddrExtTable and
 * the snmpCommunityTable is enabled or disabled.

 * If the value of this object is notAvailable(3), then at
 * least one of the necessary MIB modules are not
 * implemented for this agent instance and an attempt
 * to set this object's value to enabled(1) or disabled(2)
 * will result in a wrongValue error."
 */
 
agentppCfgSecSrcAddrValidation* agentppCfgSecSrcAddrValidation::instance = 0;


agentppCfgSecSrcAddrValidation::agentppCfgSecSrcAddrValidation():
    MibLeaf(oidAgentppCfgSecSrcAddrValidation, READWRITE, new SnmpInt32()){
	// This leaf object is a singleton. In order to access it use
	// the static pointer agentppCfgSecSrcAddrValidation::instance.
	instance = this;
	//--AgentGen BEGIN=agentppCfgSecSrcAddrValidation::agentppCfgSecSrcAddrValidation
	//--AgentGen END

}

agentppCfgSecSrcAddrValidation::~agentppCfgSecSrcAddrValidation()
{

	//--AgentGen BEGIN=agentppCfgSecSrcAddrValidation::~agentppCfgSecSrcAddrValidation
	//--AgentGen END

	// clear singleton reference
	agentppCfgSecSrcAddrValidation::instance = 0;
}

void agentppCfgSecSrcAddrValidation::get_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppCfgSecSrcAddrValidation::get_request
	if (!snmpCommunityEntry::instance) {
	    set_state(3);
	}
	else if (Mib::instance->get_request_list()->get_address_validation()) {
	    set_state(1);
	}
	else {
	    set_state(2);
	}
	//--AgentGen END
	MibLeaf::get_request(req, ind);
}

long agentppCfgSecSrcAddrValidation::get_state()
{
	//--AgentGen BEGIN=agentppCfgSecSrcAddrValidation::get_state
	//--AgentGen END
	return (long)*((SnmpInt32*)value);
}
	
void agentppCfgSecSrcAddrValidation::set_state(long l)
{
	//--AgentGen BEGIN=agentppCfgSecSrcAddrValidation::set_state
	//--AgentGen END
	*((SnmpInt32*)value) = l;
}
	

int agentppCfgSecSrcAddrValidation::set(const Vbx& vb)
{
	//--AgentGen BEGIN=agentppCfgSecSrcAddrValidation::set
	int v;
	if (vb.get_value(v) != SNMP_CLASS_SUCCESS)
	    return SNMP_ERROR_WRONG_TYPE;
	if (v == 1)
	    Mib::instance->get_request_list()->set_address_validation(TRUE);
	else
	    Mib::instance->get_request_list()->set_address_validation(FALSE);	    
	//--AgentGen END
	return MibLeaf::set(vb);
}

bool agentppCfgSecSrcAddrValidation::value_ok(const Vbx& vb)
{
	long v;
	if (vb.get_value(v) != SNMP_CLASS_SUCCESS)
	    return FALSE;
	if ((get_state() == 3) && (v == 1)) {
	    return FALSE;
	}
	if ((v != 1) 
	     && (v != 2) && (v != 3) 
	    ) return FALSE;
	//--AgentGen BEGIN=agentppCfgSecSrcAddrValidation::value_ok
	//--AgentGen END
	return TRUE;
}

//--AgentGen BEGIN=agentppCfgSecSrcAddrValidation
//--AgentGen END



// Columnar Objects


/**
 *  agentppCfgStoragePath
 *
 * "The path to the configuration on the agent's file
 * system. Depending on the persistent storage type,
 * the path can be a directory or a file."
 */
 
agentppCfgStoragePath::agentppCfgStoragePath(const Oidx& id):
    SnmpDisplayString(id, READCREATE, new NS_SNMP OctetStr()){
	//--AgentGen BEGIN=agentppCfgStoragePath::agentppCfgStoragePath
	//--AgentGen END

}

agentppCfgStoragePath::~agentppCfgStoragePath()
{

	//--AgentGen BEGIN=agentppCfgStoragePath::~agentppCfgStoragePath
	//--AgentGen END

}

MibEntryPtr agentppCfgStoragePath::clone()
{
	MibEntryPtr other = new agentppCfgStoragePath(oid);
	((agentppCfgStoragePath*)other)->replace_value(value->clone());
	((agentppCfgStoragePath*)other)->set_reference_to_table(my_table);
	//--AgentGen BEGIN=agentppCfgStoragePath::clone
	//--AgentGen END
	return other;
}

void agentppCfgStoragePath::get_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppCfgStoragePath::get_request
	//--AgentGen END
	SnmpDisplayString::get_request(req, ind);
}

NS_SNMP OctetStr agentppCfgStoragePath::get_state() {
	//--AgentGen BEGIN=agentppCfgStoragePath::get_state
	//--AgentGen END
	return *((NS_SNMP OctetStr*)value);
}
void agentppCfgStoragePath::set_state(const NS_SNMP OctetStr& s)
{
	//--AgentGen BEGIN=agentppCfgStoragePath::set_state
	//--AgentGen END
	*((NS_SNMP OctetStr*)value) = s;
}

int agentppCfgStoragePath::prepare_set_request(Request* req, int& ind)
{
	int status;
	if ((status = SnmpDisplayString::prepare_set_request(req, ind)) !=
	    SNMP_ERROR_SUCCESS) return status;

	Vb vb(req->get_value(ind));
	NS_SNMP OctetStr v;
	if (vb.get_value(v) != SNMP_CLASS_SUCCESS)
	    return SNMP_ERROR_WRONG_TYPE;
	if (!(((v.len() >= 0) && (v.len() <= 255))))
		 return SNMP_ERROR_WRONG_LENGTH;
	//--AgentGen BEGIN=agentppCfgStoragePath::prepare_set_request
	if (((agentppCfgStorageEntry*)my_table)->is_secure_paths()) {
	    if ((v.len() == 0) || (v[0] == '/') || (v[0] == '\\')) {
		return SNMP_ERROR_BAD_VALUE;
	    }
	    for (unsigned int i=0; i<v.len(); i++) {
		if (v[i] == '.') {
		    return SNMP_ERROR_BAD_VALUE;
		}
	    }
	    if ((v[v.len()-1] != '/') && (v[v.len()-1] != '\\')) {
		return SNMP_ERROR_BAD_VALUE;
	    }
	}
#ifndef WIN32
	status = mkdir(v.get_printable(), 700);
	if ((status == -1) && (errno != EEXIST)) {
	    return SNMP_ERROR_BAD_VALUE;
	}
	else if (status != -1) {
	    rmdir(v.get_printable());
	}
#endif
	//--AgentGen END
	return SNMP_ERROR_SUCCESS;
}

//--AgentGen BEGIN=agentppCfgStoragePath
int agentppCfgStoragePath::commit_set_request(Request* req, int ind)
{
#ifndef WIN32
	Vb vb(req->get_value(ind));
	NS_SNMP OctetStr v;
	vb.get_value(v);
	int status = mkdir(v.get_printable(), S_IRWXU);
	if ((status == -1) && (errno != EEXIST)) {
	    return SNMP_ERROR_COMITFAIL;
	}
#endif
	return SnmpDisplayString::commit_set_request(req, ind);
}
//--AgentGen END




/**
 *  agentppCfgStorageFormat
 *
 * "The storage format specifies the format of the persistent
 * configuration storage associated with this row.
 * Currently only AGENT++'s BER encoded MIB object
 * serialization 'agentppBER(1)' is supported."
 */
 
agentppCfgStorageFormat::agentppCfgStorageFormat(const Oidx& id):
    MibLeaf(id, READCREATE, new SnmpInt32(1), VMODE_DEFAULT){
	//--AgentGen BEGIN=agentppCfgStorageFormat::agentppCfgStorageFormat
	//--AgentGen END

}

agentppCfgStorageFormat::~agentppCfgStorageFormat()
{

	//--AgentGen BEGIN=agentppCfgStorageFormat::~agentppCfgStorageFormat
	//--AgentGen END

}

MibEntryPtr agentppCfgStorageFormat::clone()
{
	MibEntryPtr other = new agentppCfgStorageFormat(oid);
	((agentppCfgStorageFormat*)other)->replace_value(value->clone());
	((agentppCfgStorageFormat*)other)->set_reference_to_table(my_table);
	//--AgentGen BEGIN=agentppCfgStorageFormat::clone
	//--AgentGen END
	return other;
}

void agentppCfgStorageFormat::get_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppCfgStorageFormat::get_request
	//--AgentGen END
	MibLeaf::get_request(req, ind);
}

long agentppCfgStorageFormat::get_state()
{
	//--AgentGen BEGIN=agentppCfgStorageFormat::get_state
	//--AgentGen END
	return (long)*((SnmpInt32*)value);
}
	
void agentppCfgStorageFormat::set_state(long l)
{
	//--AgentGen BEGIN=agentppCfgStorageFormat::set_state
	//--AgentGen END
	*((SnmpInt32*)value) = l;
}
	

bool agentppCfgStorageFormat::value_ok(const Vbx& vb)
{
	long v;
	if (vb.get_value(v) != SNMP_CLASS_SUCCESS)
	    return FALSE;
	if (v != 1)
	    return FALSE;
	//--AgentGen BEGIN=agentppCfgStorageFormat::value_ok
	if (!((agentppCfgStorageEntry*)my_table)
	    ->get_mib()->get_config_format((unsigned int)v)) {
	    return FALSE;
	}
	//--AgentGen END
	return TRUE;
}

int agentppCfgStorageFormat::prepare_set_request(Request* req, int& ind)
{
	int status;
	if ((status = MibLeaf::prepare_set_request(req, ind)) !=
	    SNMP_ERROR_SUCCESS) return status;

	//--AgentGen BEGIN=agentppCfgStorageFormat::prepare_set_request
	//--AgentGen END
	return SNMP_ERROR_SUCCESS;
}

//--AgentGen BEGIN=agentppCfgStorageFormat
//--AgentGen END




/**
 *  agentppCfgStorageOperation
 *
 * "The states 'idle(1)' and 'inProgress(2)'  can only be
 * read, whereas the states 'store(3)' and 'restore(4)' can
 * only be written.

 * Setting this object to 'store(3)' will save the agent's
 * configuration to the location identified by
 * agentppCfgStoragePath. Setting this object to
 * 'restore(4)' resets the agent to the configuration
 * read from location agentppCfgStoragePath.
 * While the two operations above are in progress,
 * this object returns 'inProgress(2)' on get requests.
 * Otherwise 'idle(1)' is returned on get requests.

 * While its state is 'inProgress' any set request returns
 * a 'resourceUnavailable(13)' error."
 */
 
agentppCfgStorageOperation::agentppCfgStorageOperation(const Oidx& id):
    MibLeaf(id, READCREATE, new SnmpInt32(1), VMODE_DEFAULT){
	//--AgentGen BEGIN=agentppCfgStorageOperation::agentppCfgStorageOperation
#ifndef _NO_THREADS
	operationTask = 0;
#endif
	//--AgentGen END

}

agentppCfgStorageOperation::~agentppCfgStorageOperation()
{

	//--AgentGen BEGIN=agentppCfgStorageOperation::~agentppCfgStorageOperation
#ifndef _NO_THREADS
	if (operationTask) {
	    delete operationTask;
	}
#endif
	//--AgentGen END

}

MibEntryPtr agentppCfgStorageOperation::clone()
{
	MibEntryPtr other = new agentppCfgStorageOperation(oid);
	((agentppCfgStorageOperation*)other)->replace_value(value->clone());
	((agentppCfgStorageOperation*)other)->set_reference_to_table(my_table);
	//--AgentGen BEGIN=agentppCfgStorageOperation::clone
	//--AgentGen END
	return other;
}

void agentppCfgStorageOperation::get_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppCfgStorageOperation::get_request
	//--AgentGen END
	MibLeaf::get_request(req, ind);
}

long agentppCfgStorageOperation::get_state()
{
	//--AgentGen BEGIN=agentppCfgStorageOperation::get_state
	//--AgentGen END
	return (long)*((SnmpInt32*)value);
}
	
void agentppCfgStorageOperation::set_state(long l)
{
	//--AgentGen BEGIN=agentppCfgStorageOperation::set_state
	//--AgentGen END
	*((SnmpInt32*)value) = l;
}
	

int agentppCfgStorageOperation::set(const Vbx& vb)
{
	//--AgentGen BEGIN=agentppCfgStorageOperation::set
#ifndef _NO_THREADS
	if ((!operationTask) || (!operationTask->is_alive())) {
	    int operation;
	    if (vb.get_value(operation) != SNMP_CLASS_SUCCESS)
		return SNMP_ERROR_WRONG_TYPE;

	    LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
	    LOG("agentppCfgStorageEntry: Starting storage (operation)");
	    LOG(operation);
	    LOG_END;

	    if (operationTask) {
		delete operationTask;
	    }
	    operationTask = new OperationTask(operation, this);
	    operationTask->start();
	    set_state(agentppCfgStorageOperation::e_inProgress);
	    return SNMP_ERROR_SUCCESS;
	}
	else {
	    return SNMP_ERROR_COMITFAIL;
	}
#endif
	//--AgentGen END
	return MibLeaf::set(vb);
}

bool agentppCfgStorageOperation::value_ok(const Vbx& vb)
{
	long v;
	if (vb.get_value(v) != SNMP_CLASS_SUCCESS)
	    return FALSE;
	if ((v != -1) && (v != 1) 
	     && (v != 2) && (v != 3) 
	     && (v != 4)) return FALSE;
	//--AgentGen BEGIN=agentppCfgStorageOperation::value_ok
	//--AgentGen END
	return TRUE;
}

int agentppCfgStorageOperation::prepare_set_request(Request* req, int& ind)
{
	int status;
	if ((status = MibLeaf::prepare_set_request(req, ind)) !=
	    SNMP_ERROR_SUCCESS) return status;

	//--AgentGen BEGIN=agentppCfgStorageOperation::prepare_set_request
	if (get_state() >= 2) {
	    return SNMP_ERROR_INCONSIST_VAL;
	}
	//--AgentGen END
	return SNMP_ERROR_SUCCESS;
}

//--AgentGen BEGIN=agentppCfgStorageOperation
//--AgentGen END




/**
 *  agentppCfgStorageStorageType
 *
 * "The storage type of the entry in the AGENT++ persistent
 * storage table."
 */
 
agentppCfgStorageStorageType::agentppCfgStorageStorageType(const Oidx& id):
    StorageType(id, 3){
	//--AgentGen BEGIN=agentppCfgStorageStorageType::agentppCfgStorageStorageType
	//--AgentGen END

}

agentppCfgStorageStorageType::~agentppCfgStorageStorageType()
{

	//--AgentGen BEGIN=agentppCfgStorageStorageType::~agentppCfgStorageStorageType
	//--AgentGen END

}

MibEntryPtr agentppCfgStorageStorageType::clone()
{
	MibEntryPtr other = new agentppCfgStorageStorageType(oid);
	((agentppCfgStorageStorageType*)other)->replace_value(value->clone());
	((agentppCfgStorageStorageType*)other)->set_reference_to_table(my_table);
	//--AgentGen BEGIN=agentppCfgStorageStorageType::clone
	//--AgentGen END
	return other;
}

void agentppCfgStorageStorageType::get_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppCfgStorageStorageType::get_request
	//--AgentGen END
	StorageType::get_request(req, ind);
}

long agentppCfgStorageStorageType::get_state()
{
	//--AgentGen BEGIN=agentppCfgStorageStorageType::get_state
	//--AgentGen END
	return (long)*((SnmpInt32*)value);
}
	
void agentppCfgStorageStorageType::set_state(long l)
{
	//--AgentGen BEGIN=agentppCfgStorageStorageType::set_state
	//--AgentGen END
	*((SnmpInt32*)value) = l;
}
	

int agentppCfgStorageStorageType::set(const Vbx& vb)
{
	//--AgentGen BEGIN=agentppCfgStorageStorageType::set
	//--AgentGen END
	return StorageType::set(vb);
}


int agentppCfgStorageStorageType::prepare_set_request(Request* req, int& ind)
{
	int status;
	if ((status = StorageType::prepare_set_request(req, ind)) !=
	    SNMP_ERROR_SUCCESS) return status;

	//--AgentGen BEGIN=agentppCfgStorageStorageType::prepare_set_request
	//--AgentGen END
	return SNMP_ERROR_SUCCESS;
}

//--AgentGen BEGIN=agentppCfgStorageStorageType
//--AgentGen END




/**
 *  agentppCfgStorageStatus
 *
 * "Control for creating and deleting entries.  Entries may
 * not be modified while active."
 */
 
agentppCfgStorageStatus::agentppCfgStorageStatus(const Oidx& id):
    snmpRowStatus(id, READCREATE)
{
	//--AgentGen BEGIN=agentppCfgStorageStatus::agentppCfgStorageStatus
	//--AgentGen END

}

agentppCfgStorageStatus::~agentppCfgStorageStatus()
{

	//--AgentGen BEGIN=agentppCfgStorageStatus::~agentppCfgStorageStatus
	//--AgentGen END

}

MibEntryPtr agentppCfgStorageStatus::clone()
{
	MibEntryPtr other = new agentppCfgStorageStatus(oid);
	((agentppCfgStorageStatus*)other)->replace_value(value->clone());
	((agentppCfgStorageStatus*)other)->set_reference_to_table(my_table);
	//--AgentGen BEGIN=agentppCfgStorageStatus::clone
	//--AgentGen END
	return other;
}

long agentppCfgStorageStatus::get_state()
{
	//--AgentGen BEGIN=agentppCfgStorageStatus::get_state
	//--AgentGen END
	return (long)*((SnmpInt32*)value);
}
	
void agentppCfgStorageStatus::set_state(long l)
{
	//--AgentGen BEGIN=agentppCfgStorageStatus::set_state
	//--AgentGen END
	*((SnmpInt32*)value) = l;
}
	

int agentppCfgStorageStatus::set(const Vbx& vb)
{
	//--AgentGen BEGIN=agentppCfgStorageStatus::set
	//--AgentGen END
	return snmpRowStatus::set(vb);
}

int agentppCfgStorageStatus::prepare_set_request(Request* req, int& ind)
{
	int status;
	if ((status = snmpRowStatus::prepare_set_request(req, ind)) !=
	    SNMP_ERROR_SUCCESS) return status;

	//--AgentGen BEGIN=agentppCfgStorageStatus::prepare_set_request
	//--AgentGen END
	return SNMP_ERROR_SUCCESS;
}

//--AgentGen BEGIN=agentppCfgStorageStatus
//--AgentGen END



// Tables


/**
 *  agentppCfgStorageEntry
 *
 * "A row specifying the location and storage format
 * of an AGENT++ agent configuration."
 */
 
agentppCfgStorageEntry* agentppCfgStorageEntry::instance = 0;

const index_info indAgentppCfgStorageEntry[1] = {
	{ sNMP_SYNTAX_OCTETS, FALSE, 0, 255 }
};

agentppCfgStorageEntry::agentppCfgStorageEntry():
   StorageTable(oidAgentppCfgStorageEntry, indAgentppCfgStorageEntry, 1)
{
	// This table object is a singleton. In order to access it use
	// the static pointer agentppCfgStorageEntry::instance.
	instance = this;

	add_col(new agentppCfgStoragePath(colAgentppCfgStoragePath));
	add_col(new agentppCfgStorageFormat(colAgentppCfgStorageFormat));
	add_col(new TimeStamp(colAgentppCfgStorageLastStore, READONLY, 
			      VMODE_DEFAULT));
	add_col(new TimeStamp(colAgentppCfgStorageLastRestore, READONLY, 
			      VMODE_DEFAULT));
	add_col(new agentppCfgStorageOperation(colAgentppCfgStorageOperation));
	add_storage_col(new agentppCfgStorageStorageType(colAgentppCfgStorageStorageType));
	add_col(new agentppCfgStorageStatus(colAgentppCfgStorageStatus));
	//--AgentGen BEGIN=agentppCfgStorageEntry::agentppCfgStorageEntry
	securePaths = TRUE;
	//--AgentGen END
}

agentppCfgStorageEntry::~agentppCfgStorageEntry()
{
	//--AgentGen BEGIN=agentppCfgStorageEntry::~agentppCfgStorageEntry
	
	//--AgentGen END
	// clear singleton reference
	agentppCfgStorageEntry::instance = 0;
}

void agentppCfgStorageEntry::get_request(Request* req, int ind)
{
	//--AgentGen BEGIN=agentppCfgStorageEntry::get_request
	//--AgentGen END
	StorageTable::get_request(req, ind);
}

int agentppCfgStorageEntry::prepare_set_request(Request* req, int& ind)
{
	//--AgentGen BEGIN=agentppCfgStorageEntry::prepare_set_request
	//--AgentGen END
	return StorageTable::prepare_set_request(req, ind);
}

int agentppCfgStorageEntry::is_transition_ok(MibTable* table, 
					     MibTableRow* row, 
					     const Oidx& index,
					     int currentStatus, 
					     int requestedStatus)
{
	// The row 'row' with 'index' from 'table' (which will be 0 for local table)
	// is requested to change status. Accept or deny this state change here.
	//--AgentGen BEGIN=agentppCfgStorageEntry::is_transition_ok
	if (requestedStatus == rowDestroy) {
	    if (((agentppCfgStorageStorageType*)row->
		 get_nth(nAgentppCfgStorageStorageType))->get_state() >= 4) {
		return SNMP_ERROR_INCONSIST_VAL;
	    }
	    if (((agentppCfgStorageOperation*)row->
		 get_nth(nAgentppCfgStorageOperation))->get_state() != 
		agentppCfgStorageOperation::e_idle) {
		return SNMP_ERROR_INCONSIST_VAL;
	    }
	}
	//--AgentGen END
	return SNMP_ERROR_SUCCESS;
}



void agentppCfgStorageEntry::set_row(MibTableRow* r, 
				     const NS_SNMP OctetStr& p1, 
				     long p2, unsigned long p3,
				     unsigned long p4, long p5, long p6,
				     long p7)
{
	r->get_nth(0)->replace_value(new NS_SNMP OctetStr(p1));
	r->get_nth(1)->replace_value(new SnmpInt32(p2));
	r->get_nth(2)->replace_value(new TimeTicks(p3));
	r->get_nth(3)->replace_value(new TimeTicks(p4));
	r->get_nth(4)->replace_value(new SnmpInt32(p5));
	r->get_nth(5)->replace_value(new SnmpInt32(p6));
	r->get_nth(6)->replace_value(new SnmpInt32(p7));
}

//--AgentGen BEGIN=agentppCfgStorageEntry
void agentppCfgStorageEntry::set_mib(Mib* m) 
{
	mib = m;
	// init first row
	NS_SNMP OctetStr primaryRow("primary");
	Oidx primaryRowIndex(Oidx::from_string(primaryRow));
	MibTableRow* primary = find_index(primaryRowIndex);
	if (!primary) {
	    primary = add_row(primaryRowIndex);
	}
	set_row(primary, mib->get_persistent_objects_path(), 
		agentppCfgStorageFormat::e_agentppBER, 0, 0,
		agentppCfgStorageOperation::e_idle,
		agentppCfgStorageStorageType::e_permanent,
		rowActive);
	primary->set_access(READONLY);
	primary->get_nth(nAgentppCfgStorageOperation)->set_access(READWRITE);
}
//--AgentGen END

// Notifications

// Group
agentpp_config_mib::agentpp_config_mib():
   MibGroup("1.3.6.1.4.1.4976.3.3", "agentpp_config_mib")
{
	//--AgentGen BEGIN=agentpp_config_mib::agentpp_config_mib
	add(new agentppCfgLogLevel(ERROR_LOG, oidAgentppCfgLogLevelError));
	add(new agentppCfgLogLevel(WARNING_LOG, oidAgentppCfgLogLevelWarning));
	add(new agentppCfgLogLevel(EVENT_LOG, oidAgentppCfgLogLevelEvent));
	add(new agentppCfgLogLevel(INFO_LOG, oidAgentppCfgLogLevelInfo));
	add(new agentppCfgLogLevel(DEBUG_LOG, oidAgentppCfgLogLevelDebug));
	//--AgentGen END
	add(new agentppCfgSecSrcAddrValidation());
	//--AgentGen BEGIN=agentpp_config_mib::agentpp_config_mib:post
	//--AgentGen END
}


//--AgentGen BEGIN=agentpp_config_mib
#ifndef _NO_THREADS
agentpp_config_mib::agentpp_config_mib(Mib* backReference):
   MibGroup("1.3.6.1.4.1.4976.3.3", "agentpp_config_mib")
{
	add(new agentppCfgLogLevel(ERROR_LOG, oidAgentppCfgLogLevelError));
	add(new agentppCfgLogLevel(WARNING_LOG, oidAgentppCfgLogLevelWarning));
	add(new agentppCfgLogLevel(EVENT_LOG, oidAgentppCfgLogLevelEvent));
	add(new agentppCfgLogLevel(INFO_LOG, oidAgentppCfgLogLevelInfo));
	add(new agentppCfgLogLevel(DEBUG_LOG, oidAgentppCfgLogLevelDebug));
	add(new agentppCfgSecSrcAddrValidation());
	add(new agentppCfgStorageEntry());
	agentppCfgStorageEntry* storageTable = 
	    (agentppCfgStorageEntry*)content.last();
	storageTable->set_mib(backReference);
}
#endif
//--AgentGen END



//--AgentGen BEGIN=_CLASSES
agentppCfgLogLevel::agentppCfgLogLevel(int lc, const Oidx& oid):
    MibLeaf(oid, READWRITE, new SnmpInt32()){
    logClass = lc;
	if (DefaultLog::log())
	    set_state(DefaultLog::log()->get_filter(logClass));
	
}

agentppCfgLogLevel::~agentppCfgLogLevel()
{
}

void agentppCfgLogLevel::get_request(Request* req, int ind)
{
	if (DefaultLog::log())
		set_state(DefaultLog::log()->get_filter(logClass));	
	MibLeaf::get_request(req, ind);
}

long agentppCfgLogLevel::get_state()
{
	return (long)*((SnmpInt32*)value);
}
	
void agentppCfgLogLevel::set_state(long l)
{
	*((SnmpInt32*)value) = l;
}
	

int agentppCfgLogLevel::commit_set_request(Request* req, int ind)
{
	int status = MibLeaf::commit_set_request(req, ind);
	if (DefaultLog::log())
		DefaultLog::log()->set_filter(logClass, get_state() % 255);
	return status;
}

int agentppCfgLogLevel::undo_set_request(Request* req, int& ind)
{
	int undoValue = *((SnmpInt32*)undo);
	if (DefaultLog::log())
		DefaultLog::log()->set_filter(logClass, undoValue);
	return MibLeaf::undo_set_request(req, ind);
}

bool agentppCfgLogLevel::value_ok(const Vbx& vb)
{
	long v;
	if (vb.get_value(v) != SNMP_CLASS_SUCCESS)
	    return FALSE;
	if (!(((v >= 0) && (v <= 15))))
		 return FALSE;
	return TRUE;
}
//--AgentGen END


#ifdef AGENTPP_NAMESPACE
}
#endif


//--AgentGen BEGIN=_END
#endif
//--AgentGen END

#endif
