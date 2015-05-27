/*_############################################################################
  _## 
  _##  agentpp_test_mib.h  
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

#ifndef _agentpp_test_mib_h
#define _agentpp_test_mib_h

#include <agent_pp/mib.h>
#include <agent_pp/mib_complex_entry.h>
#include <agent_pp/snmp_textual_conventions.h>
#include <agent_pp/notification_originator.h>

#include <agentx_pp/agentx_subagent.h>

#ifndef AGENTPP_TEST_MIB_SHARED_TABLES
#define AGENTPP_TEST_MIB_SHARED_TABLES
#endif

// Scalars
#define oidAgentppTestTimeout             "1.3.6.1.4.1.4976.6.3.1.1.0"
#define oidAgentppTestIndexAllocStrategy  "1.3.6.1.4.1.4976.6.3.1.6.0"

// Columns
#define oidAgentppTestSharedTableCreationTime       "1.3.6.1.4.1.4976.6.3.1.3.1.2"
#define colAgentppTestSharedTableCreationTime       "2"
#define oidAgentppTestSharedTableDelay    "1.3.6.1.4.1.4976.6.3.1.3.1.3"
#define colAgentppTestSharedTableDelay    "3"
#define oidAgentppTestSharedTableRowStatus    "1.3.6.1.4.1.4976.6.3.1.3.1.5"
#define colAgentppTestSharedTableRowStatus    "5"
#define oidAgentppTestSharedTableSession  "1.3.6.1.4.1.4976.6.3.1.3.1.4"
#define colAgentppTestSharedTableSession  "4"

#define oidAgentppTestRowCreation         "1.3.6.1.4.1.4976.6.3.1.4.1.2"
#define colAgentppTestRowCreation         "2"


// Tables
#define oidAgentppTestSharedEntry         "1.3.6.1.4.1.4976.6.3.1.3.1"
#define nAgentppTestSharedTableCreationTime      0
#define cAgentppTestSharedTableCreationTime      2
#define nAgentppTestSharedTableDelay     1
#define cAgentppTestSharedTableDelay     3
#define nAgentppTestSharedTableRowStatus   3
#define cAgentppTestSharedTableRowStatus   5
#define nAgentppTestSharedTableSession   2
#define cAgentppTestSharedTableSession   4

#define oidAgentppTestSessionsEntry       "1.3.6.1.4.1.4976.6.3.1.4.1"
#define nAgentppTestRowCreation          0
#define cAgentppTestRowCreation          2


// Notifications

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
 
class agentppTestTimeout: public MibLeaf
{
	
public:
	agentppTestTimeout();
	virtual ~agentppTestTimeout();

	static agentppTestTimeout* instance;
	
	virtual void       	get_request(Request*, int);
	virtual unsigned long   get_state();
	virtual void       	set_state(unsigned long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual int        	commit_set_request(Request*, int);
	virtual void       	cleanup_set_request(Request*, int&);
	virtual int        	undo_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);

//--AgentGen BEGIN=agentppTestTimeout
//--AgentGen END
};

/**
 *  agentppTestIndexAllocStrategy
 *
 * Setting this object will change the index allocation strategy 
 * for all shared tables in the agentpp_test_mib.
 */
class agentppTestIndexAllocStrategy: public MibLeaf
{
	
public:
	agentppTestIndexAllocStrategy();
	virtual ~agentppTestIndexAllocStrategy();

	static agentppTestIndexAllocStrategy* instance;
	
	virtual void       	get_request(Request*, int);
	virtual int             get_state();
	virtual void       	set_state(int);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual int        	commit_set_request(Request*, int);
	virtual void       	cleanup_set_request(Request*, int&);
	virtual int        	undo_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);

//--AgentGen BEGIN=agentppTestIndexAllocStrategy
//--AgentGen END
};


// Columnar Objects



/**
 *  agentppTestSharedTableDelay
 *
 * "The number of 1/100 seconds that a request to this
 * row will be delayed before it is processed."
 */
 
class agentppTestSharedTableDelay: public MibLeaf
{
	
public:
	agentppTestSharedTableDelay(const Oidx&);
	virtual ~agentppTestSharedTableDelay();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual int        	commit_set_request(Request*, int);
	virtual void       	cleanup_set_request(Request*, int&);
	virtual int        	undo_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);

