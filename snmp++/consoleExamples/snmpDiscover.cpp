/*_############################################################################
  _## 
  _##  snmpDiscover.cpp  
  _##
  _##  SNMP++ v3.3
  _##  -----------------------------------------------
  _##  Copyright (c) 2001-2013 Jochen Katz, Frank Fock
  _##
  _##  This software is based on SNMP++2.6 from Hewlett Packard:
  _##  
  _##    Copyright (c) 1996
  _##    Hewlett-Packard Company
  _##  
  _##  ATTENTION: USE OF THIS SOFTWARE IS SUBJECT TO THE FOLLOWING TERMS.
  _##  Permission to use, copy, modify, distribute and/or sell this software 
  _##  and/or its documentation is hereby granted without fee. User agrees 
  _##  to display the above copyright notice and this license notice in all 
  _##  copies of the software and any documentation of the software. User 
  _##  agrees to assume all liability for the use of the software; 
  _##  Hewlett-Packard and Jochen Katz make no representations about the 
  _##  suitability of this software for any purpose. It is provided 
  _##  "AS-IS" without warranty of any kind, either express or implied. User 
  _##  hereby grants a royalty-free license to any and all derivatives based
  _##  upon this software code base. 
  _##  
  _##########################################################################*/

char snmpdiscover_cpp_version[]="@(#) SNMP++ $Id: snmpDiscover.cpp 2359 2013-05-09 20:07:01Z fock $";

#include <libsnmp.h>

#include "snmp_pp/snmp_pp.h"

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

static void
usage()
{
    cout << "Usage:\n";
    cout << "snmpDiscover BroadcastIpAddress [options]\n";
    exit(1);
}

static void
help()
{
    cout << "Usage:\n";
    cout << "snmpDiscover BroadcastIpAddress [options]\n";
    cout << "options: -vN , use SNMP version 1, 2 or 3, default is 1\n";
    cout << "         -PPort , remote port to use\n";
    cout << "         -CCommunity_name, specify community default is 'public' \n";
    cout << "         -rN , retries default is N = 1 retry\n";
    cout << "         -tN , timeout in hundredths of seconds; default is N = 100\n";
#ifdef WITH_LOG_PROFILES
    cout << "         -Lprofile , log profile to use, default is '"
#ifdef DEFAULT_LOG_PROFILE
         << DEFAULT_LOG_PROFILE
#else
         << "original"
#endif
         << "'" << endl;
#endif
    cout << "         -h, -? - prints this help" << endl;
    exit(1);
   }

