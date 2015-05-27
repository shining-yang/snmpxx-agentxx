/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - mib_proxy.h  
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

#ifndef mib_proxy_h_
#define mib_proxy_h_

#include <agent_pp/agent++.h>
#include <agent_pp/mib.h>
#ifdef _SNMPv3
#include <agent_pp/snmp_target_mib.h>
#include <agent_pp/snmp_proxy_mib.h>
#endif

#ifndef _PROXY_FORWARDER

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

/*--------------------------- class MibProxy -----------------------------*/

/**
 * The MibProxy class represents a sub-tree within an agents MIB whose
 * nodes and their values are proxied (forwarded) from another 
 * SNMPv1/v2c agent. The following figure displays a possible scenario:
 * <pre>
 *    +--------------+        +----------+       +-----------+
 *    |    Manager   |        | AGENT++  |       | Any v1 or |
 *    |              |--------| MibProxy |-------| v2c agent |
 *    | v1/v2c or v3 |        |          |       |           |
 *    +--------------+        +----------+       +-----------+
 * </pre>
 * SNMP v3 support for MibProxy can be easily added by sub-classing.
 *
 * @author Frank Fock
 * @version 3.02
 */

class AGENTPP_DECL MibProxy: public MibEntry {

public:
	/** 
	 * Default constructor
	 */
	MibProxy();

	/**
	 * Copy constructor
	 *
	 * @param other
	 *    another MibProxy instance
	 */
	MibProxy(const MibProxy& other);

	/**
	 * Simple proxy registration
	 *
	 * @param root
	 *    the oid of the root of the proxied MIB portion
	 * @param access
	 *    the max-access for the whole sub-tree
	 * @param source
	 *    the UDP address of the SNMP agent to whom requests for oids
	 *    within this MibProxy sub-tree should be redirected
	 */  
	MibProxy(const Oidx&, mib_access, const NS_SNMP UdpAddress&);

	/**
	 * Proxy registration with oid translation
	 *
	 * @param root
	 *    the oid of the root of the proxied MIB portion
	 * @param access
	 *    the max-access for the whole sub-tree
	 * @param translated_root
	 *    root will be replaced by translated_root for requests
	 *    sent out to the remote agent (for example if root="1.3.6.1.3"
	 *    and translated_root="1.3.6.1.2" all oids of mgmt of the 
	 *    remote agent would become experimental objects of the proxy
	 *    agent
	 * @param source
	 *    the UDP address of the SNMP agent to whom requests for oids
	 *    within this MibProxy sub-tree should be redirected
	 */   
	MibProxy(const Oidx&, mib_access, const Oidx&, const NS_SNMP UdpAddress&);

	/**
	 * Destructor
	 */
	virtual ~MibProxy() { }

	/**
	 * Return type of the MIB entry
	 *
	 * @return
	 *    AGENTPP_PROXY
	 */
	virtual mib_type	type() const  { return AGENTPP_PROXY; }

	/**
	 * Clone 
	 *
	 * @return
	 *    a pointer to a clone of the receiver
	 */
	virtual MibEntry*	clone() { return new MibProxy(*this); }

	/**
	 * Return the immediate successor of the greatest object identifier 
	 * within the receiver's scope
	 * 
	 * @return 
	 *    a pointer to the non including upper bound of the receiver's 
	 *    scope
	 */
	virtual OidxPtr		max_key();

	/**
	 * Find the successor of the given OID in the proxied MIB view
	 * and cache its value for a possibly following get_next_request 
	 * call 
	 * 
	 * @param id
	 *    an oid less or equal than max_key()
	 * @param request
	 *    a pointer to the Request instance for in whose context the
	 *    find_next is issued.
	 * @return
	 *    an oid greater than id or an empty oid (zero length) if 
	 *    an successor could not be determined or would be greater than
	 *    max_key()
	 */ 
	virtual Oidx		find_succ(const Oidx&, Request* r = 0);

	/**
	 * Check whether the reciever node contains any instance of a
	 * managed object.
	 *
	 * @return TRUE if the node currently manages an instance.
	 */  
	virtual bool		is_empty() 
				{ return ((find_succ(*key()).len() == 0) ||
					  (lastNext.get_oid() >= *max_key()));}

	virtual void		get_request(Request*, int);
	virtual int		commit_set_request(Request*, int);
	virtual int		prepare_set_request(Request*, int&);
	virtual void		get_next_request(Request*, int);

