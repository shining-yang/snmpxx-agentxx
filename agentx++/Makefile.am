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
SUBDIRS = include src $(example_SUBDIRS) ${agenpro_SUBDIRS}
ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = agentx++-2.pc.in libagentx.h CHANGES README.autoconf \
    LICENSE_v2.txt doc_config src/common.mk src/Makefile.linux

if BUILD_EXAMPLES
example_SUBDIRS = examples
endif

if BUILD_AGENPRO_MASTER
agenpro_SUBDIRS = agenpro
else 
if BUILD_AGENPRO_SUBAGENT
agenpro_SUBDIRS = agenpro
endif
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = agentx++-2.pc

library_includedir = $(includedir)
library_include_HEADERS= libagentx.h
