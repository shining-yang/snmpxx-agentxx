/*_############################################################################
  _## 
  _##  AgentX++
  _##  -------------------------------------
  _##  Copyright (C) 2000-2003 - Frank Fock
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

#ifndef __LIBAGENTX_H_INCLUDED__
#define __LIBAGENTX_H_INCLUDED__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

#ifdef STDCXX_98_HEADERS
# include <cctype>
# include <cerrno>
# include <climits>
# include <csignal>
# include <cstddef>
# include <cstdio>
# include <cstdlib>
# include <cstring>
# include <ctime>
#else
# include <stdio.h>
# ifdef STDC_HEADERS
#  include <stdlib.h>
#  include <stddef.h>
# else
#  ifdef HAVE_STDLIB_H
#   include <stdlib.h>
#  endif
# endif
# ifdef HAVE_STRING_H
#  if !defined STDC_HEADERS && defined HAVE_MEMORY_H
#   include <memory.h>
#  endif
#  include <string.h>
# endif
# ifdef HAVE_STRINGS_H
#  include <strings.h>
# endif
# ifdef HAVE_INTTYPES_H
#  include <inttypes.h>
# endif
# ifdef HAVE_CTYPE_H
#  include <ctype.h>
# endif
# ifdef HAVE_SIGNAL_H
#  include <signal.h>
# endif
# ifdef HAVE_ERRNO_H
#  include <errno.h>
# endif
# ifdef HAVE_TIME_H
#  include <time.h>
#endif
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#elif defined(HAVE_SYS_UNISTD_H)
# include <sys/unistd.h>
#endif
#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_TIMEB_H
#include <sys/timeb.h> // and _ftime
#endif

#if HAVE_WINSOCK2_H
# include <winsock2.h>
# if HAVE_WS2TCPIP_H
#  include <ws2tcpip.h>
# endif
# if HAVE_WSPIAPI_H
#  include <wspiapi.h>
# endif
#elif HAVE_WINSOCK_H
  /* IIRC winsock.h must be included before windows.h */
# include <winsock.h>
#else
# ifdef HAVE_NETDB_H
#  include <netdb.h>
# endif
# ifdef HAVE_SYS_SOCKET_H
#  include <sys/socket.h>
# endif
# ifdef HAVE_ARPA_INET_H
#  include <arpa/inet.h>
# endif
# ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
# endif
#endif

#ifdef HAVE_POLL_H
# include <poll.h>
#endif
#ifdef HAVE_SYS_SELECT_H
# include <sys/select.h>
#endif

#ifdef _WIN32
# ifdef HAVE_IO_H
#  include <io.h>
# endif
# ifdef HAVE_PROCESS_H
#  include <process.h>
# endif
# include <windows.h>
#endif

#ifndef HAVE_GETPID
# ifdef HAVE__GETPID
#  define getpid _getpid
# endif
#endif

#ifndef HAVE_STRUCT_SOCKADDR_STORAGE_SS_FAMILY
# ifdef HAVE_STRUCT_SOCKADDR_STORAGE___SS_FAMILY
#  define ss_family __ss_family
# endif
#endif

/* Minimum of signed integral types.  */
#ifndef INT8_MIN
# define INT8_MIN               (-128)
#endif
#ifndef INT16_MIN
# define INT16_MIN              (-32767-1)
#endif
#ifndef INT32_MIN
# define INT32_MIN              (-2147483647-1)
#endif
#ifndef INT64_MIN
# define INT64_MIN              (-int64_t(9223372036854775807)-1)
#endif
/* Maximum of signed integral types.  */
#ifndef INT_MAX
# define INT8_MAX               (127)
#endif
#ifndef INT16_MAX
# define INT16_MAX              (32767)
#endif
#ifndef INT32_MAX
# define INT32_MAX              (2147483647)
#endif
#ifndef INT64_MAX
# define INT64_MAX              (int64_t(9223372036854775807))
#endif

/* Maximum of unsigned integral types.  */
#ifndef UINT8_MAX
# define UINT8_MAX              (255)
#endif
#ifndef UINT16_MAX
# define UINT16_MAX             (65535)
#endif
#ifndef UINT32_MAX
# define UINT32_MAX             (4294967295U)
#endif
#ifndef UINT64_MAX
# define UINT64_MAX             (uint64_t(18446744073709551615))
#endif

#ifndef NULL
#define NULL	0
#endif

#ifdef STDCXX_98_HEADERS
# include <iostream>
#else
# include <iostream.h>
#endif

#ifdef HAVE_NAMESPACE_STD
using namespace std;
#endif

#include <snmp_pp/config_snmp_pp.h>
#include <snmp_pp/log.h>
#ifdef _THREADS
#ifndef _WIN32THREADS
#include <pthread.h>
#endif
#endif
#include <agent_pp/agentx_def.h>

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

#include <agentx_pp/agentx++.h>

#endif /* ?__LIBAGENTX_H_INCLUDED__ */