	/**
	 * Set the READ and WRITE community for outgoing requests, 
	 * respectively
	 *
	 * @param access_type
	 *    the access type associated with the community - currently
	 *    only READING and WRITING are supported
	 * @param community
	 *    the community to be used for the specified access type
	 */ 
	void			set_community(access_types a, 
					      const NS_SNMP OctetStr& c) 
					{ community[a] = c; }

protected:
	virtual Oidx		translate(const Oidx&);
	virtual Oidx		backward_translate(const Oidx&);

	void		determineDefaultRange(const Oidx&);

	NS_SNMP UdpAddress	source;
	Oidx		translation;
	bool		translating;

	NS_SNMP OctetStr	community[WRITING+1];

	Oidx		range;

	Vbx		lastNext;
	int		lastNextStatus;
};

#ifdef _SNMPv3

/*--------------------------- class MibProxyV3 -----------------------------*/

/**
 * The MibProxyV3 class represents a sub-tree within an agents MIB whose
 * nodes and their values are proxied (forwarded) from another 
 * SNMPv1/v2c agent. The following figure displays a possible scenario:
 *
 *    ----------------        --------------       ---------------
 *    |    Manager   |        | AGENT++    |       | Any v1,v2c, |
 *    |              |--------| MibProxyV3 |-------| or v3 agent |
 *    | v1/v2c or v3 |        |            |       |             |
 *    ----------------        --------------       ---------------
 *
 * TODO: Notification support.
 *
 * @author Frank Fock
 * @version 3.4.1
 */

class AGENTPP_DECL MibProxyV3: public MibEntry {

public:
	/** 
	 * Default constructor
	 */
	MibProxyV3(const Oidx&, mib_access);

	/**
	 * Copy constructor
	 *
	 * @param other
	 *    another MibProxyV3 instance
	 */
	MibProxyV3(const MibProxyV3& other);

	/**
	 * Simple proxy registration
	 *
	 * @param proxyTable
	 *    a pointer to a snmpProxyEntry object representing the
	 *    proxy information described in the SNMP-PROXY-MIB
	 * @param root
	 *    the oid of the root of the proxied MIB portion
	 * @param access
	 *    the max-access for the whole sub-tree
	 */  
	MibProxyV3(snmpProxyEntry*, const Oidx&, mib_access);

	/**
	 * Creates a proxy forwared application for that will be able
	 * to forward messages with the given contextEngineID.
	 *
	 * @param contextEngineID
	 *    an OctetStr denoting a contextEngineID. 
	 */
	MibProxyV3(const NS_SNMP OctetStr&); 

	/**
	 * Destructor
	 */
	virtual ~MibProxyV3() { }

	/**
	 * Return type of the MIB entry
	 *
	 * @return
	 *    AGENTPP_PROXY
	 */
	virtual mib_type	type() const  { return AGENTPP_PROXY; }

	/**
	 * Clone 
	 *
	 * @return
	 *    a pointer to a clone of the receiver
	 */
	virtual MibEntry*	clone() { return new MibProxyV3(*this); }

	/**
	 * Return the immediate successor of the greatest object identifier 
	 * within the receiver's scope
	 * 
	 * @return 
	 *    a pointer to the non including upper bound of the receiver's 
	 *    scope
	 */
	virtual OidxPtr		max_key();

	/**
	 * Find the successor of the given OID in the proxied MIB view
	 * and cache its value for a possibly following get_next_request 
	 * call 
	 * 
	 * @param id
	 *    an oid less or equal than max_key()
	 * @param request
	 *    a pointer to the Request instance for in whose context the
	 *    find_next is issued.
	 * @return
	 *    an oid greater than id or an empty oid (zero length) if 
	 *    an successor could not be determined or would be greater than
	 *    max_key()
	 */ 
	virtual Oidx		find_succ(const Oidx&, Request* r = 0);

	virtual void		get_request(Request*, int);
	virtual int		commit_set_request(Request*, int);
	virtual int		prepare_set_request(Request*, int&);
	virtual void		get_next_request(Request*, int);


protected:

	OidList<MibTableRow>*	get_matches(Request*);
	bool			match_target_params(Request*, const NS_SNMP OctetStr&);
	int			process_single(Pdux&, Request*);

	void			check_references();
	void			determineDefaultRange(const Oidx&);

	snmpProxyEntry*		myProxyInfo;
	
	Oidx		range;

	Vbx		lastNext;
	int		lastNextStatus;
};

#ifdef AGENTPP_NAMESPACE
}
#endif

#endif

#endif
#endif
