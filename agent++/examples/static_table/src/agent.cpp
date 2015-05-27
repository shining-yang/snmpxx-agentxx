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
#include <agent_pp/snmp_group.h>
#include <agent_pp/system_group.h>
#include <agent_pp/snmp_target_mib.h>
#include <agent_pp/snmp_notification_mib.h>
#include <agent_pp/notification_originator.h>
#include <agent_pp/mib_complex_entry.h>
#include <agent_pp/v3_mib.h>
#include <agent_pp/vacm.h>

#include <snmp_pp/oid_def.h>
#include <snmp_pp/mp_v3.h>
#include <snmp_pp/log.h>

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

#ifdef AGENTPP_NAMESPACE
using namespace Agentpp;
#endif

// globals:

static const char* loggerModuleName = "agent++.static_table";

u_short port;
Mib* mib;
RequestList* reqList;
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



void init(Mib& mib)
{
        mib.add(new sysGroup("AGENT++v3.4 Static Table Sample Agent - Use 'MD5' as SNMPv3 user and 'MD5UserAuthPassword' as authentication", 
			     "1.3.6.1.4.1.4976", 10)); 
 	mib.add(new snmpGroup());
	mib.add(new snmp_target_mib());
	mib.add(new snmp_notification_mib());

	// An example usage of the MibStaticTable for a read-only table
	// implementation:
	MibStaticTable* st = new MibStaticTable("1.3.6.1.4.1.4976.6.1.1");
	st->add(MibStaticEntry("2.224", 
			       SnmpInt32(1)));
	// an oldstyle entry with fully specified OID (deprecated)
	st->add(MibStaticEntry("1.3.6.1.4.1.4976.6.1.1.3.224", 
			       OctetStr("An oldstyle table text")));
	st->add(MibStaticEntry("2.71", 
			       SnmpInt32(2)));
	st->add(MibStaticEntry("3.71", 
			       OctetStr("A table text")));

	// An example usage of the MibStaticTable for a read-only scalar
	// group:
	MibStaticTable* ssg = new MibStaticTable("1.3.6.1.4.1.4976.6.1.2");
	ssg->add(MibStaticEntry("1.0", 
				SnmpInt32(1)));
	ssg->add(MibStaticEntry("2.0", 
				OctetStr("A scalar text object")));
	ssg->add(MibStaticEntry("3.0", 
				Counter32(123456)));
	ssg->add(MibStaticEntry("4.1.0", 
				OctetStr("A scalar text in a sub group")));
	ssg->add(MibStaticEntry("4.2.0", 
				Gauge32(65535)));
	mib.add(ssg);
	mib.add(st);

#ifdef _SNMPv3
	UsmUserTable *uut = new UsmUserTable();

	uut->addNewRow("unsecureUser",
		       SNMP_AUTHPROTOCOL_NONE,
		       SNMP_PRIVPROTOCOL_NONE, "", "");
	
	uut->addNewRow("MD5",
		       SNMP_AUTHPROTOCOL_HMACMD5,
		       SNMP_PRIVPROTOCOL_NONE,
		       "MD5UserAuthPassword", "");
	
	uut->addNewRow("SHA",
		       SNMP_AUTHPROTOCOL_HMACSHA,
		       SNMP_PRIVPROTOCOL_NONE,
		       "SHAUserAuthPassword", "");
	
	uut->addNewRow("MD5DES",
		       SNMP_AUTHPROTOCOL_HMACMD5,
		       SNMP_PRIVPROTOCOL_DES,
		       "MD5DESUserAuthPassword",
		       "MD5DESUserPrivPassword");

	uut->addNewRow("SHADES",
		       SNMP_AUTHPROTOCOL_HMACSHA,
		       SNMP_PRIVPROTOCOL_DES,
		       "SHADESUserAuthPassword",
		       "SHADESUserPrivPassword");
	
	uut->addNewRow("MD53DES",
		       SNMP_AUTHPROTOCOL_HMACMD5,
		       SNMP_PRIVPROTOCOL_3DESEDE,
		       "MD53DESUserAuthPassword",
		       "MD53DESUserPrivPassword");

	uut->addNewRow("SHA3DES",
		       SNMP_AUTHPROTOCOL_HMACSHA,
		       SNMP_PRIVPROTOCOL_3DESEDE,
		       "SHA3DESUserAuthPassword",
		       "SHA3DESUserPrivPassword");
	
	uut->addNewRow("MD5IDEA",
		       SNMP_AUTHPROTOCOL_HMACMD5,
		       SNMP_PRIVPROTOCOL_IDEA,
		       "MD5IDEAUserAuthPassword",
		       "MD5IDEAUserPrivPassword");
	
	uut->addNewRow("SHAIDEA",
		       SNMP_AUTHPROTOCOL_HMACSHA,
		       SNMP_PRIVPROTOCOL_IDEA,
		       "SHAIDEAUserAuthPassword",
		       "SHAIDEAUserPrivPassword");

	uut->addNewRow("MD5AES128",
		       SNMP_AUTHPROTOCOL_HMACMD5,
		       SNMP_PRIVPROTOCOL_AES128,
		       "MD5AES128UserAuthPassword",
		       "MD5AES128UserPrivPassword");
	
	uut->addNewRow("SHAAES128",
		       SNMP_AUTHPROTOCOL_HMACSHA,
		       SNMP_PRIVPROTOCOL_AES128,
		       "SHAAES128UserAuthPassword",
		       "SHAAES128UserPrivPassword");

	uut->addNewRow("MD5AES192",
		       SNMP_AUTHPROTOCOL_HMACMD5,
		       SNMP_PRIVPROTOCOL_AES192,
		       "MD5AES192UserAuthPassword",
		       "MD5AES192UserPrivPassword");
	
	uut->addNewRow("SHAAES192",
		       SNMP_AUTHPROTOCOL_HMACSHA,
		       SNMP_PRIVPROTOCOL_AES192,
		       "SHAAES192UserAuthPassword",
		       "SHAAES192UserPrivPassword");

	uut->addNewRow("MD5AES256",
		       SNMP_AUTHPROTOCOL_HMACMD5,
		       SNMP_PRIVPROTOCOL_AES256,
		       "MD5AES256UserAuthPassword",
		       "MD5AES256UserPrivPassword");

	uut->addNewRow("SHAAES256",
		       SNMP_AUTHPROTOCOL_HMACSHA,
		       SNMP_PRIVPROTOCOL_AES256,
		       "SHAAES256UserAuthPassword",
		       "SHAAES256UserPrivPassword");

	// add non persistent USM statistics
	mib.add(new UsmStats());
	// add the USM MIB - usm_mib MibGroup is used to
	// make user added entries persistent
	mib.add(new usm_mib(uut));
	// add non persistent SNMPv3 engine object
	mib.add(new V3SnmpEngine());
#endif
}	



