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

agentppincdir = $(includedir)/agent_pp

agentppinc_HEADERS =	agent++.h \
			agentpp_config_mib.h \
			agentpp_simulation_mib.h \
			avl_map.h \
			entry.h \
			List.h \
			map.h \
			mib_avl_map.h \
			mib_complex_entry.h \
			mib_context.h \
			mib_entry.h \
			mib.h \
			mib_map.h \
			mib_policy.h \
			notification_log_mib.h \
			notification_originator.h \
			oidx_defs.h \
			oidx_ptr.h \
			request.h \
			sim_mib.h \
			snmp_community_mib.h \
			snmp_counters.h \
			snmp_group.h \
			snmp_notification_mib.h \
			snmp_pp_ext.h \
			snmp_request.h \
			snmp_target_mib.h \
			snmp_textual_conventions.h \
			system_group.h \
			threads.h \
			tools.h \
			v3_mib.h \
			vacm.h

if WITH_PROXY_SOURCES
agentppinc_HEADERS +=	snmp_proxy_mib.h
if WITH_PROXY_FORWARDER
agentppinc_HEADERS +=	proxy_forwarder.h
else
agentppinc_HEADERS +=	mib_proxy.h
endif
endif

install-exec-hook:
	$(mkinstalldirs) $(DESTDIR)$(agentppincdir)

EXTRA_DIST = agent++.h.in