int main(int argc, char **argv)
{
   //---------[ check the arg count ]----------------------------------------
   if ( argc < 2 )
     usage();
   if ( strstr( argv[1],"-h") != 0 )
     help();
   if ( strstr( argv[1],"-?") != 0 )
     usage();

#if !defined(_NO_LOGGING) && !defined(WITH_LOG_PROFILES)
   // Set filter for logging
   DefaultLog::log()->set_filter(ERROR_LOG, 7);
   DefaultLog::log()->set_filter(WARNING_LOG, 7);
   DefaultLog::log()->set_filter(EVENT_LOG, 7);
   DefaultLog::log()->set_filter(INFO_LOG, 7);
   DefaultLog::log()->set_filter(DEBUG_LOG, 7);
#endif

   Snmp::socket_startup();  // Initialize socket subsystem

   //---------[ make a GenAddress and Oid object to retrieve ]---------------
   UdpAddress address( argv[1]);      // make a SNMP++ Generic address
   if ( !address.valid()) {           // check validity of address
	  cout << "Invalid Address or DNS Name, " << argv[1] << "\n";
	  usage();
   }
   Oid oid("1.3.6.1.2.1.1.1.0");      // default is sysDescr

   //---------[ determine options to use ]-----------------------------------
   snmp_version version=version1;                  // default is v1
   int retries=1;                                  // default retries is 1
   int timeout=100;                                // default is 1 second
   u_short port=161;                               // default snmp port is 161
   OctetStr community("public");                   // community name

#ifdef _SNMPv3
   v3MP *v3_MP;
#endif

   char *ptr;

   for(int x=1;x<argc;x++) {                           // parse for version
     if ( strstr( argv[x],"-v2")!= 0) {
       version = version2c;
       continue;
     }
     if ( strstr( argv[x],"-r")!= 0) {                 // parse for retries
       ptr = argv[x]; ptr++; ptr++;
       retries = atoi(ptr);
       if (( retries<0)|| (retries>5)) retries=1; 
       continue;
     }
     if ( strstr( argv[x], "-t")!=0) {                 // parse for timeout
       ptr = argv[x]; ptr++; ptr++;
       timeout = atoi( ptr);
       if (( timeout < 100)||( timeout>500)) timeout=100;
       continue;
     }
     if ( strstr( argv[x],"-C")!=0) {
       ptr = argv[x]; ptr++; ptr++;
       community = ptr;
       continue;
     }
     if ( strstr( argv[x],"-P")!=0) {
       ptr = argv[x]; ptr++; ptr++;
       sscanf(ptr, "%hu", &port);
       continue;
     }

#ifdef WITH_LOG_PROFILES
     if ( strstr( argv[x], "-L" ) != 0 ) {
       ptr = argv[x]; ptr++; ptr++;
       DefaultLog::log()->set_profile(ptr);
     }
#endif

#ifdef _SNMPv3
     if ( strstr( argv[x],"-v3")!= 0) {
       version = version3;
       continue;
     }
#endif
  }

   //----------[ create a SNMP++ session ]-----------------------------------
   int status;
   // bind to any port and use IPv6 if needed
   Snmp snmp(status, 0, (address.get_ip_version() == Address::version_ipv6));

   if ( status != SNMP_CLASS_SUCCESS) {
      cout << "SNMP++ Session Create Fail, " << snmp.error_msg(status) << "\n";
      return 1;
   }

   //---------[ init SnmpV3 ]--------------------------------------------
#ifdef _SNMPv3
   // MUST create a dummy v3MP object if _SNMPv3 is enabled!
   int construct_status;
   v3_MP = new v3MP("dummy", 0, construct_status);
   if (construct_status != SNMPv3_MP_OK)
   {
     cout << "Error initializing v3MP: " << construct_status << endl;
     return 1;
   }
#endif

   //--------[ build up SNMP++ object needed ]-------------------------------
   address.set_port(port);

   //-------[ issue the broadcast, blocked mode ]----------------------------
   cout << "SNMP++ broadcast to " << argv[1] << " SNMPV" 
#ifdef _SNMPv3
        << ((version==version3) ? (version) : (version+1))
#else
        << (version+1)
#endif
        << " Retries=" << retries
        << " Timeout=" << timeout * 10 <<"ms";
#ifdef _SNMPv3
   if (version == version3)
     cout << endl;
   else
#endif
     cout << " Community=" << community.get_printable() << endl << flush;

   UdpAddressCollection addresses;

   // do multiple loops as requested from "retries"
   for (int loops=1; loops<= retries + 1; ++loops)
   {
     status = snmp.broadcast_discovery(addresses,
				       (timeout + 99) / 100,
				       address, version, &community);

     if (status == SNMP_CLASS_SUCCESS)
     {
       cout << "SNMP++ Success sending broadcast " << loops << "." << endl;
     }
     else
     {
       cout << "SNMP++ Broadcast Error, " << snmp.error_msg( status)
	    << " (" << status <<")" << endl;
     }
   }

   // filter out duplicates
   UdpAddressCollection filtered_addrs;
   int dummy_pos;

   for (int n=0; n < addresses.size(); ++n)
     if (filtered_addrs.find(addresses[n], dummy_pos))
       filtered_addrs += addresses[n];

   // print out all addressess
   cout << "Found " << filtered_addrs.size() << " agents." << endl;
   for (int m=0; m < filtered_addrs.size(); ++m)
     cout << "Answer received from: " << filtered_addrs[m].get_printable()
	  << endl;

   Snmp::socket_cleanup();  // Shut down socket subsystem
}
