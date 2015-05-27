  ############################################################################
  ## 
  ##  Makefile.am  
  ## 
  ##
  ##  AgentX++ 2
  ##  -------------------------------------
  ##  Copyright (C) 2000-2013 - Frank Fock
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

agentxppincdir = $(includedir)/agentx_pp

agentxppinc_HEADERS = agentx++.h \
		  agentx.h \
		  agentx_pdu.h \
		  agentx_session.h \
		  agentx_reg.h \
		  agentx_queue.h \
		  agentx_peer.h \
		  agentx_request.h \
		  agentx_threads.h \
		  agentx_node.h \
		  agentx_mib.h \
		  agentx_def.h \
		  agentx_index.h \
		  agentpp_agentx_mib.h 
if WITH_MASTER_SOURCES
agentxppinc_HEADERS +=	agentx_master.h
endif
if WITH_SUBAGENT_SOURCES
agentxppinc_HEADERS +=	agentx_subagent.h
endif

install-exec-hook:
	$(mkinstalldirs) $(DESTDIR)$(agentxppincdir)

EXTRA_DIST = agentx++.h.in
