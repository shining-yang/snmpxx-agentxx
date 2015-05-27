  ############################################################################
  ## 
  ##  configure.ac  
  ## 
  ##
  ##  AgentX++ 2
  ##  -------------------------------------
  ##  Copyright (C) 2000-2014 - Frank Fock
  ##  
  ##  Use of this software is subject to the license agreement you received
  ##  with this software and which can be downloaded from 
  ##  http:/www.agentpp.com
  ##
  ##  This is licensed software and may not be used in a commercial
  ##  environment, except for evaluation purposes, unless a valid
  ##  license has been purchased.
  ##  
  ##########################################################################*
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# don't require higher autoconf version for win32 support
AC_PREREQ([2.63])
AC_INIT([agentX++], [2.0.6], [support@agentpp.com], [agentx++])
AGENTX_PP_MAJOR_VERSION=2
AGENTX_PP_MINOR_VERSION=0
AGENTX_PP_MICRO_VERSION=6
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_SRCDIR([src/agentx.cpp])
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE([disable])

dnl
dnl Get cannonical host
dnl
AC_CANONICAL_HOST
AC_DEFINE_UNQUOTED([OS_TYPE], "$host_os",
                   [Define to be the name of the operating system.])

dnl disable unnecessary compiler checks
AC_DEFUN([AC_PROG_F77], [:])
AC_DEFUN([AC_PROG_FC], [:])
AC_DEFUN([AC_PROG_OBJC], [:])
AC_DEFUN([AC_PROG_OBJCCPP], [:])
AC_DEFUN([AC_LIBTOOL_F77], [:])

dnl disable check for GNU compatible malloc - just a working malloc is enough
AC_DEFUN([AC_FUNC_MALLOC_IF], [:])
AC_DEFUN([AC_FUNC_MALLOC], [
  AC_CHECK_FUNCS([malloc realloc])
])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_GREP
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_SED
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
PKG_PROG_PKG_CONFIG

# Check for extra flags to enable some kind of behavior
AC_CHECK_DECL([_AIX],[ac_cv_need_minus_d_linux_source_compat="yes"; ac_cv_need_minus_d_all_source="yes"])
AC_CHECK_DECL([__linux],[ac_cv_need_minus_d_gnu_source="yes"]) dnl probably required by kfreebsd, too?
AC_CHECK_DECL([__sun],[ac_cv_need_minus_d_extensions="yes"])

AS_IF([test "x$ac_cv_need_minus_d_linux_source_compat" = "xyes"],
      [CPPFLAGS="$CPPFLAGS -D_LINUX_SOURCE_COMPAT"])

AS_IF([test "x$ac_cv_need_minus_d_gnu_source" = "xyes"],
      [CPPFLAGS="$CPPFLAGS -D_GNU_SOURCE"])

AS_IF([test "x$ac_cv_need_minus_d_all_source" = "xyes"],
      [CPPFLAGS="$CPPFLAGS -D_ALL_SOURCE"])

AS_IF([test "x$ac_cv_need_minus_d_extensions" = "xyes"],
      [CPPFLAGS="$CPPFLAGS -D__EXTENSIONS__"])

ACX_CHECK_XOPEN_SOURCE
AS_IF([test -n "$XOPEN_SOURCE_CPPFLAGS"], [CPPFLAGS="$CPPFLAGS $XOPEN_SOURCE_CPPFLAGS"])

dnl
dnl Process features.
dnl
ACX_CHECK_WITH_LIB([snmp++],[yes])
ACX_CHECK_WITH_LIB([agent++],[yes])

