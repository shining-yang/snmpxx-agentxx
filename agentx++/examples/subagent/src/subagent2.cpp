/*_############################################################################
  _## 
  _##  subagent2.cpp  
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


#include <stdlib.h>
#include <signal.h>

#include <agent_pp/agent++.h>
#include <agent_pp/snmp_group.h>
#include <agent_pp/system_group.h>
#include <agent_pp/snmp_target_mib.h>
#include <agent_pp/snmp_notification_mib.h>
#include <agent_pp/notification_originator.h>
#include <agent_pp/v3_mib.h>
#include <snmp_pp/oid_def.h>
#include <snmp_pp/mp_v3.h>
#include <snmp_pp/log.h>
#include <agentx_pp/agentx_subagent.h>
#include <atm_mib.h>
#include <if_mib.h>
#include <agentpp_test_mib.h>
#include <agentpp_notifytest_mib.h>

#ifdef AGENTPP_NAMESPACE
using namespace Agentpp;
#endif

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

// globals:

u_short port;
SubAgentXMib* mib;
AgentXRequestList* reqList;
ifEntry* ifTable;
bool run = TRUE;

class NoCommit: public MibLeaf
{
public:
	NoCommit(const Oidx& o): MibLeaf(o, READWRITE, new SnmpInt32(0)) { }
	virtual ~NoCommit() { }
  
	virtual int		commit_set_request(Request*, int) 
				  { undo = value->clone();
				    return SNMP_ERROR_COMITFAIL; }  
};

extern "C" {
static void sig(int signo)
{
	if ((signo == SIGINT) ||
	    (signo == SIGSEGV)) {

		printf ("\n");
      
		switch (signo) {
		case SIGSEGV: {
			printf ("Segmentation fault, aborting.\n");
			exit(1);
		}
		case SIGTERM: 
		case SIGINT: {
			run = FALSE;
			printf ("User abort\n");
		}
		}
	}
}
}

void init_signals()
{
	signal (SIGTERM, sig);
	signal (SIGINT, sig);
	signal (SIGSEGV, sig); 
}	


class NotifyThread: public Thread {
public:
    NotifyThread(SubAgentXMib* m) { mib = m; }
    void run() {
	for (int i=0; i<100; i++) {
  	  // Send a cold start trap to the master
	  Vbx vb("1.3.6.1.2.1.2.2.1.1.1");
	  vb.set_value(1);
	  mib->notify("", "1.3.6.1.6.3.1.1.5.3", &vb, 1);
	  sleep(100);
	}	
    }
private:
    SubAgentXMib* mib;
};

void init(SubAgentXMib& mib)
{
/* Test objects with arbitrary OIDs
	mib.add(new MibLeaf("1.3.6.1.2.2.1.2.1.0", 
			    READWRITE, new SnmpInt32(100100)));
	mib.add(new NoCommit("1.3.6.1.2.2.1.2.2.0"));
	mib.add(new MibLeaf("1.3.6.1.2.2.1.5.2.0", 
			    READWRITE, new Counter64(123456789ul)));
*/
//	mib.add("subagent2", new atm_mib());
	mib.add(new agentpp_test_mib("", &mib));
	mib.add(new agentpp_notifytest_mib("", &mib));
}	



int main (int argc, char* argv[])
{
#ifndef _NO_LOGGING
	DefaultLog::log()->set_filter(ERROR_LOG, 8);
	DefaultLog::log()->set_filter(WARNING_LOG, 8);
	DefaultLog::log()->set_filter(EVENT_LOG, 8);
	DefaultLog::log()->set_filter(INFO_LOG, 8);
	DefaultLog::log()->set_filter(DEBUG_LOG, 8);
#endif

	mib = new SubAgentXMib();
	init_signals();

	AgentXSlave* agentx = new AgentXSlave();
	if (argc > 1) {
	    agentx->set_master_address(UdpAddress(argv[1]));
	    agentx->set_connect_mode(AX_USE_TCP_SOCKET);
	}
	else {
#ifdef AX_UNIX_SOCKET
	    agentx->set_unix_port_loc("/var/agentx/");
	    agentx->set_connect_mode(AX_USE_UNIX_SOCKET | AX_USE_TCP_SOCKET);
#else
	    agentx->set_connect_mode(AX_USE_TCP_SOCKET);
#endif
	}

	reqList = new AgentXRequestList(agentx);
	// register requestList for outgoing requests
	mib->set_request_list(reqList);
	mib->set_default_priority(120);

#ifdef _SNMPv3
	/* Do not use NotificationOriginator in a subagent!
	   Instead use SubAgentXMib::notify(..)

	Vbx* vbs = new Vbx();
	coldStartOid coldOid;
	NotificationOriginator no;
	UdpAddress dest("127.0.0.1/162");
	no.add_v1_trap_destination(dest);
	no.generate(vbs, 0, coldOid, "", "");
	*/
#endif		
	if (!mib->init()) {
		return 1;
	}

	init(*mib);

	NotifyThread notify(mib);
	notify.start();

	// add dynamic tables AFTER initializing subagent
	// to avoid registration of the table itself ...
	ifTable = new ifEntry("", mib);
	// ... and add it with add_no_reg 
	mib->add_no_reg(ifTable);
#ifdef linux
	ifTable->do_update();
#endif
	// Send a cold start trap to the master
	Vbx vb("1.3.6.1.2.1.2.2.1.1.1");
	vb.set_value(1);
	mib->notify("", "1.3.6.1.6.3.1.1.5.3", &vb, 1);

	Request* req;
	unsigned int retries = 0;
	do {
		while ((run) && (!mib->get_agentx()->quit())) {
	  
			req = reqList->receive(40000);

			if (req) {
				mib->process_request(req);
			}
			else {
				mib->cleanup();
			}
		}
		mib->save_all();
		retries = 0;
		// Make sure that all pending set requests which may
		// have locked any resources are terminated and resources
		// are freed before connection to master is reestablished. 
		reqList->terminate_set_requests();
		while ((run) && (retries++ < 10) && (!mib->init())) {
#ifdef _WIN32
			Sleep(10);
#else
			sleep(10);
#endif
		}
	}
	while ((run) && (retries < 10));
	// stop all threads
        mib->delete_thread_pool();
	// delete all pending requests (especially SET requests)
	// in order to unlock all objects
	delete reqList;
	// now delete mib
	delete mib;
	delete agentx;
}