int main (int argc, char* argv[])
{
	if (argc>1)
		port = atoi(argv[1]);
	else
		port = 4700;

#ifndef _NO_LOGGING
	DefaultLog::log()->set_filter(ERROR_LOG, 5);
	DefaultLog::log()->set_filter(WARNING_LOG, 5);
	DefaultLog::log()->set_filter(EVENT_LOG, 5);
	DefaultLog::log()->set_filter(INFO_LOG, 5);
	DefaultLog::log()->set_filter(DEBUG_LOG, 6);
#endif
	int status;
	Snmp::socket_startup();  // Initialize socket subsystem
	Snmpx snmp(status, port);

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

        vacm->addNewGroup(SNMP_SECURITY_MODEL_USM, "new",
                         "newGroup", storageType_volatile);

        vacm->addNewGroup(SNMP_SECURITY_MODEL_USM, "test", 
                         "testGroup", storageType_volatile);
        vacm->addNewGroup(SNMP_SECURITY_MODEL_V2, "public", 
                         "v1v2group", storageType_volatile);
        vacm->addNewGroup(SNMP_SECURITY_MODEL_V1, "public", 
                         "v1v2group", storageType_volatile);
        vacm->addNewGroup(SNMP_SECURITY_MODEL_USM, "initial", 
                         "initial", storageType_volatile);
        vacm->addNewGroup(SNMP_SECURITY_MODEL_USM, "unsecureUser", 
                         "newGroup", storageType_volatile);
        vacm->addNewGroup(SNMP_SECURITY_MODEL_USM, "MD5", 
                         "testNoPrivGroup", storageType_volatile);
        vacm->addNewGroup(SNMP_SECURITY_MODEL_USM, "SHA", 
                         "testNoPrivGroup", storageType_volatile);
        vacm->addNewGroup(SNMP_SECURITY_MODEL_USM, "MD5DES", 
                         "testGroup", storageType_volatile);
        vacm->addNewGroup(SNMP_SECURITY_MODEL_USM, "SHADES", 
                         "testGroup", storageType_volatile);
        vacm->addNewGroup(SNMP_SECURITY_MODEL_USM, "MD5IDEA", 
                         "testGroup", storageType_volatile);
        vacm->addNewGroup(SNMP_SECURITY_MODEL_USM, "SHAIDEA", 
                         "testGroup", storageType_volatile);
        vacm->addNewGroup(SNMP_SECURITY_MODEL_USM, "MD5AES128",
                         "testGroup", storageType_volatile);
        vacm->addNewGroup(SNMP_SECURITY_MODEL_USM, "SHAAES128", 
                         "testGroup", storageType_volatile);
        vacm->addNewGroup(SNMP_SECURITY_MODEL_USM, "MD5AES192",
                         "testGroup", storageType_volatile);
        vacm->addNewGroup(SNMP_SECURITY_MODEL_USM, "SHAAES192", 
                         "testGroup", storageType_volatile);
        vacm->addNewGroup(SNMP_SECURITY_MODEL_USM, "MD5AES256",
                         "testGroup", storageType_volatile);
        vacm->addNewGroup(SNMP_SECURITY_MODEL_USM, "SHAAES256", 
                         "testGroup", storageType_volatile);

        // remove a group with:
        //vacm->deleteGroup(SNMP_SECURITY_MODEL_USM, "neu");

        // Set access rights of groups.
        // The group "newGroup" (when using the USM with a security
	// level >= noAuthNoPriv within context "") would have full access  
        // (read, write, notify) to all objects in view "newView". 
        vacm->addNewAccessEntry("newGroup", 
				"other",        // context
				SNMP_SECURITY_MODEL_USM,
				SNMP_SECURITY_LEVEL_NOAUTH_NOPRIV,
				match_exact,  // context must mach exactly
				// alternatively: match_prefix  
				"newView", // readView
				"newView", // writeView
				"newView", // notifyView
				storageType_nonVolatile);
        vacm->addNewAccessEntry("testGroup", "",
				SNMP_SECURITY_MODEL_USM,
				SNMP_SECURITY_LEVEL_AUTH_PRIV, 
				match_prefix,
				"testView", "testView", 
				"testView", storageType_nonVolatile);
        vacm->addNewAccessEntry("testNoPrivGroup", "",
				SNMP_SECURITY_MODEL_USM,
				SNMP_SECURITY_LEVEL_AUTH_NOPRIV, 
				match_prefix,
				"testView", "testView", 
				"testView", storageType_nonVolatile);
        vacm->addNewAccessEntry("testGroup", "",
				SNMP_SECURITY_MODEL_USM,
				SNMP_SECURITY_LEVEL_NOAUTH_NOPRIV, 
				match_prefix,
				"testView", "testView", 
				"testView", storageType_nonVolatile);
        vacm->addNewAccessEntry("v1v2group", "", 
				SNMP_SECURITY_MODEL_V2,
				SNMP_SECURITY_LEVEL_NOAUTH_NOPRIV, 
				match_exact,
				"v1ReadView", "v1WriteView", 
				"v1NotifyView", storageType_nonVolatile);
        vacm->addNewAccessEntry("v1v2group", "", 
				SNMP_SECURITY_MODEL_V1,
				SNMP_SECURITY_LEVEL_NOAUTH_NOPRIV, 
				match_exact,
				"v1ReadView", "v1WriteView", 
				"v1NotifyView", storageType_nonVolatile);
        vacm->addNewAccessEntry("initial", "",
				SNMP_SECURITY_MODEL_USM,
				SNMP_SECURITY_LEVEL_NOAUTH_NOPRIV, 
				match_exact,
				"restricted", "", 
				"restricted", storageType_nonVolatile);
        vacm->addNewAccessEntry("initial", "",
				SNMP_SECURITY_MODEL_USM,
				SNMP_SECURITY_LEVEL_AUTH_NOPRIV, 
				match_exact,
				"internet", "internet", 
				"internet", storageType_nonVolatile);
        vacm->addNewAccessEntry("initial", "",
				SNMP_SECURITY_MODEL_USM,
				SNMP_SECURITY_LEVEL_AUTH_PRIV, 
				match_exact,
				"internet", "internet", 
				"internet", storageType_nonVolatile);

        // remove an AccessEntry with:
        // vacm->deleteAccessEntry("newGroup", 
	//	      		"",        
	//			SNMP_SECURITY_MODEL_USM, 
	//			SNMP_SECURITY_LEVEL_NOAUTH_NOPRIV);


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
	Vbx* vbs = 0;
	coldStartOid coldOid;
	NotificationOriginator no;
	UdpAddress dest("127.0.0.1/162");
	no.add_v1_trap_destination(dest, "defaultV1Trap", "v1trap", "public");
	no.generate(vbs, 0, coldOid, "", "");

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
	delete mib;
	Snmp::socket_cleanup();  // Shut down socket subsystem
	return 0;
}
