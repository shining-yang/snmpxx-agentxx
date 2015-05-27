/*_############################################################################
  _## 
  _##  agentx_reg.cpp  
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

#include <agentx_pp/agentx_reg.h>

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

// static const char *loggerModuleName = "agentx++.agentx_reg";



AgentXRegEntry::AgentXRegEntry(u_int id, const AgentXRegion& r,
			       u_int p, u_int sp, const OctetStr& c, 
			       u_char t)
{
	sessionID = id;
	region = r;
	priority = p;
	specific = sp;
	context = c;
	timeout = t;
}

AgentXRegEntry::AgentXRegEntry(const AgentXRegEntry& other)
{
	sessionID = other.sessionID;
	region = other.region;
	priority = other.priority;
	specific = other.specific;
	context = other.context;
	timeout = other.timeout;
}

AgentXRegEntry::~AgentXRegEntry()
{
}

AgentXRegEntry* AgentXRegEntry::clone()
{
	return new AgentXRegEntry(*this);
}

bool AgentXRegEntry::is_duplicate_of(const AgentXRegEntry& other) 
{
	return ((region == other.region) && (context == other.context));
}

bool AgentXRegEntry::is_overlapping(const AgentXRegEntry& other)
{
	return ((context == other.context) && (region.overlaps(other.region)));
}

bool AgentXRegEntry::operator<(const AgentXRegEntry& other) 
{
	return ((region.get_lower().len() > other.region.get_lower().len()) ||
		((region.get_lower().len() == other.region.get_lower().len()) &&
		 (priority < other.priority)));
}

bool AgentXRegEntry::operator>(const AgentXRegEntry& other) 
{
	return ((region.get_lower().len() < other.region.get_lower().len()) ||
		((region.get_lower().len() == other.region.get_lower().len()) &&
		 (priority > other.priority)));
}

bool AgentXRegEntry::operator==(const AgentXRegEntry& other) 
{
	return ((sessionID == other.sessionID) && 
		(priority == other.priority) &&
		(region == other.region) &&
		(specific == other.specific) &&
		(context == other.context));
}

#ifdef AGENTPP_NAMESPACE
}
#endif

