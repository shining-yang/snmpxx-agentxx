/*_############################################################################
  _## 
  _##  master.cpp  
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
#include <agent_pp/notification_log_mib.h>
#include <agent_pp/snmp_community_mib.h>
#include <agent_pp/agentpp_config_mib.h>
#include <agent_pp/v3_mib.h>
#include <snmp_pp/oid_def.h>
#include <snmp_pp/mp_v3.h>
#include <snmp_pp/log.h>
#include <agentx_pp/agentx_master.h>
#include <agentx_pp/agentpp_agentx_mib.h>

#ifdef AGENTPP_NAMESPACE
using namespace Agentpp;
#endif

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

static const char *loggerModuleName = "agentx++.master";

// globals:

u_short port;
MasterAgentXMib* mib = 0;
RequestList* reqList = 0;
AgentXMaster* agentx = 0;
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
			if (run) printf ("User abort\n");
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
	// catch SIGPIPE and ignore it
	// this can occur when the subagent dies when we are trying
	// to send a request
#ifndef WIN32
	signal (SIGPIPE, sig);
#endif
}	


/**
 * The RegDump class can be used for debugging purposes to dump the
 * registration of the AgentX master agent. It is not used by default. 
 */
class RegDump: public MibLeaf
{
public:
  RegDump(Mib* m): MibLeaf("1.3.6.1.4.1.4976.100.0", READWRITE, new OctetStr(""))
  { 
    myMib = m;
  }
  
  virtual int set(const Vbx& vb)
  {
	OctetStr context;
	vb.get_value(context);
	MibContext* c = myMib->get_context(context);
	if (!c) return SNMP_ERROR_COMITFAIL;
	OidListCursor<MibEntry> cr(c->get_content());
	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
	LOG("RegDump: BEGIN");
	LOG_END;
	for (; cr.get(); cr.next()) {
		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
		LOG(cr.get()->key()->get_printable());
		LOG(cr.get()->max_key()->get_printable());
		LOG_END;
	}
	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
	LOG("RegDump: END");
	LOG_END;	
	return MibLeaf::set(vb);
  }

protected:
  Mib* myMib;

};

#ifdef _SNMPv3
void allow_agentx_tcp_from_localhost()
{
	if (snmpTargetAddrEntry::instance && 
		snmpTargetAddrExtEntry::instance) { 
		snmpTargetAddrEntry::instance->
		    add_entry("AgentXLocalhost", "1.3.6.1.2.1.100.1.5",
			      OctetStr::from_hex_string("7F 00 00 01 00 00"),
					  "agentX", "null");
		MibTableRow* r =
		    snmpTargetAddrExtEntry::instance->
		    add_row(Oidx::from_string("AgentXLocalhost", FALSE));
		snmpTargetAddrExtEntry::instance->
		    set_row(r, OctetStr::from_hex_string("FF FF FF FF 00 00"),
			    1500);
		r = agentppAgentXExtSrcAddrEntry::instance->
		    add_row(Oidx::from_string("localhost"));
		r->get_nth(0)->replace_value(new OctetStr("agentX"));
		r->get_nth(1)->replace_value(new SnmpInt32(rowActive));
	}
}

void allow_agentx_tcp_from_ip(const IpAddress& ipaddr)
{
	if (snmpTargetAddrEntry::instance && 
		snmpTargetAddrExtEntry::instance) {
                OctetStr ipStr;
                for (int i=0; i<4; i++) {
                        ipStr += ipaddr[i];
                }
                ipStr += (unsigned char)0;
                ipStr += (unsigned char)0;
		snmpTargetAddrEntry::instance->
		    add_entry("AgentX-IP", "1.3.6.1.2.1.100.1.5", ipStr,
					  "agentX", "null");
		MibTableRow* r =
		    snmpTargetAddrExtEntry::instance->
		    add_row(Oidx::from_string("AgentX-IP", FALSE));
		snmpTargetAddrExtEntry::instance->
		    set_row(r, OctetStr::from_hex_string("FF FF FF FF 00 00"),
			    1500);
		r = agentppAgentXExtSrcAddrEntry::instance->
		    add_row(Oidx::from_string("subagent ip"));
		r->get_nth(0)->replace_value(new OctetStr("agentX"));
		r->get_nth(1)->replace_value(new SnmpInt32(rowActive));
	}
}

#endif

void init(Mib& mib)
{
	OctetStr sysDescr("AgentX++ v");
	sysDescr += AGENTXPP_VERSION_STRING;
	sysDescr += " example agent";
        mib.add(new sysGroup(sysDescr.get_printable(), 
			     "1.3.6.1.4.1.4976", 10)); 
 	mib.add(new snmpGroup());
	mib.add(new TestAndIncr(oidSnmpSetSerialNo));
	mib.add(new snmp_target_mib());
	
	// Add the following if you want to be able to
	// dump the agent's current AgentX registration to stdout
	//	mib.add(new RegDump(&mib));

#ifdef _SNMPv3
	mib.add(new snmp_community_mib());
	mib.add(new agentpp_agentx_mib());
#endif
	mib.add(new snmp_notification_mib());
#ifdef _SNMPv3
	mib.add(new notification_log_mib());

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
	mib.add(new MPDGroup());
	mib.add(new agentpp_config_mib(&mib));
#endif
	/* No longer needed if auto context creation is activated! See below.
	// add an empty context, so a subagent can register
	// its objects within this context (and of course the default 
	// context.
	mib.add_context("subagent");
	mib.add_context("subagent2");
	*/
}	



