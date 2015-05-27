/*_############################################################################
  _## 
  _##  agent.cpp  
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
#include <agent_pp/snmp_community_mib.h>
#include <agent_pp/notification_originator.h>
#include <agent_pp/v3_mib.h>
#include <snmp_pp/oid_def.h>
#include <snmp_pp/mp_v3.h>
#include <snmp_pp/log.h>
#include <agentx_pp/agentx_subagent.h>

#include "module_includes.h"
#include "module_init.h"

#ifdef AGENTPP_NAMESPACE 
using namespace Agentpp;
#endif

static const char *loggerModuleName = "subagent";

// globals:

u_short port;
SubAgentXMib* mib;
AgentXRequestList* reqList;
bool run = TRUE;


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
			if (run) {
				printf ("User abort\n");
				run = FALSE;
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



void init(Mib& mib)
{
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
#ifdef AX_UNIX_SOCKET
	agentx->set_unix_port_loc("/var/agentx/");
	agentx->set_connect_mode(AX_USE_UNIX_SOCKET | AX_USE_TCP_SOCKET);
#else
	agentx->set_connect_mode(AX_USE_TCP_SOCKET);
#endif
	reqList = new AgentXRequestList(agentx);
	// register requestList for outgoing requests
	mib->set_request_list(reqList);

	init(*mib);
	module_init("", mib);

       	mib->init();

	Request* req;
	while ((run) && (!mib->get_agentx()->quit())) {
	  
		req = reqList->receive(20000);

		if (req) {
		    mib->process_request(req);
		}
		else {
		  // ping the master
		  mib->ping_master();
		}
	}
	delete mib;
	delete agentx;
	return 0;
}


