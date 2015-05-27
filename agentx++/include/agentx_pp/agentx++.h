/*_############################################################################
  _## 
  _##  agentx++.h.in  
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

#ifndef _agentxpp_h_
#define _agentxpp_h_

#define AGENTXPP_VERSION_STRING "2.0.6"
#define AGENTXPP_VERSION @AGENT_PP_MAJOR_VERSION@
#define AGENTXPP_RELEASE @AGENT_PP_MINOR_VERSION@
#define AGENTXPP_PATCHLEVEL @AGENT_PP_MICRO_VERSION@

#include <sys/stat.h>
#include <agent_pp/agent++.h>
#include <agent_pp/snmp_pp_ext.h>

#ifndef AGENTPP_USE_THREAD_POOL
#error AGENTPP_USE_THREAD_POOL must be defined in agent++.h of AGENT++ >= v3.5c
#endif

#define AGENTX_OK			0
#define AGENTX_OPEN_PDU			1
#define AGENTX_CLOSE_PDU		2
#define AGENTX_REGISTER_PDU		3
#define AGENTX_UNREGISTER_PDU		4
#define AGENTX_GET_PDU			5
#define AGENTX_GETNEXT_PDU		6
#define AGENTX_GETBULK_PDU		7
#define AGENTX_TESTSET_PDU		8
#define AGENTX_COMMITSET_PDU		9
#define AGENTX_UNDOSET_PDU		10
#define AGENTX_CLEANUPSET_PDU		11
#define AGENTX_NOTIFY_PDU		12
#define AGENTX_PING_PDU			13
#define AGENTX_INDEXALLOCATE_PDU	14
#define AGENTX_INDEXDEALLOCATE_PDU	15
#define AGENTX_ADDAGENTCAPS_PDU		16
#define AGENTX_REMOVEAGENTCAPS_PDU     	17
#define AGENTX_RESPONSE_PDU     	18

#define AGENTX_REASON_OTHER		1
#define AGENTX_REASON_PARSE_ERROR      	2
#define AGENTX_REASON_PROTOCOL_ERROR   	3
#define AGENTX_REASON_TIMEOUTS   	4
#define AGENTX_REASON_SHUTDOWN	    	5
#define AGENTX_REASON_BY_MANAGER        6

#define AGENTX_OPEN_FAILED		256
#define AGENTX_NOT_OPEN			257
#define AGENTX_INDEX_WRONG_TYPE		258
#define AGENTX_INDEX_ALREADY_ALLOCATED	259
#define AGENTX_INDEX_NONE_AVAILABLE	260
#define AGENTX_INDEX_NOT_ALLOCATED	261
#define AGENTX_UNSUPPORTED_CONTEXT	262
#define AGENTX_DUPLICATE_REGISTRATION	263
#define AGENTX_UNKNOWN_REGISTRATION	264
#define AGENTX_UNKNOWN_AGENTCAPS	265
#define AGENTX_PARSE_ERROR		266
#define AGENTX_REQUEST_DENIED		267
#define AGENTX_PROCESSING_ERROR		268

/*  General errors  */

#define AGENTX_SUCCESS			0
#define AGENTX_ERROR			-1
#define AGENTX_MISSING_VB		-2
#define AGENTX_PDU_NOT_INITALIZED	-3
#define AGENTX_SHORT_PDU		-4
#define AGENTX_DISCONNECT		-5
#define AGENTX_BADF			-6
#define AGENTX_EOF			-7
#define AGENTX_INC_PAYLOAD		-8
#define AGENTX_INC_HEADER		-9
#define AGENTX_BADVER			-10
#define AGENTX_TIMEOUT			-11


/*  User errors  */
#define AGENTX_NOREG           -40
#define AGENTX_DUPMAP          -41


#define AGENTX_INT_SIZE		4
#define AGENTX_SHORT_SIZE	2