AC_ARG_ENABLE(debug,
[  --disable-debug         disable support for debugging output],
ENABLE_DEBUG=$enableval, ENABLE_DEBUG=yes)
AC_ARG_ENABLE(namespace,
[  --enable-namespace      enable using of namespace (default: on if supported)
  --disable-namespace     disable using of namespace],
ENABLE_NAMESPACE=$enableval, ENABLE_NAMESPACE=check)
AC_ARG_ENABLE(master,
[  --disable-master         disable support for master agent],
ENABLE_MASTER=$enableval, ENABLE_MASTER=yes)
AC_ARG_ENABLE(subagent,
[  --disable-subagent         disable support for subagent agent],
ENABLE_SUBAGENT=$enableval, ENABLE_SUBAGENT=yes)
AC_ARG_ENABLE(proxy,
[  --enable-proxy          enable building of proxy (default: proxy-forwarder)
  --disable-proxy         disable building of proxy],
ENABLE_PROXY=$enableval, ENABLE_PROXY=check)
AC_ARG_ENABLE(forwarder,
[  --enable-forwarder      enable building of proxy-forwarder (default: check)
  --disable-forwarder     disable building of proxy-forwarder],
ENABLE_FORWARDER=$enableval, ENABLE_FORWARDER=check)

AS_IF([test "x${ENABLE_DEBUG}" = "xyes"],
      [AC_DEFINE([DEBUG], 1, [Define if you want the debug output support compiled in.])],
      [AC_LIB_APPENDTOVAR([CXXFLAGS], [-D_NDEBUG])]
)

AC_ARG_WITH([examples], [
  --with-examples         build with examples (usually disabled)], [WITH_EXAMPLES=""], [WITH_EXAMPLES="no"])

AM_CONDITIONAL(BUILD_EXAMPLES, false)
AS_IF([test "x${WITH_EXAMPLES}" != "xno"], [AM_CONDITIONAL(BUILD_EXAMPLES, true)])

AC_ARG_WITH([agenpro-master], [
  --with-agenpro-master         build with AgenPro master templates (usually disabled)], [WITH_AGENPRO_MASTER=""], [WITH_AGENPRO_MASTER="no"])

AM_CONDITIONAL(BUILD_AGENPRO_MASTER, false)
AS_IF([test "x${WITH_AGENPRO_MASTER}" != "xno" ], [AM_CONDITIONAL(BUILD_AGENPRO_MASTER, true)])

AC_ARG_WITH([agenpro-subagent], [
  --with-agenpro-subagent         build with AgenPro subagent templates (usually disabled)], [WITH_AGENPRO_SUBAGENT=""], [WITH_AGENPRO_SUBAGENT="no"])

AM_CONDITIONAL(BUILD_AGENPRO_SUBAGENT, false)
AS_IF([test "x${WITH_AGENPRO_SUBAGENT}" != "xno" ], [AM_CONDITIONAL(BUILD_AGENPRO_SUBAGENT, true)])


dnl Checks for inet libraries:
AC_SEARCH_LIBS(gethostent, [nsl])
AC_SEARCH_LIBS(setsockopt, [socket net network])
AC_SEARCH_LIBS(connect, [inet])

AC_SEARCH_LIBS([clock_gettime],[rt posix4])

# Checks for optional dependencies
ACX_WIN32(
[],
[
    ACX_PTHREAD([
        AC_DEFINE(HAVE_PTHREAD, 1,
            [Define if you have POSIX threads libraries and header files.])
        CC="$PTHREAD_CC"
        CXX="$PTHREAD_CXX"
        AS_IF([test -n "$PTHREAD_LIBS"], [LIBS="$PTHREAD_LIBS $LIBS"])
        AS_IF([test -n "$PTHREAD_CFLAGS"], [
          CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
          CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
        ])
        ac_cv_enable_threads=yes
        
        dnl Checks for nanosleep library:
        AC_SEARCH_LIBS(nanosleep, [rt])
    ],
    [])
])

LINKFLAGS=""
CLIBFLAGS=""

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h ctype.h errno.h float.h limits.h memory.h netdb.h netinet/in.h poll.h signal.h stdio.h stdlib.h string.h strings.h sys/param.h sys/select.h sys/socket.h sys/time.h sys/timeb.h sys/types.h sys/unistd.h time.h unistd.h])

ACX_WIN32(
    AC_CHECK_HEADERS([io.h process.h winsock.h winsock2.h wstcpip.h wspiapi.h])
    AC_CHECK_LIB([wsock32],[main])
    AC_CHECK_LIB([ws2_32],[main]),
    []
)

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_UID_T
AC_CXX_BOOL

