/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - agent.cpp  
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
#include <signal.h>

#include <agent_pp/agent++.h>
#include <agent_pp/mib.h>
#include <agent_pp/sim_mib.h>
#include <agent_pp/request.h>
#include <agent_pp/snmp_group.h>
#include <agent_pp/system_group.h>
#include <agent_pp/snmp_target_mib.h>
#include <agent_pp/snmp_notification_mib.h>
#include <agent_pp/snmp_community_mib.h>
#include <agent_pp/notification_originator.h>
#include <agent_pp/agentpp_simulation_mib.h>
#include <agent_pp/agentpp_config_mib.h>
#include <agent_pp/v3_mib.h>
#include <agent_pp/vacm.h>

#include <snmp_pp/oid_def.h>
#include <snmp_pp/mp_v3.h>
#include <snmp_pp/log.h>

#include "module_includes.h"
#include "module_init.h"

#ifdef AGENTPP_NAMESPACE 
using namespace Agentpp;
#endif

// globals:

static const char* loggerModuleName = "agenpro.simagent";

u_short port;
Mib* mib;
RequestList* reqList;
bool run = TRUE;

#define VERSION  AGENTPP_VERSION
#define RELEASE  AGENTPP_RELEASE
#define REVISION AGENTPP_PATCHLEVEL

#define START_ARGS(usagefunc) \
int  arg=0,inarg=0,NEXTARG; char *ARGP=0; \
while(++arg<argc) {  \
  NEXTARG = 0; \
  switch (argv[arg][0]) { \
    default: \
      exit(usagefunc()); \
    case '-' : \
      inarg=1; \
      while(inarg!=0) { \
	switch(argv[arg][inarg++]) { \
          case '?': default: exit(usagefunc()); 
//---------------------------------------------------//
#define CASE_ARG(argchar,action) \
	  case argchar: { \
            if ( argv[arg][inarg] ) {ARGP=&argv[arg][inarg];action; NEXTARG=0;inarg=0;} \
            else {ARGP=argv[arg+1];inarg=0;action;} \
          } \
          break;
//---------------------------------------------------//
#define STOP_ARGS() \
        } \
      if (NEXTARG) { arg+=NEXTARG; NEXTARG=0; break;}  } \
      break;\
  }\
} 

static int usage()
{
  printf("Usage  : agent <options>\n");
  printf("\n");
  printf("Options:      -c                      config mode,readonly mib-var. can be set\n");
  printf("Options:      -d <debug level>        (1..5),default is off \n");
  printf("Options:      -a <address>            default is 0.0.0.0/4700\n");
  printf("Options:      -t <trap destination>   default is localhost/162 \n");
  printf("Options:      -v 			print version info\n");
  return 1;
}




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
        mib.add(new simSysGroup()); 
 	mib.add(new snmpGroup());
	mib.add(new agentpp_simulation_mib());
	mib.add(new TestAndIncr(oidSnmpSetSerialNo));
	mib.add(new snmp_target_mib());
	// Not added by default:
	//	mib.add(new snmp_community_mib());
	mib.add(new snmp_notification_mib());
