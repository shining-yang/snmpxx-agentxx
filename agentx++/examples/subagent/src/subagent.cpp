/*_############################################################################
  _## 
  _##  subagent.cpp  
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
#include <agent_pp/mib_complex_entry.h>
#include <snmp_pp/oid_def.h>
#include <snmp_pp/mp_v3.h>
#include <snmp_pp/log.h>
#include <agentx_pp/agentx_subagent.h>
#include <if_mib.h>
#include <atm_mib.h>

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
bool run = TRUE;

extern "C" {
static void sig(int signo)
{
	if ((signo == SIGTERM) || (signo == SIGINT) ||
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



void init(SubAgentXMib& mib)
{
	// add a static table to the "subagent" context
	MibStaticTable* st = new MibStaticTable("1.3.6.1.4.1.4976.6.1.1");
	st->add(MibStaticEntry("1.3.6.1.4.1.4976.6.1.1.2.224", 
			       SnmpInt32(1)));
	st->add(MibStaticEntry("1.3.6.1.4.1.4976.6.1.1.3.224", 
			       OctetStr("a text")));
	st->add(MibStaticEntry("1.3.6.1.4.1.4976.6.1.1.2.71", 
			       SnmpInt32(2)));
	st->add(MibStaticEntry("1.3.6.1.4.1.4976.6.1.1.3.71", 
			       OctetStr("another text")));

	mib.add("subagent", st);
	mib.add("subagent", new sysUpTime());
	mib.add("subagent", new atm_mib());
	Oidx testOID("1.3.6.1.4.1.4976.6.1.2.0");
	for (unsigned long i=0; i<100; i++) {
		testOID[testOID.len()-1] = i;
		mib.add(new MibLeaf(testOID, READWRITE, new SnmpInt32(i)));
	}
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
	mib->set_default_priority(100);
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

	init(*mib);
#ifdef linux
	mib->add(new if_mib("", mib));
#endif
       	mib->init();

	// add agent caps *after* connection is up
	mib->add_agent_caps("", "1.3.6.1.4.1.4976.4.1.1", 
			    "AGENT++ agent built-in capabilities");

	// initialize values -> register rows with master
#ifdef linux
	ifEntry::instance->do_update();
#endif
	Request* req;
	while ((run) && (!mib->get_agentx()->quit())) {
	  
		req = reqList->receive(20000);

		if (req) {
		    mib->process_request(req);
		}
		else {
		  // ping the master
		  mib->ping_master();
		  // we add or remove here rows 
		  /* This shows how manipulation of a shared
		     table can be done from outside the table class:

		  if (ifTable->size() < 3) {
			ifTable->any_index();
		  }
		  else {
			List<MibTableRow>* l = ifTable->get_rows();
			Oidx ind(l->last()->get_index());
			l->clear();
			delete l;
			ifTable->remove_row(ind);
		  }
		  */
		}
	}
	delete mib;
	delete agentx;
	return 0;
}


