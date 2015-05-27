/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - snmp_request.h  
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


#ifndef snmp_request_h_
#define snmp_request_h_

#include <snmp_pp/snmperrs.h>
#include <snmp_pp/address.h>
#include <snmp_pp/target.h>
#include <agent_pp/snmp_pp_ext.h>
#include <snmp_pp/snmp_pp.h>

#define MIN_REQUEST_ID	10000
#define DEFAULT_RETRIES 1
#define DEFAULT_TIMEOUT 900

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


class AGENTPP_DECL InformInfo {

 public:
	InformInfo(NS_SNMP CTarget&, Vbx*, int, const Oidx&);
	~InformInfo();

	NS_SNMP CTarget	   target;
	Vbx*       vbs;
	int        sz;
	Oidx       oid;
};


class AGENTPP_DECL SnmpRequest {
    friend class SnmpRequestV3;
public:
       static int process(int, const NS_SNMP UdpAddress&, Vbx*, int& sz, Vbx*, int&,
			  const NS_SNMP OctetStr&, const int=0, const int=0);
       static int process_trap(NS_SNMP SnmpTarget&, Vbx*, int sz,
			       const Oidx&, const Oidx&, bool = FALSE);

       static int get (const NS_SNMP UdpAddress&, Vbx*, int sz, int&);
       static int next(const NS_SNMP UdpAddress&, Vbx*, int sz, int&);
       static int getbulk(const NS_SNMP UdpAddress&, Vbx*, int& sz, Vbx*, int&,
			  const int, const int);
       static int set (const NS_SNMP UdpAddress&, Vbx*, int sz, int&);

       static int get (const NS_SNMP UdpAddress&, Vbx*, int sz, int&, const NS_SNMP OctetStr&);
       static int next(const NS_SNMP UdpAddress&, Vbx*, int sz, int&, const NS_SNMP OctetStr&);
       static int getbulk(const NS_SNMP UdpAddress&, Vbx*, int& sz, Vbx*, int&,
			  const NS_SNMP OctetStr&, const int, const int);
       static int gettable(const NS_SNMP UdpAddress&, Vbx*, int sz, Vbx*, int&,
			   const NS_SNMP OctetStr&, const int);
       static int set (const NS_SNMP UdpAddress&, Vbx*, int sz, int&, const NS_SNMP OctetStr&);

       static int trap(NS_SNMP SnmpTarget&, Vbx*, int sz, const Oidx&, const Oidx&);
       static void inform(NS_SNMP CTarget&, Vbx*, int sz, const Oidx&);

 protected:
       static Snmpx *get_new_snmp(int &status);
};

#ifdef _SNMPv3

/**
 * The SnmpRequestV3 class can be used to send SNMPv1/v2c/v3 messages
 * synchronously to other SNMP entities.
 *
 * @author Frank Fock
 * @version 3.4.2
 */

class AGENTPP_DECL SnmpRequestV3 {

 public:
	/**
	 * Construct a SnmpRequestV3 session. If there could not be
	 * opened any SNMP port, a error message will be printed to
	 * stdout and the program will exit.
	 * This behavior can be avoided, when the static method
	 * send(..) is used instead.
	 */
	SnmpRequestV3();
	/**
	 * Destructor
	 */
	~SnmpRequestV3();

	/**
	 * Static method to send a SNMP request synchronously.
	 *
	 * @param target
	 *    a UTarget instance denoting the target address for the request.
	 * @param pdu
	 *    the Pdux instance to send.
	 * @return
	 *    SNMP_ERROR_SUCCESS on success or any other SNMP error code on
	 *    failure. Additonally, if the SNMP port could not be initialized
	 *    a SNMP class error will be returned.
	 */
	static int send(NS_SNMP UTarget&, Pdux&);

	/**
	 * Method to send a SNMP request synchronously.
	 *
	 * @param target
	 *    a UTarget instance denoting the target address for the request.
	 * @param pdu
	 *    the Pdux instance to send.
     * @param non_repeaters
     *    Only needed for Getbulk
     * @param repetitions
     *    Only needed for Getbulk
	 * @return
	 *    SNMP_ERROR_SUCCESS on success or any other SNMP error code on
	 */
        int send_request(NS_SNMP UTarget&, Pdux&,
                         const int non_repeaters = 0, const int repetitions = 0);

 protected:

	Snmpx* snmp;
};
#endif
#ifdef AGENTPP_NAMESPACE
}
#endif
#endif
