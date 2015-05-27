/*_############################################################################
  _## 
  _##  agentx_peer.h  
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


#ifndef _agentx_peer_h_
#define _agentx_peer_h_

#include <sys/types.h>
#include <agent_pp/agent++.h>
#include <agentx_pp/agentx_def.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


class AGENTXPP_DECL AgentXPeer {

 public:
	AgentXPeer();
	AgentXPeer(int, time_t, int, u_int);
	AgentXPeer(const AgentXPeer&);
	~AgentXPeer();

	int	operator==(const AgentXPeer& other) const 
					{ return sd == other.sd; }

	int	sd;
	int	nSessions;
	time_t  connectTime;
	int     timeout;
	u_int	mibID;
        bool closing;
};

#ifdef AGENTPP_NAMESPACE
}
#endif


#endif