int main (int argc, char* argv[])
{
        IpAddress subagentIP("127.0.0.1");
	int agentxPort = AGENTX_SVC_PORT;
	if (argc>1) {
	    port = atoi(argv[1]);
	    if (argc > 2) {
		agentxPort = atoi(argv[2]);
                if (argc > 3) {
                    subagentIP = argv[3];
                }
	    }
	}
	else {
	    port = 4700;
	}


#ifndef _NO_LOGGING
	DefaultLog::log()->set_filter(ERROR_LOG, 8);
	DefaultLog::log()->set_filter(WARNING_LOG, 8);
	DefaultLog::log()->set_filter(EVENT_LOG, 8);
	DefaultLog::log()->set_filter(INFO_LOG, 8);
	DefaultLog::log()->set_filter(DEBUG_LOG, 9);
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
#ifdef _SNMPv3
        unsigned int snmpEngineBoots = 0;
        OctetStr engineId(SnmpEngineID::create_engine_id(port));

        // you may use your own methods to load/store this counter
	mib = new MasterAgentXMib();
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
#else
	mib = new MasterAgentXMib();	
#endif

	init_signals();

	reqList = new RequestList();
#ifdef _SNMPv3
	// register v3MP
	reqList->set_v3mp(v3mp);
#endif
	// register requestList for outgoing requests
	mib->set_request_list(reqList);
	agentx = new AgentXMaster();

	agentx->set_tcp_port(agentxPort);
#ifdef AX_UNIX_SOCKET
#ifndef WIN32
	agentx->set_connect_mode(AX_USE_UNIX_SOCKET | AX_USE_TCP_SOCKET);
#else
	agentx->set_connect_mode(AX_USE_TCP_SOCKET);
#endif
	agentx->set_unix_port_loc("/var/agentx/");
#else
	agentx->set_connect_mode(AX_USE_TCP_SOCKET);
#endif
	mib->set_agentx(agentx);
	// enable auto context creation -> a subagent may register for a
	// context not yet known by the master
	mib->set_auto_context(TRUE);

	init(*mib);
	reqList->set_snmp(&snmp);

#ifdef _SNMPv3
	// add entrires in various tables to allow AgentX subagents 
        // to connect to this master using its TCP port from all IPv4 ports
        // from the localhost (127.0.0.1)
	allow_agentx_tcp_from_ip(subagentIP);

	// register VACM
	Vacm* vacm = new Vacm(*mib);
	reqList->set_vacm(vacm);

	// initialize security information
        vacm->addNewContext("");
        vacm->addNewContext("subagent");

        // Add new entries to the SecurityToGroupTable.
        // Used to determine the group a given SecurityName belongs to. 
        // User "new" of the USM belongs to newGroup

        vacm->addNewGroup(SecurityModel_USM, "unsecureUser",
                         "newGroup", storageType_volatile);

        vacm->addNewGroup(SecurityModel_USM, "test", 
                         "testGroup", storageType_volatile);
        vacm->addNewGroup(SecurityModel_v2, "public", 
                         "v1v2group", storageType_volatile);
        vacm->addNewGroup(SecurityModel_v1, "public", 
                         "v1v2group", storageType_volatile);
        vacm->addNewGroup(SecurityModel_USM, "initial", 
                         "initial", storageType_volatile);
        vacm->addNewGroup(SecurityModel_USM, "MD5", 
                         "newGroup", storageType_volatile);
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
				"subagent",        // context
				SecurityModel_USM, 
				SecurityLevel_noAuthNoPriv,
				match_exact,  // context must mach exactly
				// alternatively: match_prefix  
				"newView", // readView
				"newView", // writeView
				"newView", // notifyView
				storageType_nonVolatile);
        vacm->addNewAccessEntry("newGroup", 
				"subagent2",        // context
				SecurityModel_USM, 
				SecurityLevel_noAuthNoPriv,
				match_exact,  // context must mach exactly
				// alternatively: match_prefix  
				"newView", // readView
				"newView", // writeView
				"newView", // notifyView
				storageType_nonVolatile);
        vacm->addNewAccessEntry("newGroup", 
				"",        // context
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
	// add SNMPv1/v2c community to v3 security name mapping
	OctetStr co("public");
	MibTableRow* row = snmpCommunityEntry::instance->
	    add_row(Oidx::from_string(co, FALSE));
	OctetStr tag("v1v2cPermittedManagers");
	snmpCommunityEntry::instance->
	    set_row(row, co, co,
		    reqList->get_v3mp()->get_local_engine_id(),
		    "", tag, 3, 1);

#endif		
	// start AgentX master
	mib->init();
	Vbx* vbs = 0;
	coldStartOid coldOid;
	NotificationOriginator no;
	UdpAddress dest("127.0.0.1/162");
	no.add_v1_trap_destination(dest, "defaultV1Trap", "v1trap", "public");
	mib->notify("", coldOid, vbs, 0);

	Request* req;
	while (run) {
	  
		req = reqList->receive(2);

		if (req) {
		    mib->process_request(req);
		}
		else if ((reqList->size() == 0) &&
			 (mib->get_thread_pool()->is_idle())) {
		    mib->cleanup();
		}
	}
	mib->delete_thread_pool();
	delete mib;
	delete reqList;
	delete agentx;
#ifdef _SNMPv3
	delete v3mp;
#endif
	Snmp::socket_cleanup();  // Shut down socket subsystem
	return 0;
}