#ifdef _SNMPv3
	UsmUserTable *uut = new UsmUserTable();

	uut->addNewRow("unsecureUser",
		       SNMPv3_usmNoAuthProtocol,
		       SNMPv3_usmNoPrivProtocol, "", "");
	
	uut->addNewRow("MD5",
		       SNMPv3_usmHMACMD5AuthProtocol,
		       SNMPv3_usmNoPrivProtocol,
		       "MD5UserAuthPassword", "");
	
	uut->addNewRow("SHA",
		       SNMPv3_usmHMACSHAAuthProtocol,
		       SNMPv3_usmNoPrivProtocol,
		       "SHAUserAuthPassword", "");
	
	uut->addNewRow("MD5DES",
		       SNMPv3_usmHMACMD5AuthProtocol,
		       SNMPv3_usmDESPrivProtocol,
		       "MD5DESUserAuthPassword",
		       "MD5DESUserPrivPassword");

	uut->addNewRow("SHADES",
		       SNMPv3_usmHMACSHAAuthProtocol,
		       SNMPv3_usmDESPrivProtocol,
		       "SHADESUserAuthPassword",
		       "SHADESUserPrivPassword");
	
	uut->addNewRow("MD5IDEA",
		       SNMPv3_usmHMACMD5AuthProtocol,
		       SNMPv3_usmIDEAPrivProtocol,
		       "MD5IDEAUserAuthPassword",
		       "MD5IDEAUserPrivPassword");
	
	uut->addNewRow("SHAIDEA",
		       SNMPv3_usmHMACSHAAuthProtocol,
		       SNMPv3_usmIDEAPrivProtocol,
		       "SHAIDEAUserAuthPassword",
		       "SHAIDEAUserPrivPassword");

	// add non persistent USM statistics
	mib.add(new UsmStats());
	// add the USM MIB - usm_mib MibGroup is used to
	// make user added entries persistent
	mib.add(new usm_mib(uut));
	// add non persistent SNMPv3 engine object
	mib.add(new V3SnmpEngine());
	mib.add(new agentpp_config_mib(&mib));
#endif
}	



int main (int argc, char* argv[])
{
  port = 4700;
  int debuglevel = 0;
  int configFlag = 1;
  char trapDestination[80]; strcpy(trapDestination,"127.0.0.1/162"); 
  char myAddress[80]; strcpy(myAddress, "0.0.0.0/4700");
  
  //--------------------------------------------------------------------------
  // Check commandline arguments and change variables
  //--------------------------------------------------------------------------
  START_ARGS(usage);	
  CASE_ARG('c',{sscanf(ARGP,"%i",&configFlag);                NEXTARG=1;})
  CASE_ARG('d',{sscanf(ARGP,"%i",&debuglevel);                NEXTARG=1;})
  CASE_ARG('a',{sscanf(ARGP,"%s",myAddress);                    NEXTARG=1;})
  CASE_ARG('t',{sscanf(ARGP,"%s",trapDestination);            NEXTARG=1;})
  CASE_ARG('v',{printf("Version %d.%d.%d was generated:  %s,  %s.\n",
                       VERSION,RELEASE,REVISION,__DATE__,__TIME__); exit(0);})      	 
    STOP_ARGS();

#ifndef _NO_LOGGING
	DefaultLog::log()->set_filter(ERROR_LOG, debuglevel);
	DefaultLog::log()->set_filter(WARNING_LOG, debuglevel);
	DefaultLog::log()->set_filter(EVENT_LOG, debuglevel);
	DefaultLog::log()->set_filter(INFO_LOG, debuglevel);
	DefaultLog::log()->set_filter(DEBUG_LOG, debuglevel);
#endif

        int status;
	Snmp::socket_startup();  // Initialize socket subsystem
	UdpAddress inaddr(myAddress);
	Snmpx snmp(status, inaddr);

	port = inaddr.get_port();
	printf("SNMP listen port: %d\n",port);


	if (status == SNMP_CLASS_SUCCESS) {

		LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
		LOG("main: SNMP listen port");
		LOG(port);
		LOG_END;
	}
	else {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
		LOG("main: SNMP port init failed");
		LOG(status);
		LOG_END;
		exit(1);
	}
	mib = new Mib();
#ifdef _SNMPv3
        unsigned int snmpEngineBoots = 0;
        OctetStr engineId(SnmpEngineID::create_engine_id(port));

        // you may use your own methods to load/store this counter
        status = mib->get_boot_counter(engineId, snmpEngineBoots);
        if ((status != SNMPv3_OK) && (status < SNMPv3_FILEOPEN_ERROR)) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
		LOG("main: Error loading snmpEngineBoots counter (status)");
		LOG(status);
		LOG_END;
		exit(1);
	}

        snmpEngineBoots++;
        status = mib->set_boot_counter(engineId, snmpEngineBoots);
        if (status != SNMPv3_OK) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
		LOG("main: Error saving snmpEngineBoots counter (status)");
		LOG(status);
		LOG_END;
		exit(1);
	}
	int stat;
        v3MP *v3mp = new v3MP(engineId, snmpEngineBoots, stat);