#define AGENTX_INSTANCE_REGISTRATION   	0x01
#define AGENTX_NEW_INDEX	       	0x02
#define AGENTX_ANY_INDEX	       	0x04
#define AGENTX_NON_DEFAULT_CONTEXT     	0x08
#define AGENTX_NETWORK_BYTE_ORDER      	0x10

#define AGENTX_HEADER_LEN 20

#ifndef AGENTX_USOCK_PERM
#define AGENTX_USOCK_PERM S_IRWXU
#endif
#ifndef AGENTX_USOCK_LOC
#define AGENTX_USOCK_LOC "/var/agentx"
#endif
#ifndef AGENTX_SVC_PORT
#define AGENTX_SVC_PORT 705
#endif

#define AGENTX_DEFAULT_TIMEOUT		5
#define AGENTX_MAX_TIMEOUTS             3
#define AGENTX_DEFAULT_PRIORITY		127

#define SNMP_TRAP_OID			"1.3.6.1.6.3.1.1.4.1.0"
#define SNMP_SYSUPTIME_OID		"1.3.6.1.2.1.1.3.0"

#define TRANSPORT_DOMAIN_LOCAL		"1.3.6.1.2.1.100.1.13"
#define TRANSPORT_DOMAIN_TCP_IPv4	"1.3.6.1.2.1.100.1.5"
#define TRANSPORT_DOMAIN_TCP_IPv6	"1.3.6.1.2.1.100.1.6"

#define AX_USE_UNIX_SOCKET	1
#define AX_USE_TCP_SOCKET	2

// Timeout set requests to (1) avoid deadlocks or (2) if master agent
// does not properly implement SET request processing
#define AGENTX_SUBAGENT_TIMEOUT_SET_REQUESTS 1

// Set this to 1 if the AgentX++ master agent should talk to
// NET-SNMP 4.2.1 (this subagent handles GETBULK with more
// than one repetition incorrectly)
#define MAX_AGENTX_REPETITIONS	100

// If defined NET_SNMP_WORKAROUNDS activates workarounds for
// NET-SNMP 5.1 sub-agent bugs
#define NET_SNMP_WORKAROUNDS 1

#ifndef AGENTXPP_DECL
	#if defined (WIN32) && defined (AGENTX_PP_DLL)
		#ifdef AGENTX_PP_EXPORTS
			#define AGENTXPP_DECL __declspec(dllexport)
			#define AGENTXPP_DECL_TEMPL
			#pragma warning (disable : 4018)	// signed/unsigned mismatch when exporting templates
		#else
			#define AGENTXPP_DECL __declspec(dllimport)
			#define AGENTXPP_DECL_TEMPL extern
			#pragma warning (disable : 4231)	// disable warnings on extern before template instantiation
		#endif
	#else
		#define AGENTXPP_DECL
		#define AGENTXPP_DECL_TEMPL
	#endif
#endif

#ifdef __APPLE__
#define AGENTXPP_DECL_TEMPL_LIST_AGENTXPEER
#define AGENTXPP_DECL_TEMPL_LIST_THREAD
#define AGENTXPP_DECL_TEMPL_LIST_AGENTXREGENTRY
#define AGENTXPP_DECL_TEMPL_LIST_AGENTXINDEXDB
#define AGENTXPP_DECL_TEMPL_LIST_AGENTXSEARCHRANGE
#define AGENTXPP_DECL_TEMPL_LIST_AGENTXPDU
#define AGENTXPP_DECL_TEMPL_LISTCURSOR_AGENTXPDU
#define AGENTXPP_DECL_TEMPL_ORDEREDLIST_AGENTXPDU
#define AGENTXPP_DECL_TEMPL_ORDEREDLISTCURSOR_AGENTXPDU
#define AGENTXPP_DECL_TEMPL_LIST_VBX
#define AGENTXPP_DECL_TEMPL_LIST_AGENTXSESSION
#define AGENTXPP_DECL_TEMPL_LIST_AGENTXPENDINGMESSAGE
#endif

#include <agentx_pp/agentx.h>

#endif // _agentxpp_h_
