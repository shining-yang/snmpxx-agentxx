  ############################################################################
  ## 
  ##  AGENT++ 4.0 - configure.ac  
  ## 
  ##  Copyright (C) 2000-2013  Frank Fock and Jochen Katz (agentpp.com)
  ##  
  ##  Licensed under the Apache License, Version 2.0 (the "License");
  ##  you may not use this file except in compliance with the License.
  ##  You may obtain a copy of the License at
  ##  
  ##      http:/www.apache.org/licenses/LICENSE-2.0
  ##  
  ##  Unless required by applicable law or agreed to in writing, software
  ##  distributed under the License is distributed on an "AS IS" BASIS,
  ##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  ##  See the License for the specific language governing permissions and
  ##  limitations under the License.
  ##  
  ##########################################################################*
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# don't require higher autoconf version for win32 support
AC_PREREQ([2.63])
AC_INIT([agent++], [4.0.4], [support@agentpp.com], [agent++])
AGENT_PP_MAJOR_VERSION=4
AGENT_PP_MINOR_VERSION=0
AGENT_PP_MICRO_VERSION=4
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_SRCDIR([src/mib.cpp])
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

AC_ARG_ENABLE(debug,
[  --disable-debug         disable support for debugging output],
ENABLE_DEBUG=$enableval, ENABLE_DEBUG=yes)
AC_ARG_ENABLE(namespace,
[  --enable-namespace      enable using of namespace (default: on if supported)
  --disable-namespace     disable using of namespace],
ENABLE_NAMESPACE=$enableval, ENABLE_NAMESPACE=check)
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
AS_IF([test "x${WITH_EXAMPLES}" != "xno" ], [AM_CONDITIONAL(BUILD_EXAMPLES, true)])

AC_ARG_WITH([agenpro], [
  --with-agenpro         build with AgenPro templates (usually disabled)], [WITH_AGENPRO=""], [WITH_AGENPRO="no"])

AM_CONDITIONAL(BUILD_AGENPRO, false)
AS_IF([test "x${WITH_AGENPRO}" != "xno" ], [AM_CONDITIONAL(BUILD_AGENPRO, true)])

AC_ARG_WITH([agenpro-simagent], [
  --with-agenpro-simagent         build with AgenPro simagent instead regular templates (usually disabled)], [WITH_AGENPRO_SIMAGENT=""], [WITH_AGENPRO_SIMAGENT="no"])

AM_CONDITIONAL(BUILD_AGENPRO_SIMAGENT, false)
AS_IF([test "x${WITH_AGENPRO_SIMAGENT}" != "xno" ], [AM_CONDITIONAL(BUILD_AGENPRO_SIMAGENT, true)])


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
        dnl Determine if pthread_t is int (32bit) or long (64bit)
        AC_MSG_CHECKING([for type of (pthread_t)])
        AC_COMPILE_IFELSE([
        AC_LANG_SOURCE([
        #include <stddef.h>
        #include <sys/types.h>
        #include <sys/socket.h>
        #include <pthread.h>

        int
        main()
        {
          pthread_t p = 2^60;
          long i = p;
          return 0;
        }
        ])
        ],[
          AC_MSG_RESULT(long)
          AGENTPP_OPAQUE_PTHREAD_T=long],[
          AC_COMPILE_IFELSE([
        AC_LANG_SOURCE([
        #include <stddef.h>
        #include <sys/types.h>
        #include <sys/socket.h>
        #include <pthread.h>

        int
        main()
        {
          pthread_t p = 2^31;
          int i = p;
          return 0;
        }
        ])
        ],[
            AC_MSG_RESULT(int)
            AGENTPP_OPAQUE_PTHREAD_T=int],[
              AC_MSG_WARN(could not determine pthread_t data type)
              AGENTPP_OPAQUE_PTHREAD_T="int"])])
        AC_DEFINE_UNQUOTED(AGENTPP_OPAQUE_PTHREAD_T, $AGENTPP_OPAQUE_PTHREAD_T, [Determine what pthread_t data type is])
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

# Checks for libraries.
AS_IF([test "x${acx_with_libsnmp}" != "xyes"], [AC_MSG_ERROR([libsnmp++ is mandatory and must not be disabled])])
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

dnl Libtool setup.
dnl must be latest to catch rpath compiled in by changes made during checks before
LT_INIT
AC_PROG_LIBTOOL

dnl finish
AC_SUBST(AGENT_PP_MAJOR_VERSION)
AC_SUBST(AGENT_PP_MINOR_VERSION)
AC_SUBST(AGENT_PP_MICRO_VERSION)

AC_SUBST(LINKFLAGS)
AC_SUBST(CLIBFLAGS)

AC_SUBST(WITH_MODULES)
AC_SUBST(WITH_NAMESPACE)

AC_SUBST(WITH_PROXY)
AC_SUBST(WITH_PROXY_FORWARDER)
AM_CONDITIONAL(WITH_PROXY_SOURCES, test "${WITH_PROXY}" = "1")
AM_CONDITIONAL(WITH_PROXY_FORWARDER, test "${WITH_PROXY_FORWARDER}" = "1")

AC_SUBST(NEED_DEFINE_BOOL)

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 include/agent_pp/Makefile
                 examples/Makefile
                 examples/atm_mib/Makefile
                 examples/atm_mib/include/Makefile
                 examples/atm_mib/src/Makefile
                 examples/cmd_exe_mib/Makefile
                 examples/cmd_exe_mib/include/Makefile
                 examples/cmd_exe_mib/src/Makefile
                 examples/dynamic_table/Makefile
                 examples/dynamic_table/src/Makefile
                 examples/proxy/Makefile
                 examples/proxy/src/Makefile
                 examples/proxy_forwarder/Makefile
                 examples/proxy_forwarder/src/Makefile
                 examples/static_table/Makefile
                 examples/static_table/src/Makefile
                 agenpro/Makefile
                 agenpro/agent/Makefile
                 agenpro/agent/include/Makefile
                 agenpro/agent/src/Makefile
                 agenpro/simagent/Makefile
                 agenpro/simagent/include/Makefile
                 agenpro/simagent/src/Makefile
                 src/Makefile])
AC_OUTPUT(agent++-4.pc include/agent_pp/agent++.h)
