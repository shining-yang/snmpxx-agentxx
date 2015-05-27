/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - proxy_forwarder.h  
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


#ifndef _proxy_forwarder_h_
#define _proxy_forwarder_h_

#include <agent_pp/agent++.h>
#include <agent_pp/List.h>
#include <agent_pp/request.h>
#include <agent_pp/snmp_request.h> 

#ifdef _SNMPv3
#ifdef _PROXY_FORWARDER

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

class MibTableRow;

/**
 * The ProxyForwarder class represents a proxy forwarder instance 
 * as defined in RFC2573. 
 *
 * @author Frank Fock
 * @version 3.4.2
 */

class AGENTPP_DECL ProxyForwarder 
{
 public:
  typedef enum { ALL, WRITE, READ, NOTIFY, INFORM } pdu_type;
 
  /**
   * Construct a proxy forwarder for a given contextEngineID and
   * PDU type.
   *
   * @param contextEngineID
   *    a contextEngineID which must differ from the agents engineID.
   * @param pduType
   *    the PDU type(s) for which the proxy forwarder will be
   *    registered.
   */    
  ProxyForwarder(const NS_SNMP OctetStr&, pdu_type);

  ~ProxyForwarder();

  /**
   * Process a request and forward it to the target(s) determined
   * by exploring the SNMP-TARGET-MIB and SNMP-PROXY-MIB. 
   *
   * @param request
   *    a pointer to a Request instance.
   * @return 
   *    TRUE if the request could be proxied successfully (i.e.,
   *    an appropriate outgoing target could be found), or FALSE
   *    otherwise.
   */
  bool		process_request(Request*);

  /**
   * Return a key value for the proxy forwarder application.
   *
   * @return
   *    a pointer to an Oidx instance whose first n-1 subidentifier
   *    represent the context engine ID and the nth subidentifier 
   *    denotes the PDU types registered for.
   */
  OidxPtr			key() { return &regKey; }



 protected:
  
  /**
   * Initialize SNMP session used to forward messages. This method
   * must be called before processRequest(..) is called the first time.
   */ 
  void		initialize();

  OidList<MibTableRow>*	get_matches(Request*);
  bool		match_target_params(Request*, const NS_SNMP OctetStr&);
  bool      		process_single(Pdux&, Request*);  
  bool	       	process_multiple(Pdux&, Request*);  
  void			check_references();
  void			transform_pdu(const Pdux&, Pdux&);


  Oidx			regKey;
  SnmpRequestV3*       	snmp;
};

#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
#endif

#endif
