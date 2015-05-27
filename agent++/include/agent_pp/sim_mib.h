/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - sim_mib.h  
  _## 
  _##  Copyright (C) 2000-2013  Frank Fock and Jochen Katz (agentpp.com)
  _##  
  _##  Licensed under the Apache License, Version 2.0 (the "License");
  _##  you may not use this file except in compliance with the License.
  _##  You may obtain a copy of the License at
  _##  
  _##      http://www.apache.org/licenses/LICENSE-2.0
  _##  
  _##  Unless required by applicable law or agreed to in writing, software
  _##  distributed under the License is distributed on an "AS IS" BASIS,
  _##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  _##  See the License for the specific language governing permissions and
  _##  limitations under the License.
  _##  
  _##########################################################################*/


#ifndef sim_mib_h_
#define sim_mib_h_

#include <agent_pp/agent++.h>
#include <agent_pp/mib.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

/**
 * The class SimMib can be used to create simulation agents from
 * real agents. Simply use SimMib instead of Mib and add MIB objects
 * like MibLeaf and MibTable to it. Then the content of the MIB can be 
 * cloned from a SNMPv1/v2c agent.
 *
 * @author Frank Fock
 * @version 3.1
 */

class AGENTPP_DECL SimMib:public Mib 
{
 public:
	/**
	 * Default Constructor
	 */
	SimMib();

	/**
	 * Desctructor
	 */
	virtual ~SimMib();
};


/**
 * The class SimMibLeaf can be used to create simulation agents.
 *
 * @author Frank Fock
 * @version 3.1c
 */

class AGENTPP_DECL SimMibLeaf: public MibLeaf 
{
 public:
  /**
   * Default constructor.
   */
  SimMibLeaf();

  /**
   * Construct a MibLeaf object.
   *
   * @param o - The object identifier of the managed object. If it is part
   *            of a table, only the column subidentifier is given. If it
   *            is a scalar object the object identifier given includes the
   *            full registration oid including the suffix ".0".
   * @param a - The maximum access rights for the managed object.
   * @param s - The syntax of the managed object.
   * @see Oidx, mib_access
   */
  SimMibLeaf(const Oidx&, mib_access, NS_SNMP SmiUINT32);

  /**
   * Construct a MibLeaf object.
   *
   * @param o - The object identifier of the managed object. If it is part
   *            of a table, only the column subidentifier is given. If it
   *            is a scalar object the object identifier given includes the
   *            full registration oid including the suffix ".0".
   * @param a - The maximum access rights for the managed object.
   * @param s - A pointer to the SnmpSyntax object to be used for storing
   *            the value of the managed object. NOTE: Do not delete this
   *            pointer from outside MibLeaf. It will be deleted by ~MibLeaf
   * @see Oidx, mib_access
   */
  SimMibLeaf(const Oidx&, mib_access, NS_SNMP SnmpSyntax*);

  /**
   * Construct a MibLeaf object.
   *
   * @param o - The object identifier of the managed object. If it is part
   *            of a table, only the column subidentifier is given. If it
   *            is a scalar object the object identifier given includes the
   *            full registration oid including the suffix ".0".
   * @param a - The maximum access rights for the managed object.
   * @param s - A pointer to the SnmpSyntax object to be used for storing
   *            the value of the managed object. NOTE: Do not delete this
   *            pointer from outside MibLeaf. It will be deleted by ~MibLeaf
   * @param d - If TRUE the value of the object pointed by s will be used
   *            as default value, otherwise the managed object has no default
   *            value.
   * @see Oidx, mib_access
   */
  SimMibLeaf(const Oidx&, mib_access, NS_SNMP SnmpSyntax*, bool);

  /**
   * Copy Constructor.
   */
  SimMibLeaf(const SimMibLeaf&);

  /**
   * Destructor.
   */
  virtual ~SimMibLeaf();

  /**
   * Clone the receiver object.
   * 
   * @return A pointer to the clone.
   */
  virtual MibEntryPtr	clone();

  /**
   * Return the maximum access rights for the managed object
   * represented by the receiver node.
   * 
   * @return The maximum access (one of the following values:
   *         NOACCESS, READONLY, READWRITE, or READCREATE)
   */
  virtual mib_access    get_access();

  /**
   * Set config mode. All SimMibLeaf objects acting as a leaf object
   * get access rights of read-write and all SimMibLeaf objects acting
   * as a columnar object get read-create access rights.
   */ 
  static void		set_config_mode();
  /**
   * Unset config mode. All SimMibLeaf objects get their original
   * access rights (i.e., those rights they got when they have been
   * constructed). 
   */ 
  static void		unset_config_mode();

  /**
   * Get the current config mode.
   *
   * @return 
   *    true if the config mode is set, false otherwise.
   */
  static bool	get_config_mode();

 protected:

  static bool	configMode;

};

class AGENTPP_DECL SimMibTable: public MibTable {
 public:
	SimMibTable(const Oidx& o, const index_info* i, 
		    unsigned int n): MibTable(o, i, n) { }
	virtual ~SimMibTable() { }
	
	virtual int check_creation(Request*, int&); 
};

class AGENTPP_DECL SimRowStatus: public snmpRowStatus {
 public:
	SimRowStatus(const Oidx& o, mib_access a): snmpRowStatus(o, a) { }
	virtual ~SimRowStatus() { }

	virtual bool	       	transition_ok(const Vbx&);

	/**
	  * Return the maximum access rights for the managed object
	  * represented by the receiver node.
	  * 
	  * @return The maximum access (one of the following values:
	  *         NOACCESS, READONLY, READWRITE, or READCREATE)
	  */
	virtual mib_access    get_access();

	/**
	  * Clone the receiver object.
	  * 
	  * @return A pointer to the clone.
	  */
	virtual MibEntryPtr	clone();

};

/**********************************************************************
 *  
 *  class simSysUpTime
 * 
 **********************************************************************/

class AGENTPP_DECL simSysUpTime: public SimMibLeaf {

public:
	simSysUpTime();

	void		get_request(Request*, int);

	static time_t	get();
	static time_t	get_currentTime();	

private:
	static time_t	start;
};

/**********************************************************************
 *  
 *  class simSysGroup
 * 
 **********************************************************************/

class AGENTPP_DECL simSysGroup: public MibGroup {

public:

	simSysGroup();
};


#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
