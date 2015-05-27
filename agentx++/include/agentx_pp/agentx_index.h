/*_############################################################################
  _## 
  _##  agentx_index.h  
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

#ifndef agentx_index_h_
#define agentx_index_h_

#include <agent_pp/mib_complex_entry.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

/*------------------- class AgentXIndexEntry -----------------------*/

class AgentXIndexEntry: public MibStaticEntry {
 public:
	AgentXIndexEntry(u_int sid, const Oidx& o, const NS_SNMP SnmpSyntax& v):
	  MibStaticEntry(o, v) { session_id = sid; }
	AgentXIndexEntry(const AgentXIndexEntry& o): MibStaticEntry(o)
	  { session_id = o.session_id; }
	virtual ~AgentXIndexEntry() { }

	u_int		get_session_id() { return session_id; }
 protected:
	u_int	       	session_id;
};


/*--------------------- class AgentXIndex -----------------------*/

class AgentXIndex: public MibStaticEntry {
 public:
	AgentXIndex(const Vbx&); 

	AgentXIndex(AgentXIndex&);

	virtual ~AgentXIndex();

	virtual int		allocate(u_int, const Vbx&, bool); 
	virtual int		release(u_int, const Vbx&, bool);

	virtual int		new_index(u_int, Vbx&, bool);
	virtual int		any_index(u_int, Vbx&, bool); 

	virtual void		remove_session(u_int);

 protected:

      	virtual Oidx*		get_index_value(const Vbx&);
	
	virtual bool	       	is_simple_syntax();

	OidList<AgentXIndexEntry> allocatedValues;
	OidList<AgentXIndexEntry> usedValues;
};


/*--------------------- class AgentXIndexDB -----------------------*/

class AgentXIndexDB: public Synchronized {

 public:
	AgentXIndexDB(const Oidx& c) { context = c; }	

	/**
	 * Destructor.
	 */
	virtual	~AgentXIndexDB();

	static bool		is_valid_syntax(NS_SNMP SmiUINT32);
  
	virtual int	        allocate(u_int, const Vbx&, bool);
	virtual int		release(u_int, const Vbx&, bool);
	virtual void		remove_session(u_int);

	virtual int		new_index(u_int, Vbx&, bool);
	virtual int		any_index(u_int, Vbx&, bool);

        Oidx*			key() { return &context; }

 protected:

	OidList<AgentXIndex> indexes;
	Oidx		     context;
};

#ifdef AGENTPP_NAMESPACE
}
#endif


#endif
