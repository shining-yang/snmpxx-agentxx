/*_############################################################################
  _## 
  _##  agentx_threads.h  
  _## 
  _##
  _##  AgentX++ 2
  _##  -------------------------------------
  _##  Copyright (C) 2000-2013 - Frank Fock
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


#ifndef _agentx_threads_h_
#define _agentx_threads_h_

#include <agent_pp/agent++.h>
#include <agent_pp/threads.h>

#ifndef _THREADS
#error "To use AgentX++ threads must be enabled in AGENT++"
#endif

#define SYNCHRONIZED(x) { Lock _synchronize(*this); x }

#endif