AS_IF([test "x$ac_cv_cxx_bool" = "xyes"], [NEED_DEFINE_BOOL=0], [NEED_DEFINE_BOOL=1])

AX_CXX_NAMESPACES
AX_CXX_NAMESPACE_STD
AX_CXX_HAVE_STL
AX_CXX_HAVE_STD
AX_CXX_TEMPLATES
AX_CXX_HEADER_STDCXX_98
AX_CXX_HEADER_STDCXX_TR1
# Check for C++0x features
AC_CXX_COMPILE_STDCXX_0X
AX_CXX_HEADER_STDCXX_0X


# Checks for libraries.
AS_IF([test "x${acx_with_libsnmp}" != "xyes"], [AC_MSG_ERROR([libsnmp++ is mandatory and must not be disabled])])
AS_IF([test "x${acx_with_libagent}" != "xyes"], [AC_MSG_ERROR([libagent++ is mandatory and must not be disabled])])
AC_LANG_PUSH([C++])

dnl pull the rabbit out of the hat to avoid regexp in ACX_CHECK_LIB_FLAGS searching for winsock2.h
acx_include_sockets="\
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
#endif"

ACX_CHECK_LIB_FLAGS([snmp++], [], [
AC_LANG_SOURCE([
#ifdef SNMP_PP_NAMESPACE
Snmp_pp::
#endif
DefaultLog::log ()
])
  ], [
AC_LANG_SOURCE([
$acx_include_sockets

#include <snmp_pp/snmp_pp.h>
])
  ], [snmp++-3 >= 3.3.0], [
    WITH_LIBSNMP=1
    AC_LIB_APPENDTOVAR([LINKFLAGS], ${LIBS_SNMP})
    AC_LIB_APPENDTOVAR([CLIBFLAGS], ${INC_SNMP})
    AC_MSG_NOTICE([compiling in support for libsnmp++ (LIBS="$LIBS_SNMP}")])
  ]
)
AC_MSG_CHECKING([if libsnmp++ is built with SNMPv3 support])
acx_cppflags_safe="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $CLIBFLAGS"
AC_COMPILE_IFELSE([
AC_LANG_SOURCE([
$acx_include_sockets

#include <snmp_pp/config_snmp_pp.h>
#if !defined(_SNMPv3)
#error ProxyForwarder requires SNMPv3
#endif

int main() { return 0; }
])
  ], [
    AC_MSG_RESULT([yes])
    acx_libsnmp_snmpv3=yes
  ], [
    AC_MSG_RESULT([no])
    acx_libsnmp_snmpv3=no
  ]
)
CPPFLAGS="$acx_cppflags_safe"

AC_CHECK_TYPES([struct sockaddr_in6],,,[
AC_LANG_SOURCE([
$acx_include_sockets
])
])
ACX_CHECK_LIB_FLAGS([agent++], [], [
AC_LANG_SOURCE([
#ifdef AGENTPP_NAMESPACE
Agentpp::
#endif
Vacm::getErrorMsg(0)
])
  ], [
AC_LANG_SOURCE([
$acx_include_sockets

#include <agent_pp/agent++.h>
#include <agent_pp/vacm.h>
])
  ], [agent++-4 >= 4.0.0], [
    WITH_LIBAGENT=1
    AC_LIB_APPENDTOVAR([LINKFLAGS], ${LIBS_AGENT})
    AC_LIB_APPENDTOVAR([CLIBFLAGS], ${INC_AGENT})
    AC_MSG_NOTICE([compiling in support for libagent++ (LIBS="$LIBS_AGENT}")])
  ]
)

