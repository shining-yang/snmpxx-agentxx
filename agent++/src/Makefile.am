  ############################################################################
  ## 
  ##  AGENT++ 4.0 - Makefile.am  
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

AM_CPPFLAGS = -I$(top_srcdir)/include $(PTHREAD_CFLAGS) @CLIBFLAGS@

lib_LTLIBRARIES = libagent++.la

libagent___la_SOURCES =	agentpp_config_mib.cpp \
			agentpp_simulation_mib.cpp \
			avl_map.cpp \
			map.cpp \
			mib_avl_map.cpp \
			mib_complex_entry.cpp \
			mib_context.cpp \
			mib.cpp \
			mib_entry.cpp \
			mib_map.cpp \
			mib_policy.cpp \
			notification_log_mib.cpp \
			notification_originator.cpp \
			request.cpp sim_mib.cpp \
			snmp_community_mib.cpp \
			snmp_counters.cpp \
			snmp_group.cpp \
			snmp_notification_mib.cpp \
			snmp_pp_ext.cpp \
			snmp_request.cpp \
			snmp_target_mib.cpp \
			snmp_textual_conventions.cpp \
			system_group.cpp \
			threads.cpp \
			tools.cpp \
			v3_mib.cpp \
			vacm.cpp

if WITH_PROXY_SOURCES
libagent___la_SOURCES+=	snmp_proxy_mib.cpp
if WITH_PROXY_FORWARDER
libagent___la_SOURCES+=	proxy_forwarder.cpp
else
libagent___la_SOURCES+=	mib_proxy.cpp
endif
endif

libagent___la_LDFLAGS =	-version-info @AGENT_PP_MAJOR_VERSION@@AGENT_PP_MINOR_VERSION@:@AGENT_PP_MICRO_VERSION@:0 @LINKFLAGS@