//--AgentGen BEGIN=agentppTestSharedTableDelay
	void			delay_execution();
//--AgentGen END
};




/**
 *  agentppTestSharedTableRowStatus
 *
 * "The row status of the row."
 */
 
class agentppTestSharedTableRowStatus: public snmpRowStatus
{
	
public:
	agentppTestSharedTableRowStatus(const Oidx&);
	virtual ~agentppTestSharedTableRowStatus();

	virtual MibEntryPtr	clone();
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual int        	commit_set_request(Request*, int);
	virtual void       	cleanup_set_request(Request*, int&);
	virtual int        	undo_set_request(Request*, int&);
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
 
class agentppTestRowCreation: public MibLeaf
{
	
public:
	agentppTestRowCreation(const Oidx&);
	virtual ~agentppTestRowCreation();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual unsigned long   get_state();
	virtual void       	set_state(unsigned long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual int        	commit_set_request(Request*, int);
	virtual void       	cleanup_set_request(Request*, int&);
	virtual int        	undo_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);

//--AgentGen BEGIN=agentppTestRowCreation
//--AgentGen END
};


// Tables


/**
 *  agentppTestSharedEntry
 *
 * "A row of a shared table. Each row is allocated and
 * registered in random intervals."
 */
 
class agentppTestSharedEntry: public AgentXSharedTable{
	
public:
	agentppTestSharedEntry(const NS_SNMP OctetStr&, SubAgentXMib*);
	virtual ~agentppTestSharedEntry();

	static agentppTestSharedEntry* instance;
	
	virtual void        	get_request(Request*, int);
	virtual int         	commit_set_request(Request*, int);
	virtual int         	prepare_set_request(Request*, int&);
	virtual void        	cleanup_set_request(Request*, int&);
	virtual int         	undo_set_request(Request*, int&);
	virtual void        	row_added(MibTableRow*, const Oidx&, MibTable*);
	virtual void        	row_delete(MibTableRow*, const Oidx&, MibTable*);
	virtual void        	row_init(MibTableRow*, const Oidx&, MibTable*);
	virtual void        	row_activated(MibTableRow*, const Oidx&, MibTable*);
	virtual void        	row_deactivated(MibTableRow*, const Oidx&, MibTable*);
//--AgentGen BEGIN=agentppTestSharedEntry
	virtual void		index_allocated(const Oidx&, int, int,
                                                Vbx* vbs=0, u_int vbs_length=0);
	OidList<RequestID>	pending_row_ops;
//--AgentGen END
};


/**
 *  agentppTestSessionsEntry
 *
 * "A row of this table is created by each subagent
 * session that implements the AGENTPP-TEST-MIB."
 */
 
class agentppTestSessionsEntry: public AgentXSharedTable{
	
public:
	agentppTestSessionsEntry(const NS_SNMP OctetStr&, SubAgentXMib*);
	virtual ~agentppTestSessionsEntry();

	static agentppTestSessionsEntry* instance;
	
	virtual void        	get_request(Request*, int);
	virtual int         	commit_set_request(Request*, int);
	virtual int         	prepare_set_request(Request*, int&);
	virtual void        	cleanup_set_request(Request*, int&);
	virtual int         	undo_set_request(Request*, int&);
	virtual void        	row_added(MibTableRow*, const Oidx&, MibTable*);
	virtual void        	row_delete(MibTableRow*, const Oidx&, MibTable*);
	virtual void        	row_init(MibTableRow*, const Oidx&, MibTable*);
	virtual void        	row_activated(MibTableRow*, const Oidx&, MibTable*);
	virtual void        	row_deactivated(MibTableRow*, const Oidx&, MibTable*);
//--AgentGen BEGIN=agentppTestSessionsEntry
//--AgentGen END
};

// Notifications
#ifdef _SNMPv3
#endif

// Group

class agentpp_test_mib: public MibGroup
{
  public:
        agentpp_test_mib(const NS_SNMP OctetStr&, SubAgentXMib*);
        virtual ~agentpp_test_mib() { }
//--AgentGen BEGIN=agentpp_test_mib
//--AgentGen END
};


#ifdef AGENTPP_NAMESPACE
}
#endif


//--AgentGen BEGIN=_END
//--AgentGen END

#endif
