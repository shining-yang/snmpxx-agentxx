/*_############################################################################
  _## 
  _##  agentx_peer.cpp  
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

#include <agentx_pp/agentx_def.h>

#include <agentx_pp/agentx_peer.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


// static const char *loggerModuleName = "agentx++.agentx_peer";


AgentXPeer::AgentXPeer()
{
	sd     	= 0;
	nSessions = 0;
	connectTime = 0;
	timeout = 0;
	mibID = 0;
	closing = FALSE;
}

AgentXPeer::AgentXPeer(int socket, time_t ct, int to, u_int mid): 
  sd(socket), nSessions(0), connectTime(ct), timeout(to), mibID(mid)
{ 
	closing = FALSE;
}

AgentXPeer::AgentXPeer(const AgentXPeer& other) {
	sd = other.sd;
	nSessions = other.nSessions;
	connectTime = other.connectTime;
	timeout = other.timeout;
	mibID = other.mibID;
	closing = other.closing;
}

AgentXPeer::~AgentXPeer()
{ 
}

#ifdef AGENTPP_NAMESPACE
}
#endif