#endif
	reqList = new RequestList();
#ifdef _SNMPv3
	// register v3MP
	reqList->set_v3mp(v3mp);
#endif
	// register requestList for outgoing requests
	mib->set_request_list(reqList);

	init_signals();

	// add supported objects
	init(*mib);
	// add user modules
	module_init(mib);
	// load persitent objects from disk
	mib->init();

	reqList->set_snmp(&snmp);

#ifdef _SNMPv3
	// register VACM
	Vacm* vacm = new Vacm(*mib);
	reqList->set_vacm(vacm);

	// initialize security information
        vacm->addNewContext("");
        vacm->addNewContext("other");

        // Add new entries to the SecurityToGroupTable.
        // Used to determine the group a given SecurityName belongs to. 
        // User "new" of the USM belongs to newGroup

        vacm->addNewGroup(SecurityModel_USM, "new",
                         "newGroup", storageType_volatile);

        vacm->addNewGroup(SecurityModel_USM, "test", 
                         "testGroup", storageType_volatile);
        vacm->addNewGroup(SecurityModel_v2, "public", 
                         "v1v2group", storageType_volatile);
        vacm->addNewGroup(SecurityModel_v1, "public", 
                         "v1v2group", storageType_volatile);
        vacm->addNewGroup(SecurityModel_USM, "initial", 
                         "initial", storageType_volatile);
        vacm->addNewGroup(SecurityModel_USM, "unsecureUser", 
                         "newGroup", storageType_volatile);
        vacm->addNewGroup(SecurityModel_USM, "MD5", 
                         "testNoPrivGroup", storageType_volatile);
        vacm->addNewGroup(SecurityModel_USM, "SHA", 
                         "testNoPrivGroup", storageType_volatile);
        vacm->addNewGroup(SecurityModel_USM, "MD5DES", 
                         "testGroup", storageType_volatile);
        vacm->addNewGroup(SecurityModel_USM, "SHADES", 
                         "testGroup", storageType_volatile);
        vacm->addNewGroup(SecurityModel_USM, "MD5IDEA", 
                         "testGroup", storageType_volatile);
        vacm->addNewGroup(SecurityModel_USM, "SHAIDEA", 
                         "testGroup", storageType_volatile);

        // remove a group with:
        //vacm->deleteGroup(SecurityModel_USM, "neu");

        // Set access rights of groups.
        // The group "newGroup" (when using the USM with a security
	// level >= noAuthNoPriv within context "") would have full access  
        // (read, write, notify) to all objects in view "newView". 
        vacm->addNewAccessEntry("newGroup", 
				"other",        // context
				SecurityModel_USM, 
				SecurityLevel_noAuthNoPriv,
				match_exact,  // context must mach exactly
				// alternatively: match_prefix  
				"newView", // readView
				"newView", // writeView
				"newView", // notifyView
				storageType_nonVolatile);
        vacm->addNewAccessEntry("testGroup", "",
				SecurityModel_USM, SecurityLevel_authPriv, 
				match_prefix,
				"testView", "testView", 
				"testView", storageType_nonVolatile);
        vacm->addNewAccessEntry("testNoPrivGroup", "",
				SecurityModel_USM, SecurityLevel_authNoPriv, 
				match_prefix,
				"testView", "testView", 
				"testView", storageType_nonVolatile);
        vacm->addNewAccessEntry("testGroup", "",
				SecurityModel_USM, SecurityLevel_noAuthNoPriv, 
				match_prefix,
				"testView", "testView", 
				"testView", storageType_nonVolatile);
        vacm->addNewAccessEntry("v1v2group", "", 
				SecurityModel_v2, SecurityLevel_noAuthNoPriv, 
				match_exact,
				"v1ReadView", "v1WriteView", 
				"v1NotifyView", storageType_nonVolatile);
        vacm->addNewAccessEntry("v1v2group", "", 
				SecurityModel_v1, SecurityLevel_noAuthNoPriv, 
				match_exact,
				"v1ReadView", "v1WriteView", 
				"v1NotifyView", storageType_nonVolatile);
        vacm->addNewAccessEntry("initial", "",
				SecurityModel_USM, SecurityLevel_noAuthNoPriv, 
				match_exact,
				"restricted", "", 
				"restricted", storageType_nonVolatile);
        vacm->addNewAccessEntry("initial", "",
				SecurityModel_USM, SecurityLevel_authNoPriv, 
				match_exact,
				"internet", "internet", 
				"internet", storageType_nonVolatile);
        vacm->addNewAccessEntry("initial", "",
				SecurityModel_USM, SecurityLevel_authPriv, 
				match_exact,
				"internet", "internet", 
				"internet", storageType_nonVolatile);

        // remove an AccessEntry with:
        // vacm->deleteAccessEntry("newGroup", 
	//	      		"",        
	//			SecurityModel_USM, 
	//			SecurityLevel_noAuthNoPriv);


        // Defining Views
        // View "v1ReadView" includes all objects starting with "1.3".
        // If the ith bit of the mask is not set (0), then also all objects
	// which have a different subid at position i are included in the 
	// view.
        // For example: Oid "6.5.4.3.2.1", Mask(binary) 110111 
        //              Then all objects with Oid with "6.5.<?>.3.2.1" 
	//              are included in the view, whereas <?> may be any
	//              natural number.

        vacm->addNewView("v1ReadView", 
			 "1.3",       
			 "",             // Mask "" is same as 0xFFFFFFFFFF...
			 view_included,  // alternatively: view_excluded
			 storageType_nonVolatile);

        vacm->addNewView("v1WriteView", 
			 "1.3",       
			 "",             // Mask "" is same as 0xFFFFFFFFFF...
			 view_included,  // alternatively: view_excluded
			 storageType_nonVolatile);

        vacm->addNewView("v1NotifyView", 
			 "1.3",       
			 "",             // Mask "" is same as 0xFFFFFFFFFF...
			 view_included,  // alternatively: view_excluded
			 storageType_nonVolatile);

        vacm->addNewView("newView", "1.3", "", 
			 view_included, storageType_nonVolatile);
        vacm->addNewView("testView", "1.3.6", "",
			 view_included, storageType_nonVolatile);
        vacm->addNewView("internet", "1.3.6.1","",
			 view_included, storageType_nonVolatile);
        vacm->addNewView("restricted", "1.3.6.1.2.1.1","",
			 view_included, storageType_nonVolatile);
        vacm->addNewView("restricted", "1.3.6.1.2.1.11","", 
			 view_included, storageType_nonVolatile);
        vacm->addNewView("restricted", "1.3.6.1.6.3.10.2.1","", 
			 view_included, storageType_nonVolatile);
        vacm->addNewView("restricted", "1.3.6.1.6.3.11.2.1","",
			 view_included, storageType_nonVolatile);
        vacm->addNewView("restricted", "1.3.6.1.6.3.15.1.1","", 
			 view_included, storageType_nonVolatile);

#endif		
	if (configFlag == 2) {
          printf("agent runs in configuration mode\n");
          agentppSimMode::instance->set_state(2);
        }
        printf("ready\n");
	
	UdpAddress dest(trapDestination);

	Vbx* vbs = 0;
	coldStartOid coldOid;
	NotificationOriginator no;
	no.add_v1_trap_destination(dest, "defaultV1Trap", "v1trap", "public");
	mib->notify("", coldOid, vbs, 0);

	Request* req;
	while (run) {
	  
		req = reqList->receive(2);

		if (req) {
		    mib->process_request(req);
		}
		else {
		    mib->cleanup();
		}
	}
	delete reqList;
	delete mib;
#ifdef _SNMPv3
	delete v3mp;
#endif
	Snmp::socket_cleanup();  // Shut down socket subsystem
	return 0;
}
