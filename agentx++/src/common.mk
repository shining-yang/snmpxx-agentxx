  ############################################################################
  ## 
  ##  common.mk  
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

INCLUDE		= -I../include -I../../agent++/include \
		  -I../../snmp++/include

SNMPLIBPATH     = ../../snmp++/lib
AGENTLIBPATH	= ../../agent++/lib
LIBDESPATH      = ../../libdes

LIBAGENTXPLUS	= ../lib/libagentx++.a
SHLIBAGENTXPLUS	= ../lib/libagentx++.so

LIB	        = -L$(AGENTLIBPATH) -L$(SNMPLIBPATH) -L$(RSALIBPATH) \
	     	  -lpthread -lagent++ -lsnmp++ -ldes $(SYSLIBS)

OBJS		= agentx.o agentx_pdu.o agentx_session.o \
		  agentx_queue.o agentx_master.o agentx_peer.o \
		  agentx_request.o agentx_reg.o \
		  agentx_node.o agentx_subagent.o agentx_mib.o \
		  agentx_index.o agentpp_agentx_mib.o

SHOBJS		= agentx_sh.o agentx_pdu_sh.o \
		  agentx_session_sh.o agentx_queue_sh.o agentx_master_sh.o \
		  agentx_peer_sh.o agentx_request_sh.o agentx_reg_sh.o \
		  agentx_node_sh.o agentx_subagent_sh.o \
		  agentx_mib_sh.o agentx_index_sh.o agentpp_agentx_mib_sh.o

HEADERS		= ../include/agentx_pp/agentx.h \
		  ../include/agentx_pp/agentx_pdu.h \
		  ../include/agentx_pp/agentx_session.h \
		  ../include/agentx_pp/agentx_reg.h \
		  ../include/agentx_pp/agentx_queue.h \
		  ../include/agentx_pp/agentx_master.h \
		  ../include/agentx_pp/agentx_peer.h \
		  ../include/agentx_pp/agentx_request.h \
		  ../include/agentx_pp/agentx_threads.h \
		  ../include/agentx_pp/agentx_node.h \
		  ../include/agentx_pp/agentx_subagent.h \
		  ../include/agentx_pp/agentx_mib.h \
		  ../include/agentx_pp/agentx_def.h \
		  ../include/agentx_pp/agentx_index.h \
		  ../include/agentx_pp/agentpp_agentx_mib.h 

all:		lib shlib

lib:		$(LIBAGENTXPLUS)

shlib:		$(SHLIBAGENTXPLUS)

clean:  
	$(RM) *.o *~ ../include/agentx_pp/*~

clobber: clean
	$(RM) $(LIBAGENTXPLUS) $(SHLIBAGENTXPLUS)

#compile rules


$(LIBAGENTXPLUS):	$(OBJS)
			ar -rv $(LIBAGENTXPLUS) $(OBJS)

$(SHLIBAGENTXPLUS):	$(SHOBJS)
			$(CPP) $(SHARED) -o $(SHLIBAGENTXPLUS) $(SHOBJS)

%.o:		%.cpp $(HEADERS)
		$(RM) $@
		$(CPP) $(CFLAGS) $(CLINK) $@ $(INCLUDE) $< 

%_sh.o:		%.cpp $(HEADERS)
		$(RM) $@
		$(CPP) $(CFLAGS) $(SHARED) $(CLINK) $@ $(INCLUDE) $<

