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

DEPS = $(top_builddir)/src/libagentx++.la
LDADDS = $(top_builddir)/src/libagentx++.la @LINKFLAGS@

AM_CPPFLAGS = -I$(top_builddir)/include -I$(top_srcdir)/include -I@srcdir@/../include $(PTHREAD_CFLAGS) @CLIBFLAGS@

noinst_PROGRAMS = subagent subagent2

subagent_SOURCES = 	subagent.cpp atm_mib.cpp if_mib.cpp
subagent_LDFLAGS =
subagent_DEPENDENCIES =	$(DEPS)
subagent_LDADD =	$(LDADDS)

subagent2_SOURCES = 	subagent2.cpp atm_mib.cpp if_mib.cpp \
			agentpp_test_mib.cpp agentpp_notifytest_mib.cpp
subagent2_LDFLAGS =
subagent2_DEPENDENCIES =    $(DEPS)
subagent2_LDADD =	$(LDADDS)
