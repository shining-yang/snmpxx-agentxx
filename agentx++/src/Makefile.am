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


AM_CPPFLAGS = -I$(top_srcdir)/include $(PTHREAD_CFLAGS) @CLIBFLAGS@

lib_LTLIBRARIES = libagentx++.la

libagentx___la_SOURCES = agentx.cpp agentx_pdu.cpp agentx_session.cpp \
		  agentx_queue.cpp agentx_peer.cpp \
		  agentx_request.cpp agentx_reg.cpp \
		  agentx_node.cpp agentx_mib.cpp \
		  agentx_index.cpp agentpp_agentx_mib.cpp 

if WITH_MASTER_SOURCES
libagentx___la_SOURCES +=	agentx_master.cpp
endif
if WITH_SUBAGENT_SOURCES
libagentx___la_SOURCES +=	agentx_subagent.cpp
endif

libagentx___la_LDFLAGS =	-version-info @AGENTX_PP_MAJOR_VERSION@@AGENTX_PP_MINOR_VERSION@:@AGENTX_PP_MICRO_VERSION@:0 @LINKFLAGS@