AC_MSG_CHECKING([if libagent++ is built with ThreadPool support])
acx_cppflags_safe="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $CLIBFLAGS"
AC_COMPILE_IFELSE([
AC_LANG_SOURCE([
$acx_include_sockets

#include <agent_pp/agent++.h>
#if !defined(AGENTPP_USE_THREAD_POOL)
#error AgentX++ requires AGENT++ compiled with AGENTPP_USE_THREAD_POOL
#endif

int main() { return 0; }
])
  ], [
    AC_MSG_RESULT([yes])
    acx_libagentpp_threadpool=yes
  ], [
    AC_MSG_RESULT([no])
    acx_libagentpp_threadpool=no
  ]
)
CPPFLAGS="$acx_cppflags_safe"

AC_CHECK_TYPES([struct sockaddr_in6],,,[
AC_LANG_SOURCE([
$acx_include_sockets
])
])

dnl Determine what socket length (socklen_t) data type is
AC_MSG_CHECKING([for type of socket length (socklen_t)])
AC_COMPILE_IFELSE([
AC_LANG_SOURCE([
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>

int
main()
{
  (void)getsockopt (1, 1, 1, NULL, (socklen_t *)NULL);
  
  return 0;
}
])
],[
  AC_MSG_RESULT(socklen_t *)
  SNMP_SOCKLEN_T=socklen_t],[
  AC_COMPILE_IFELSE([
AC_LANG_SOURCE([
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>

int
main()
{
  (void)getsockopt (1, 1, 1, NULL, (size_t *)NULL);
  
  return 0;
}
])
],[
    AC_MSG_RESULT(size_t *)
    SNMP_SOCKLEN_T=size_t],[
    AC_COMPILE_IFELSE([
AC_LANG_SOURCE([
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>

int
main()
{
  (void)getsockopt (1, 1, 1, NULL, (int *)NULL);
  
  return 0;
}
])
],[
      AC_MSG_RESULT(int *)
      SNMP_SOCKLEN_T=int],[
      AC_MSG_WARN(could not determine)
      SNMP_SOCKLEN_T="int"])])])
AC_DEFINE_UNQUOTED(SNMP_SOCKLEN_T, $SNMP_SOCKLEN_T, [Determine what socket length (socklen_t) data type is])
AC_LANG_POP([C++])

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([getaddrinfo gethostbyaddr gethostbyaddr_r gethostbyname gethostbyname2 gethostbyname_r gethostname gettimeofday inet_aton inet_ntoa inet_pton inet_ntop isdigit localtime_r memset mkdir poll rmdir select socket strchr strerror strstr tzset clock_gettime])

# check this separately if it produces different results on Win2k or WinXP
AC_CHECK_DECLS([getaddrinfo],,,[
AC_LANG_SOURCE([
#if HAVE_WINSOCK2_H
#  include <winsock2.h>
#  if HAVE_WS2TCPIP_H
#    // Fake windows XP for getaddrinfo if IPv6
#    if HAVE_STRUCT_SOCKADDR_IN6
#      define _WIN32_WINNT 0x0501
#    endif
#    include <ws2tcpip.h>
#  endif
#  if HAVE_WSPIAPI_H
#    include <wspiapi.h>
#  endif
#else
#  include <netdb.h>
#endif
])
])

AS_IF(
  [test "x${ENABLE_NAMESPACE}" = "xyes"],
  [
    AS_IF([test "x$ax_cv_cxx_namespaces" = "xyes"],
          [
            WITH_NAMESPACE=1
            AC_MSG_NOTICE([enable using of namespace Agentpp])
          ],
          [AC_MSG_FAILURE([C++ compiler does not support namespaces])])
  ],
  [test "x${ENABLE_NAMESPACE}" = "xcheck"],
  [
    AS_IF([test "x$ax_cv_cxx_namespaces" = "xyes"],
          [
            WITH_NAMESPACE=1
            AC_MSG_NOTICE([enable using of namespace Agentpp])
          ],
          [
            WITH_NAMESPACE=0
            AC_MSG_NOTICE([disable using of namespace Agentpp])
          ])
  ],
  [
    WITH_NAMESPACE=0
    AC_MSG_NOTICE([disable using of namespace Agentpp])
  ]
)

dnl feature test which requires snmp++ feature
WITH_PROXY=0
AS_IF([test "x${ENABLE_FORWARDER}" = "xyes" -a "x${ENABLE_PROXY}" = "xno"], [AC_MSG_ERROR([proxy-feature must not disabled to enable proxy-forwarder])])
AS_IF([test "x${ENABLE_FORWARDER}" != "xno"], [
  AS_IF([test "x$acx_libsnmp_snmpv3" = "xyes"],
    [
      WITH_PROXY=1
      WITH_PROXY_FORWARDER=1
    ],
    [
      AS_IF([test "x${ENABLE_FORWARDER}" = "xyes"], [AC_MSG_ERROR([ProxyForwarder requires libsnmp++ compiled with SNMPv3 support])], [WITH_PROXY_FORWARDER=0])
    ]
  )
], [WITH_PROXY_FORWARDER=0])
AS_IF([test "x${ENABLE_PROXY}" = "xyes"], [WITH_PROXY=1])

AS_IF([test "${WITH_PROXY}" -eq 1],
  [AC_MSG_NOTICE(enable snmp proxy)],
  [AC_MSG_NOTICE(disable snmp proxy)]
)
AS_IF([test "${WITH_PROXY_FORWARDER}" -eq 1],
  [AC_MSG_NOTICE(enable snmp proxy-forwarder)],
  [AC_MSG_NOTICE(disable snmp proxy-forwarder)]
)

dnl feature test AgentX++
AS_IF([test "x${ENABLE_MASTER}" = "xyes"], [WITH_MASTER="1"])
AS_IF([test "x${ENABLE_SUBAGENT}" = "xyes"], [WITH_SUBAGENT="1"])
AS_IF([test "x${ENABLE_MASTER}" = "xno" -a "x${ENABLE_SUBAGENT}" = "xno"], [AC_MSG_ERROR([at least one of subagent support needs to be compiled in if master support is disabled])], [WITH_SUBAGENT=1])
AS_IF([test "${WITH_MASTER}" = "1"],
  [AC_MSG_NOTICE(enable master agent)],
  [AC_MSG_NOTICE(disable master agent)]
)
AS_IF([test "${WITH_SUBAGENT}" = "1"],
  [AC_MSG_NOTICE(enable subagent)],
  [AC_MSG_NOTICE(disable subagent)]
)


dnl Libtool setup.
dnl must be latest to catch rpath compiled in by changes made during checks before
LT_INIT
AC_PROG_LIBTOOL

dnl finish
AC_SUBST(AGENTX_PP_MAJOR_VERSION)
AC_SUBST(AGENTX_PP_MINOR_VERSION)
AC_SUBST(AGENTX_PP_MICRO_VERSION)

AC_SUBST(LINKFLAGS)
AC_SUBST(CLIBFLAGS)

AC_SUBST(WITH_MODULES)
AC_SUBST(WITH_NAMESPACE)

AC_SUBST(WITH_PROXY)
AC_SUBST(WITH_PROXY_FORWARDER)
AM_CONDITIONAL(WITH_PROXY_SOURCES, test "${WITH_PROXY}" = "1")
AM_CONDITIONAL(WITH_PROXY_FORWARDER, test "${WITH_PROXY_FORWARDER}" = "1")
AM_CONDITIONAL(WITH_MASTER_SOURCES, test "${WITH_MASTER}" = "1")
AM_CONDITIONAL(WITH_SUBAGENT_SOURCES, test "${WITH_SUBAGENT}" = "1")

AC_SUBST(NEED_DEFINE_BOOL)

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 include/agentx_pp/Makefile
                 examples/Makefile
                 examples/master/Makefile
                 examples/master/src/Makefile
                 examples/subagent/Makefile
                 examples/subagent/include/Makefile
                 examples/subagent/src/Makefile
                 agenpro/master/include/Makefile
                 agenpro/master/Makefile
                 agenpro/master/src/Makefile
                 agenpro/Makefile
                 agenpro/subagent/include/Makefile
                 agenpro/subagent/Makefile
                 agenpro/subagent/src/Makefile
                 src/Makefile])
AC_OUTPUT(agentx++-2.pc include/agentx_pp/agentx++.h)
