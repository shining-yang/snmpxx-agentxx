/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - atm_mib.h  
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


#ifndef _atm_mib_h
#define _atm_mib_h


#include <agent_pp/mib.h>

#include <agent_pp/snmp_textual_conventions.h>
#include <agent_pp/notification_originator.h>
#include <snmp_pp/log.h>

#include <agent_pp/sim_mib.h>


#define oidAtmInterfaceConfTable         "1.3.6.1.2.1.37.1.2"
#define oidAtmInterfaceConfEntry         "1.3.6.1.2.1.37.1.2.1"
#define oidAtmInterfaceMaxVpcs           "1.3.6.1.2.1.37.1.2.1.1"
#define colAtmInterfaceMaxVpcs           "1"
#define oidAtmInterfaceMaxVccs           "1.3.6.1.2.1.37.1.2.1.2"
#define colAtmInterfaceMaxVccs           "2"
#define oidAtmInterfaceConfVpcs          "1.3.6.1.2.1.37.1.2.1.3"
#define colAtmInterfaceConfVpcs          "3"
#define oidAtmInterfaceConfVccs          "1.3.6.1.2.1.37.1.2.1.4"
#define colAtmInterfaceConfVccs          "4"
#define oidAtmInterfaceMaxActiveVpiBits  "1.3.6.1.2.1.37.1.2.1.5"
#define colAtmInterfaceMaxActiveVpiBits  "5"
#define oidAtmInterfaceMaxActiveVciBits  "1.3.6.1.2.1.37.1.2.1.6"
#define colAtmInterfaceMaxActiveVciBits  "6"
#define oidAtmInterfaceIlmiVpi           "1.3.6.1.2.1.37.1.2.1.7"
#define colAtmInterfaceIlmiVpi           "7"
#define oidAtmInterfaceIlmiVci           "1.3.6.1.2.1.37.1.2.1.8"
#define colAtmInterfaceIlmiVci           "8"
#define oidAtmInterfaceAddressType       "1.3.6.1.2.1.37.1.2.1.9"
#define colAtmInterfaceAddressType       "9"
#define oidAtmInterfaceAdminAddress      "1.3.6.1.2.1.37.1.2.1.10"
#define colAtmInterfaceAdminAddress      "10"
#define oidAtmInterfaceMyNeighborIpAddress "1.3.6.1.2.1.37.1.2.1.11"
#define colAtmInterfaceMyNeighborIpAddress "11"
#define oidAtmInterfaceMyNeighborIfName  "1.3.6.1.2.1.37.1.2.1.12"
#define colAtmInterfaceMyNeighborIfName  "12"
#define oidAtmInterfaceCurrentMaxVpiBits "1.3.6.1.2.1.37.1.2.1.13"
#define colAtmInterfaceCurrentMaxVpiBits "13"
#define oidAtmInterfaceCurrentMaxVciBits "1.3.6.1.2.1.37.1.2.1.14"
#define colAtmInterfaceCurrentMaxVciBits "14"
#define oidAtmInterfaceSubscrAddress     "1.3.6.1.2.1.37.1.2.1.15"
#define colAtmInterfaceSubscrAddress     "15"
#define oidAtmInterfaceDs3PlcpTable      "1.3.6.1.2.1.37.1.3"
#define oidAtmInterfaceDs3PlcpEntry      "1.3.6.1.2.1.37.1.3.1"
#define oidAtmInterfaceDs3PlcpSEFSs      "1.3.6.1.2.1.37.1.3.1.1"
#define colAtmInterfaceDs3PlcpSEFSs      "1"
#define oidAtmInterfaceDs3PlcpAlarmState "1.3.6.1.2.1.37.1.3.1.2"
#define colAtmInterfaceDs3PlcpAlarmState "2"
#define oidAtmInterfaceDs3PlcpUASs       "1.3.6.1.2.1.37.1.3.1.3"
#define colAtmInterfaceDs3PlcpUASs       "3"
#define oidAtmInterfaceTCTable           "1.3.6.1.2.1.37.1.4"
#define oidAtmInterfaceTCEntry           "1.3.6.1.2.1.37.1.4.1"
#define oidAtmInterfaceOCDEvents         "1.3.6.1.2.1.37.1.4.1.1"
#define colAtmInterfaceOCDEvents         "1"
#define oidAtmInterfaceTCAlarmState      "1.3.6.1.2.1.37.1.4.1.2"
#define colAtmInterfaceTCAlarmState      "2"
#define oidAtmTrafficDescrParamTable     "1.3.6.1.2.1.37.1.5"
#define oidAtmTrafficDescrParamEntry     "1.3.6.1.2.1.37.1.5.1"
#define oidAtmTrafficDescrParamIndex     "1.3.6.1.2.1.37.1.5.1.1"
#define colAtmTrafficDescrParamIndex     "1"
#define oidAtmTrafficDescrType           "1.3.6.1.2.1.37.1.5.1.2"
#define colAtmTrafficDescrType           "2"
#define oidAtmTrafficDescrParam1         "1.3.6.1.2.1.37.1.5.1.3"
#define colAtmTrafficDescrParam1         "3"
#define oidAtmTrafficDescrParam2         "1.3.6.1.2.1.37.1.5.1.4"
#define colAtmTrafficDescrParam2         "4"
#define oidAtmTrafficDescrParam3         "1.3.6.1.2.1.37.1.5.1.5"
#define colAtmTrafficDescrParam3         "5"
#define oidAtmTrafficDescrParam4         "1.3.6.1.2.1.37.1.5.1.6"
#define colAtmTrafficDescrParam4         "6"
#define oidAtmTrafficDescrParam5         "1.3.6.1.2.1.37.1.5.1.7"
#define colAtmTrafficDescrParam5         "7"
#define oidAtmTrafficQoSClass            "1.3.6.1.2.1.37.1.5.1.8"
#define colAtmTrafficQoSClass            "8"
#define oidAtmTrafficDescrRowStatus      "1.3.6.1.2.1.37.1.5.1.9"
#define colAtmTrafficDescrRowStatus      "9"
#define oidAtmServiceCategory            "1.3.6.1.2.1.37.1.5.1.10"
#define colAtmServiceCategory            "10"
#define oidAtmTrafficFrameDiscard        "1.3.6.1.2.1.37.1.5.1.11"
#define colAtmTrafficFrameDiscard        "11"
#define oidAtmVplTable                   "1.3.6.1.2.1.37.1.6"
#define oidAtmVplEntry                   "1.3.6.1.2.1.37.1.6.1"
#define oidAtmVplVpi                     "1.3.6.1.2.1.37.1.6.1.1"
#define colAtmVplVpi                     "1"
#define oidAtmVplAdminStatus             "1.3.6.1.2.1.37.1.6.1.2"
#define colAtmVplAdminStatus             "2"
#define oidAtmVplOperStatus              "1.3.6.1.2.1.37.1.6.1.3"
#define colAtmVplOperStatus              "3"
#define oidAtmVplLastChange              "1.3.6.1.2.1.37.1.6.1.4"
#define colAtmVplLastChange              "4"
#define oidAtmVplReceiveTrafficDescrIndex "1.3.6.1.2.1.37.1.6.1.5"
#define colAtmVplReceiveTrafficDescrIndex "5"
#define oidAtmVplTransmitTrafficDescrIndex "1.3.6.1.2.1.37.1.6.1.6"
#define colAtmVplTransmitTrafficDescrIndex "6"
#define oidAtmVplCrossConnectIdentifier  "1.3.6.1.2.1.37.1.6.1.7"
#define colAtmVplCrossConnectIdentifier  "7"
#define oidAtmVplRowStatus               "1.3.6.1.2.1.37.1.6.1.8"
#define colAtmVplRowStatus               "8"
#define oidAtmVplCastType                "1.3.6.1.2.1.37.1.6.1.9"
#define colAtmVplCastType                "9"
#define oidAtmVplConnKind                "1.3.6.1.2.1.37.1.6.1.10"
#define colAtmVplConnKind                "10"
#define oidAtmVclTable                   "1.3.6.1.2.1.37.1.7"
#define oidAtmVclEntry                   "1.3.6.1.2.1.37.1.7.1"
#define oidAtmVclVpi                     "1.3.6.1.2.1.37.1.7.1.1"
#define colAtmVclVpi                     "1"
#define oidAtmVclVci                     "1.3.6.1.2.1.37.1.7.1.2"
#define colAtmVclVci                     "2"
#define oidAtmVclAdminStatus             "1.3.6.1.2.1.37.1.7.1.3"
#define colAtmVclAdminStatus             "3"
#define oidAtmVclOperStatus              "1.3.6.1.2.1.37.1.7.1.4"
#define colAtmVclOperStatus              "4"
#define oidAtmVclLastChange              "1.3.6.1.2.1.37.1.7.1.5"
#define colAtmVclLastChange              "5"
#define oidAtmVclReceiveTrafficDescrIndex "1.3.6.1.2.1.37.1.7.1.6"
#define colAtmVclReceiveTrafficDescrIndex "6"
#define oidAtmVclTransmitTrafficDescrIndex "1.3.6.1.2.1.37.1.7.1.7"
#define colAtmVclTransmitTrafficDescrIndex "7"
#define oidAtmVccAalType                 "1.3.6.1.2.1.37.1.7.1.8"
#define colAtmVccAalType                 "8"
#define oidAtmVccAal5CpcsTransmitSduSize "1.3.6.1.2.1.37.1.7.1.9"
#define colAtmVccAal5CpcsTransmitSduSize "9"
#define oidAtmVccAal5CpcsReceiveSduSize  "1.3.6.1.2.1.37.1.7.1.10"
#define colAtmVccAal5CpcsReceiveSduSize  "10"
#define oidAtmVccAal5EncapsType          "1.3.6.1.2.1.37.1.7.1.11"
#define colAtmVccAal5EncapsType          "11"
#define oidAtmVclCrossConnectIdentifier  "1.3.6.1.2.1.37.1.7.1.12"
#define colAtmVclCrossConnectIdentifier  "12"
#define oidAtmVclRowStatus               "1.3.6.1.2.1.37.1.7.1.13"
#define colAtmVclRowStatus               "13"
#define oidAtmVclCastType                "1.3.6.1.2.1.37.1.7.1.14"
#define colAtmVclCastType                "14"
#define oidAtmVclConnKind                "1.3.6.1.2.1.37.1.7.1.15"
#define colAtmVclConnKind                "15"
#define oidAtmVpCrossConnectIndexNext    "1.3.6.1.2.1.37.1.8.0"
#define oidAtmVpCrossConnectTable        "1.3.6.1.2.1.37.1.9"
#define oidAtmVpCrossConnectEntry        "1.3.6.1.2.1.37.1.9.1"
#define oidAtmVpCrossConnectIndex        "1.3.6.1.2.1.37.1.9.1.1"
#define colAtmVpCrossConnectIndex        "1"
#define oidAtmVpCrossConnectLowIfIndex   "1.3.6.1.2.1.37.1.9.1.2"
#define colAtmVpCrossConnectLowIfIndex   "2"
#define oidAtmVpCrossConnectLowVpi       "1.3.6.1.2.1.37.1.9.1.3"
#define colAtmVpCrossConnectLowVpi       "3"
#define oidAtmVpCrossConnectHighIfIndex  "1.3.6.1.2.1.37.1.9.1.4"
#define colAtmVpCrossConnectHighIfIndex  "4"
#define oidAtmVpCrossConnectHighVpi      "1.3.6.1.2.1.37.1.9.1.5"
#define colAtmVpCrossConnectHighVpi      "5"
#define oidAtmVpCrossConnectAdminStatus  "1.3.6.1.2.1.37.1.9.1.6"
#define colAtmVpCrossConnectAdminStatus  "6"
#define oidAtmVpCrossConnectL2HOperStatus "1.3.6.1.2.1.37.1.9.1.7"
#define colAtmVpCrossConnectL2HOperStatus "7"
#define oidAtmVpCrossConnectH2LOperStatus "1.3.6.1.2.1.37.1.9.1.8"
#define colAtmVpCrossConnectH2LOperStatus "8"
#define oidAtmVpCrossConnectL2HLastChange "1.3.6.1.2.1.37.1.9.1.9"
#define colAtmVpCrossConnectL2HLastChange "9"
#define oidAtmVpCrossConnectH2LLastChange "1.3.6.1.2.1.37.1.9.1.10"
#define colAtmVpCrossConnectH2LLastChange "10"
#define oidAtmVpCrossConnectRowStatus    "1.3.6.1.2.1.37.1.9.1.11"
#define colAtmVpCrossConnectRowStatus    "11"
#define oidAtmVcCrossConnectIndexNext    "1.3.6.1.2.1.37.1.10.0"
#define oidAtmVcCrossConnectTable        "1.3.6.1.2.1.37.1.11"
#define oidAtmVcCrossConnectEntry        "1.3.6.1.2.1.37.1.11.1"
#define oidAtmVcCrossConnectIndex        "1.3.6.1.2.1.37.1.11.1.1"
#define colAtmVcCrossConnectIndex        "1"
#define oidAtmVcCrossConnectLowIfIndex   "1.3.6.1.2.1.37.1.11.1.2"
#define colAtmVcCrossConnectLowIfIndex   "2"
#define oidAtmVcCrossConnectLowVpi       "1.3.6.1.2.1.37.1.11.1.3"
#define colAtmVcCrossConnectLowVpi       "3"
#define oidAtmVcCrossConnectLowVci       "1.3.6.1.2.1.37.1.11.1.4"
#define colAtmVcCrossConnectLowVci       "4"
#define oidAtmVcCrossConnectHighIfIndex  "1.3.6.1.2.1.37.1.11.1.5"
#define colAtmVcCrossConnectHighIfIndex  "5"
#define oidAtmVcCrossConnectHighVpi      "1.3.6.1.2.1.37.1.11.1.6"
#define colAtmVcCrossConnectHighVpi      "6"
#define oidAtmVcCrossConnectHighVci      "1.3.6.1.2.1.37.1.11.1.7"
#define colAtmVcCrossConnectHighVci      "7"
#define oidAtmVcCrossConnectAdminStatus  "1.3.6.1.2.1.37.1.11.1.8"
#define colAtmVcCrossConnectAdminStatus  "8"
#define oidAtmVcCrossConnectL2HOperStatus "1.3.6.1.2.1.37.1.11.1.9"
#define colAtmVcCrossConnectL2HOperStatus "9"
#define oidAtmVcCrossConnectH2LOperStatus "1.3.6.1.2.1.37.1.11.1.10"
#define colAtmVcCrossConnectH2LOperStatus "10"
#define oidAtmVcCrossConnectL2HLastChange "1.3.6.1.2.1.37.1.11.1.11"
#define colAtmVcCrossConnectL2HLastChange "11"
#define oidAtmVcCrossConnectH2LLastChange "1.3.6.1.2.1.37.1.11.1.12"
#define colAtmVcCrossConnectH2LLastChange "12"
#define oidAtmVcCrossConnectRowStatus    "1.3.6.1.2.1.37.1.11.1.13"
#define colAtmVcCrossConnectRowStatus    "13"
#define oidAal5VccTable                  "1.3.6.1.2.1.37.1.12"
#define oidAal5VccEntry                  "1.3.6.1.2.1.37.1.12.1"
#define oidAal5VccVpi                    "1.3.6.1.2.1.37.1.12.1.1"
#define colAal5VccVpi                    "1"
#define oidAal5VccVci                    "1.3.6.1.2.1.37.1.12.1.2"
#define colAal5VccVci                    "2"
#define oidAal5VccCrcErrors              "1.3.6.1.2.1.37.1.12.1.3"
#define colAal5VccCrcErrors              "3"
#define oidAal5VccSarTimeOuts            "1.3.6.1.2.1.37.1.12.1.4"
#define colAal5VccSarTimeOuts            "4"
#define oidAal5VccOverSizedSDUs          "1.3.6.1.2.1.37.1.12.1.5"
#define colAal5VccOverSizedSDUs          "5"
#define oidAtmTrafficDescrParamIndexNext "1.3.6.1.2.1.37.1.13.0"


#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


/**
 *  atmInterfaceMaxVpcs
 *
"The maximum number of VPCs (PVPCs and SVPCs)
 supported at this ATM interface. At the ATM UNI,
 the maximum number of VPCs (PVPCs and SVPCs)
 ranges from 0 to 256 only."
 */


class atmInterfaceMaxVpcs: public SimMibLeaf {

public:
	atmInterfaceMaxVpcs(const Oidx&);
	virtual ~atmInterfaceMaxVpcs();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmInterfaceMaxVccs
 *
"The maximum number of VCCs (PVCCs and SVCCs)
 supported at this ATM interface."
 */


class atmInterfaceMaxVccs: public SimMibLeaf {

public:
	atmInterfaceMaxVccs(const Oidx&);
	virtual ~atmInterfaceMaxVccs();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmInterfaceConfVpcs
 *
"The number of VPCs (PVPC, Soft PVPC and SVPC)
 currently in use at this ATM interface. It includes
 the number of PVPCs and Soft PVPCs that are configured
 at the interface, plus the number of SVPCs
 that are currently established at the
 interface.

 At the ATM UNI, the configured number of
 VPCs (PVPCs and SVPCs) can range from
 0 to 256 only."
 */


class atmInterfaceConfVpcs: public SimMibLeaf {

public:
	atmInterfaceConfVpcs(const Oidx&);
	virtual ~atmInterfaceConfVpcs();

	virtual MibEntryPtr	clone();
};


/**
 *  atmInterfaceConfVccs
 *
"The number of VCCs (PVCC, Soft PVCC and SVCC)
 currently in use at this ATM interface. It includes
 the number of PVCCs and Soft PVCCs that are configured
 at the interface, plus the number of SVCCs
 that are currently established at the
 interface."
 */


class atmInterfaceConfVccs: public SimMibLeaf {

public:
	atmInterfaceConfVccs(const Oidx&);
	virtual ~atmInterfaceConfVccs();

	virtual MibEntryPtr	clone();
};


/**
 *  atmInterfaceMaxActiveVpiBits
 *
"The maximum number of active VPI bits
 configured for use at the ATM interface.
 At the ATM UNI, the maximum number of active
 VPI bits configured for use ranges from
 0 to 8 only."
 */


class atmInterfaceMaxActiveVpiBits: public SimMibLeaf {

public:
	atmInterfaceMaxActiveVpiBits(const Oidx&);
	virtual ~atmInterfaceMaxActiveVpiBits();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmInterfaceMaxActiveVciBits
 *
"The maximum number of active VCI bits
 configured for use at this ATM interface."
 */


class atmInterfaceMaxActiveVciBits: public SimMibLeaf {

public:
	atmInterfaceMaxActiveVciBits(const Oidx&);
	virtual ~atmInterfaceMaxActiveVciBits();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmInterfaceIlmiVpi
 *
"The VPI value of the VCC supporting
 the ILMI at this ATM interface. If the values of
 atmInterfaceIlmiVpi and atmInterfaceIlmiVci are
 both equal to zero then the ILMI is not
 supported at this ATM interface."
 */


class atmInterfaceIlmiVpi: public SimMibLeaf {

public:
	atmInterfaceIlmiVpi(const Oidx&);
	virtual ~atmInterfaceIlmiVpi();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmInterfaceIlmiVci
 *
"The VCI value of the VCC supporting
 the ILMI at this ATM interface. If the values of
 atmInterfaceIlmiVpi and atmInterfaceIlmiVci are
 both equal to zero then the ILMI is not
 supported at this ATM interface."
 */


class atmInterfaceIlmiVci: public SimMibLeaf {

public:
	atmInterfaceIlmiVci(const Oidx&);
	virtual ~atmInterfaceIlmiVci();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmInterfaceAddressType
 *
"The type of primary ATM address configured
 for use at this ATM interface."
 */


class atmInterfaceAddressType: public SimMibLeaf {

public:
	atmInterfaceAddressType(const Oidx&);
	virtual ~atmInterfaceAddressType();

	virtual MibEntryPtr	clone();
};


/**
 *  atmInterfaceAdminAddress
 *
"The primary address assigned for administrative purposes,
 for example, an address associated with the
 service provider side of a public network UNI
 (thus, the value of this address corresponds
 with the value of ifPhysAddress at the host side).
 If this interface has no assigned administrative
 address, or when the address used for
 administrative purposes is the same as that used
 for ifPhysAddress, then this is an octet string of
 zero length."
 */


class atmInterfaceAdminAddress: public SimMibLeaf {

public:
	atmInterfaceAdminAddress(const Oidx&);
	virtual ~atmInterfaceAdminAddress();

	virtual MibEntryPtr	clone();
};


/**
 *  atmInterfaceMyNeighborIpAddress
 *
"The IP address of the neighbor system connected to
 the far end of this interface, to which a Network
 Management Station can send SNMP messages, as IP
 datagrams sent to UDP port 161, in order to access
 network management information concerning the
 operation of that system. Note that the value
 of this object may be obtained in different ways,
 e.g., by manual configuration, or through ILMI
 interaction with the neighbor system."
 */


class atmInterfaceMyNeighborIpAddress: public SimMibLeaf {

public:
	atmInterfaceMyNeighborIpAddress(const Oidx&);
	virtual ~atmInterfaceMyNeighborIpAddress();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmInterfaceMyNeighborIfName
 *
"The textual name of the interface on the neighbor
 system on the far end of this interface, and to
 which this interface connects. If the neighbor
 system is manageable through SNMP and supports
 the object ifName, the value of this object must
 be identical with that of ifName for the ifEntry
 of the lowest level physical interface
 for this port. If this interface does not have a
 textual name, the value of this object is a zero
 length string. Note that the value of this object
 may be obtained in different ways, e.g., by manual
 configuration, or through ILMI interaction with
 the neighbor system."
 */


class atmInterfaceMyNeighborIfName: public SimMibLeaf {

public:
	atmInterfaceMyNeighborIfName(const Oidx&);
	virtual ~atmInterfaceMyNeighborIfName();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmInterfaceCurrentMaxVpiBits
 *
"The maximum number of VPI Bits that may
 currently be used at this ATM interface.
 The value is the minimum of
 atmInterfaceMaxActiveVpiBits, and the
 atmInterfaceMaxActiveVpiBits of the interface's
 UNI/NNI peer.

 If the interface does not negotiate with
 its peer to determine the number of VPI Bits
 that can be used on the interface, then the
 value of this object must equal
 atmInterfaceMaxActiveVpiBits."
 */


class atmInterfaceCurrentMaxVpiBits: public SimMibLeaf {

public:
	atmInterfaceCurrentMaxVpiBits(const Oidx&);
	virtual ~atmInterfaceCurrentMaxVpiBits();

	virtual MibEntryPtr	clone();
};


/**
 *  atmInterfaceCurrentMaxVciBits
 *
"The maximum number of VCI Bits that may
 currently be used at this ATM interface.
 The value is the minimum of
 atmInterfaceMaxActiveVciBits, and the
 atmInterfaceMaxActiveVciBits of the interface's
 UNI/NNI peer.

 If the interface does not negotiate with
 its peer to determine the number of VCI Bits
 that can be used on the interface, then the
 value of this object must equal
 atmInterfaceMaxActiveVciBits."
 */


class atmInterfaceCurrentMaxVciBits: public SimMibLeaf {

public:
	atmInterfaceCurrentMaxVciBits(const Oidx&);
	virtual ~atmInterfaceCurrentMaxVciBits();

	virtual MibEntryPtr	clone();
};


/**
 *  atmInterfaceSubscrAddress
 *
"The identifier assigned by a service provider
 to the network side of a public network UNI.
 If this interface has no assigned service provider
 address, or for other interfaces this is an octet string
 of zero length."
 */


class atmInterfaceSubscrAddress: public SimMibLeaf {

public:
	atmInterfaceSubscrAddress(const Oidx&);
	virtual ~atmInterfaceSubscrAddress();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmInterfaceDs3PlcpSEFSs
 *
"The number of DS3 PLCP Severely Errored Framing
 Seconds (SEFS). Each SEFS represents a
 one-second interval which contains
 one or more SEF events."
 */


class atmInterfaceDs3PlcpSEFSs: public SimMibLeaf {

public:
	atmInterfaceDs3PlcpSEFSs(const Oidx&);
	virtual ~atmInterfaceDs3PlcpSEFSs();

	virtual MibEntryPtr	clone();
};


/**
 *  atmInterfaceDs3PlcpAlarmState
 *
"This variable indicates if there is an
 alarm present for the DS3 PLCP. The value
 receivedFarEndAlarm means that the DS3 PLCP
 has received an incoming Yellow
 Signal, the value incomingLOF means that
 the DS3 PLCP has declared a loss of frame (LOF)
 failure condition, and the value noAlarm
 means that there are no alarms present.
 Transition from the failure to the no alarm state
 occurs when no defects (e.g., LOF) are received
 for more than 10 seconds."
 */


class atmInterfaceDs3PlcpAlarmState: public SimMibLeaf {

public:
	atmInterfaceDs3PlcpAlarmState(const Oidx&);
	virtual ~atmInterfaceDs3PlcpAlarmState();

	virtual MibEntryPtr	clone();
};


/**
 *  atmInterfaceDs3PlcpUASs
 *
"The counter associated with the number of
 Unavailable Seconds encountered by the PLCP."
 */


class atmInterfaceDs3PlcpUASs: public SimMibLeaf {

public:
	atmInterfaceDs3PlcpUASs(const Oidx&);
	virtual ~atmInterfaceDs3PlcpUASs();

	virtual MibEntryPtr	clone();
};


/**
 *  atmInterfaceOCDEvents
 *
"The number of times the Out of Cell
 Delineation (OCD) events occur. If seven
 consecutive ATM cells have Header Error
 Control (HEC) violations, an OCD event occurs.
 A high number of OCD events may indicate a
 problem with the TC Sublayer."
 */


class atmInterfaceOCDEvents: public SimMibLeaf {

public:
	atmInterfaceOCDEvents(const Oidx&);
	virtual ~atmInterfaceOCDEvents();

	virtual MibEntryPtr	clone();
};


/**
 *  atmInterfaceTCAlarmState
 *
"This variable indicates if there is an
 alarm present for the TC Sublayer. The value
 lcdFailure(2) indicates that the TC Sublayer
 is currently in the Loss of Cell Delineation
 (LCD) defect maintenance state. The value
 noAlarm(1) indicates that the TC Sublayer
 is currently not in the LCD defect
 maintenance state."
 */


class atmInterfaceTCAlarmState: public SimMibLeaf {

public:
	atmInterfaceTCAlarmState(const Oidx&);
	virtual ~atmInterfaceTCAlarmState();

	virtual MibEntryPtr	clone();
};


/**
 *  atmTrafficDescrType
 *
"The value of this object identifies the type
 of ATM traffic descriptor.
 The type may indicate no traffic descriptor or
 traffic descriptor with one or more parameters.
 These parameters are specified as a parameter
 vector, in the corresponding instances of the
 objects:
 atmTrafficDescrParam1
 atmTrafficDescrParam2
 atmTrafficDescrParam3
 atmTrafficDescrParam4
 atmTrafficDescrParam5."
 */


class atmTrafficDescrType: public SimMibLeaf {

public:
	atmTrafficDescrType(const Oidx&);
	virtual ~atmTrafficDescrType();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmTrafficDescrParam1
 *
"The first parameter of the ATM traffic descriptor
 used according to the value of
 atmTrafficDescrType."
 */


class atmTrafficDescrParam1: public SimMibLeaf {

public:
	atmTrafficDescrParam1(const Oidx&);
	virtual ~atmTrafficDescrParam1();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmTrafficDescrParam2
 *
"The second parameter of the ATM traffic descriptor
 used according to the value of
 atmTrafficDescrType."
 */


class atmTrafficDescrParam2: public SimMibLeaf {

public:
	atmTrafficDescrParam2(const Oidx&);
	virtual ~atmTrafficDescrParam2();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmTrafficDescrParam3
 *
"The third parameter of the ATM traffic descriptor
 used according to the value of
 atmTrafficDescrType."
 */


class atmTrafficDescrParam3: public SimMibLeaf {

public:
	atmTrafficDescrParam3(const Oidx&);
	virtual ~atmTrafficDescrParam3();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmTrafficDescrParam4
 *
"The fourth parameter of the ATM traffic descriptor
 used according to the value of
 atmTrafficDescrType."
 */


class atmTrafficDescrParam4: public SimMibLeaf {

public:
	atmTrafficDescrParam4(const Oidx&);
	virtual ~atmTrafficDescrParam4();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmTrafficDescrParam5
 *
"The fifth parameter of the ATM traffic descriptor
 used according to the value of
 atmTrafficDescrType."
 */


class atmTrafficDescrParam5: public SimMibLeaf {

public:
	atmTrafficDescrParam5(const Oidx&);
	virtual ~atmTrafficDescrParam5();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmTrafficQoSClass
 *
"The value of this object identifies the QoS Class.
 Four Service classes have been
 specified in the ATM Forum UNI Specification:
 Service Class A: Constant bit rate video and
 Circuit emulation
 Service Class B: Variable bit rate video/audio
 Service Class C: Connection-oriented data
 Service Class D: Connectionless data
 Four QoS classes numbered 1, 2, 3, and 4 have
 been specified with the aim to support service
 classes A, B, C, and D respectively.
 An unspecified QoS Class numbered `0' is used
 for best effort traffic."
 */


class atmTrafficQoSClass: public SimMibLeaf {

public:
	atmTrafficQoSClass(const Oidx&);
	virtual ~atmTrafficQoSClass();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmTrafficDescrRowStatus
 *
"This object is used to create
 a new row or modify or delete an
 existing row in this table."
 */


class atmTrafficDescrRowStatus: public snmpRowStatus {

public:
	atmTrafficDescrRowStatus(const Oidx&);
	virtual ~atmTrafficDescrRowStatus();

	virtual MibEntryPtr	clone();
};


/**
 *  atmServiceCategory
 *
"The ATM service category."
 */


class atmServiceCategory: public SimMibLeaf {

public:
	atmServiceCategory(const Oidx&);
	virtual ~atmServiceCategory();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmTrafficFrameDiscard
 *
"If set to 'true', this object indicates that the network
 is requested to treat data for this connection, in the
 given direction, as frames (e.g. AAL5 CPCS_PDU's) rather
 than as individual cells. While the precise
 implementation is network-specific, this treatment may
 for example involve discarding entire frames during
 congestion, rather than a few cells from many frames."
 */


class atmTrafficFrameDiscard: public SimMibLeaf {

public:
	atmTrafficFrameDiscard(const Oidx&);
	virtual ~atmTrafficFrameDiscard();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmVplAdminStatus
 *
"This object is instanciated only for a VPL
 which terminates a VPC (i.e., one which is
 NOT cross-connected to other VPLs).
 Its value specifies the desired
 administrative state of the VPL."
 */


class atmVplAdminStatus: public SimMibLeaf {

public:
	atmVplAdminStatus(const Oidx&);
	virtual ~atmVplAdminStatus();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmVplOperStatus
 *
"The current operational status of the VPL."
 */


class atmVplOperStatus: public SimMibLeaf {

public:
	atmVplOperStatus(const Oidx&);
	virtual ~atmVplOperStatus();

	virtual MibEntryPtr	clone();
};


/**
 *  atmVplLastChange
 *
"The value of sysUpTime at the time this
 VPL entered its current operational state."
 */


class atmVplLastChange: public SimMibLeaf {

public:
	atmVplLastChange(const Oidx&);
	virtual ~atmVplLastChange();

	virtual MibEntryPtr	clone();
};


/**
 *  atmVplReceiveTrafficDescrIndex
 *
"The value of this object identifies the row
 in the atmTrafficDescrParamTable which
 applies to the receive direction of the VPL."
 */


class atmVplReceiveTrafficDescrIndex: public SimMibLeaf {

public:
	atmVplReceiveTrafficDescrIndex(const Oidx&);
	virtual ~atmVplReceiveTrafficDescrIndex();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmVplTransmitTrafficDescrIndex
 *
"The value of this object identifies the row
 in the atmTrafficDescrParamTable which
 applies to the transmit direction of the VPL."
 */


class atmVplTransmitTrafficDescrIndex: public SimMibLeaf {

public:
	atmVplTransmitTrafficDescrIndex(const Oidx&);
	virtual ~atmVplTransmitTrafficDescrIndex();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmVplCrossConnectIdentifier
 *
"This object is instantiated only for a VPL
 which is cross-connected to other VPLs
 that belong to the same VPC. All such
 associated VPLs have the same value of this
 object, and all their cross-connections are
 identified either by entries that are indexed
 by the same value of atmVpCrossConnectIndex in
 the atmVpCrossConnectTable of this MIB module or by
 the same value of the cross-connect index in
 the cross-connect table for SVCs and Soft PVCs
 (defined in a separate MIB module).
 At no time should entries in these respective
 cross-connect tables exist simultaneously
 with the same cross-connect index value.
 The value of this object is initialized by the
 agent after the associated entries in the
 atmVpCrossConnectTable have been created."
 */


class atmVplCrossConnectIdentifier: public SimMibLeaf {

public:
	atmVplCrossConnectIdentifier(const Oidx&);
	virtual ~atmVplCrossConnectIdentifier();

	virtual MibEntryPtr	clone();
};


/**
 *  atmVplRowStatus
 *
"This object is used to create, delete
 or modify a row in this table.
 To create a new VCL, this object is
 initially set to 'createAndWait' or
 'createAndGo'. This object should not be
 set to 'active' unless the following columnar
 objects have been set to their desired value
 in this row:
 atmVplReceiveTrafficDescrIndex and
 atmVplTransmitTrafficDescrIndex.
 The DESCRIPTION of atmVplEntry provides
 further guidance to row treatment in this table."
 */


class atmVplRowStatus: public snmpRowStatus {

public:
	atmVplRowStatus(const Oidx&);
	virtual ~atmVplRowStatus();

	virtual MibEntryPtr	clone();
};


/**
 *  atmVplCastType
 *
"The connection topology type."
 */


class atmVplCastType: public SimMibLeaf {

public:
	atmVplCastType(const Oidx&);
	virtual ~atmVplCastType();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmVplConnKind
 *
"The use of call control."
 */


class atmVplConnKind: public SimMibLeaf {

public:
	atmVplConnKind(const Oidx&);
	virtual ~atmVplConnKind();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmVclAdminStatus
 *
"This object is instanciated only for a VCL which
 terminates a VCC (i.e., one which is NOT
 cross-connected to other VCLs). Its value
 specifies the desired administrative state of
 the VCL."
 */


class atmVclAdminStatus: public SimMibLeaf {

public:
	atmVclAdminStatus(const Oidx&);
	virtual ~atmVclAdminStatus();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmVclOperStatus
 *
"The current operational status of the VCL."
 */


class atmVclOperStatus: public SimMibLeaf {

public:
	atmVclOperStatus(const Oidx&);
	virtual ~atmVclOperStatus();

	virtual MibEntryPtr	clone();
};


/**
 *  atmVclLastChange
 *
"The value of sysUpTime at the time this VCL
 entered its current operational state."
 */


class atmVclLastChange: public SimMibLeaf {

public:
	atmVclLastChange(const Oidx&);
	virtual ~atmVclLastChange();

	virtual MibEntryPtr	clone();
};


/**
 *  atmVclReceiveTrafficDescrIndex
 *
"The value of this object identifies the row
 in the ATM Traffic Descriptor Table which
 applies to the receive direction of this VCL."
 */


class atmVclReceiveTrafficDescrIndex: public SimMibLeaf {

public:
	atmVclReceiveTrafficDescrIndex(const Oidx&);
	virtual ~atmVclReceiveTrafficDescrIndex();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmVclTransmitTrafficDescrIndex
 *
"The value of this object identifies the row
 of the ATM Traffic Descriptor Table which applies
 to the transmit direction of this VCL."
 */


class atmVclTransmitTrafficDescrIndex: public SimMibLeaf {

public:
	atmVclTransmitTrafficDescrIndex(const Oidx&);
	virtual ~atmVclTransmitTrafficDescrIndex();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmVccAalType
 *
"An instance of this object only exists when the
 local VCL end-point is also the VCC end-point,
 and AAL is in use.
 The type of AAL used on this VCC.
 The AAL type includes AAL1, AAL2, AAL3/4,
 and AAL5. The other(4) may be user-defined
 AAL type. The unknown type indicates that
 the AAL type cannot be determined."
 */


class atmVccAalType: public SimMibLeaf {

public:
	atmVccAalType(const Oidx&);
	virtual ~atmVccAalType();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmVccAal5CpcsTransmitSduSize
 *
"An instance of this object only exists when the
 local VCL end-point is also the VCC end-point,
 and AAL5 is in use.
 The maximum AAL5 CPCS SDU size in octets that is
 supported on the transmit direction of this VCC."
 */


class atmVccAal5CpcsTransmitSduSize: public SimMibLeaf {

public:
	atmVccAal5CpcsTransmitSduSize(const Oidx&);
	virtual ~atmVccAal5CpcsTransmitSduSize();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmVccAal5CpcsReceiveSduSize
 *
"An instance of this object only exists when the
 local VCL end-point is also the VCC end-point,
 and AAL5 is in use.
 The maximum AAL5 CPCS SDU size in octets that is
 supported on the receive direction of this VCC."
 */


class atmVccAal5CpcsReceiveSduSize: public SimMibLeaf {

public:
	atmVccAal5CpcsReceiveSduSize(const Oidx&);
	virtual ~atmVccAal5CpcsReceiveSduSize();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmVccAal5EncapsType
 *
"An instance of this object only exists when the
 local VCL end-point is also the VCC end-point,
 and AAL5 is in use.
 The type of data encapsulation used over
 the AAL5 SSCS layer. The definitions reference
 RFC 1483 Multiprotocol Encapsulation
 over ATM AAL5 and to the ATM Forum
 LAN Emulation specification."
 */


class atmVccAal5EncapsType: public SimMibLeaf {

public:
	atmVccAal5EncapsType(const Oidx&);
	virtual ~atmVccAal5EncapsType();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmVclCrossConnectIdentifier
 *
"This object is instantiated only for a VCL
 which is cross-connected to other VCLs
 that belong to the same VCC. All such
 associated VCLs have the same value of this
 object, and all their cross-connections are
 identified either by entries that are indexed
 by the same value of atmVcCrossConnectIndex in
 the atmVcCrossConnectTable of this MIB module or by
 the same value of the cross-connect index in
 the cross-connect table for SVCs and Soft PVCs
 (defined in a separate MIB module).

 At no time should entries in these respective
 cross-connect tables exist simultaneously
 with the same cross-connect index value.
 The value of this object is initialized by the
 agent after the associated entries in the
 atmVcCrossConnectTable have been created."
 */


class atmVclCrossConnectIdentifier: public SimMibLeaf {

public:
	atmVclCrossConnectIdentifier(const Oidx&);
	virtual ~atmVclCrossConnectIdentifier();

	virtual MibEntryPtr	clone();
};


/**
 *  atmVclRowStatus
 *
"This object is used to create, delete or
 modify a row in this table. To create
 a new VCL, this object is initially set
 to 'createAndWait' or 'createAndGo'.
 This object should not be
 set to 'active' unless the following columnar
 objects have been set to their desired value
 in this row:
 atmVclReceiveTrafficDescrIndex,
 atmVclTransmitTrafficDescrIndex.
 In addition, if the local VCL end-point
 is also the VCC end-point:
 atmVccAalType.
 In addition, for AAL5 connections only:
 atmVccAal5CpcsTransmitSduSize,
 atmVccAal5CpcsReceiveSduSize, and
 atmVccAal5EncapsType. (The existence
 of these objects imply the AAL connection type.).
 The DESCRIPTION of atmVclEntry provides
 further guidance to row treatment in this table."
 */


class atmVclRowStatus: public snmpRowStatus {

public:
	atmVclRowStatus(const Oidx&);
	virtual ~atmVclRowStatus();

	virtual MibEntryPtr	clone();
};


/**
 *  atmVclCastType
 *
"The connection topology type."
 */


class atmVclCastType: public SimMibLeaf {

public:
	atmVclCastType(const Oidx&);
	virtual ~atmVclCastType();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmVclConnKind
 *
"The use of call control."
 */


class atmVclConnKind: public SimMibLeaf {

public:
	atmVclConnKind(const Oidx&);
	virtual ~atmVclConnKind();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmVpCrossConnectIndexNext
 *
"This object contains an appropriate value to
 be used for atmVpCrossConnectIndex when creating
 entries in the atmVpCrossConnectTable. The value
 0 indicates that no unassigned entries are
 available. To obtain the atmVpCrossConnectIndex
 value for a new entry, the manager issues a
 management protocol retrieval operation to obtain
 the current value of this object. After each
 retrieval, the agent should modify the value to
 the next unassigned index.
 After a manager retrieves a value the agent will
 determine through its local policy when this index
 value will be made available for reuse."
 */


class atmVpCrossConnectIndexNext: public SimMibLeaf {

public:
	atmVpCrossConnectIndexNext();
	virtual ~atmVpCrossConnectIndexNext();

	static atmVpCrossConnectIndexNext* instance;

};


/**
 *  atmVpCrossConnectAdminStatus
 *
"The desired administrative status of this
 bi-directional VP cross-connect."
 */


class atmVpCrossConnectAdminStatus: public SimMibLeaf {

public:
	atmVpCrossConnectAdminStatus(const Oidx&);
	virtual ~atmVpCrossConnectAdminStatus();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmVpCrossConnectL2HOperStatus
 *
"The operational status of the VP cross-connect
 in one direction; (i.e., from the low to
 high direction)."
 */


class atmVpCrossConnectL2HOperStatus: public SimMibLeaf {

public:
	atmVpCrossConnectL2HOperStatus(const Oidx&);
	virtual ~atmVpCrossConnectL2HOperStatus();

	virtual MibEntryPtr	clone();
};


/**
 *  atmVpCrossConnectH2LOperStatus
 *
"The operational status of the VP cross-connect
 in one direction; (i.e., from the high to
 low direction)."
 */


class atmVpCrossConnectH2LOperStatus: public SimMibLeaf {

public:
	atmVpCrossConnectH2LOperStatus(const Oidx&);
	virtual ~atmVpCrossConnectH2LOperStatus();

	virtual MibEntryPtr	clone();
};


/**
 *  atmVpCrossConnectL2HLastChange
 *
"The value of sysUpTime at the time this
 VP cross-connect entered its current operational
 state in the low to high direction."
 */


class atmVpCrossConnectL2HLastChange: public SimMibLeaf {

public:
	atmVpCrossConnectL2HLastChange(const Oidx&);
	virtual ~atmVpCrossConnectL2HLastChange();

	virtual MibEntryPtr	clone();
};


/**
 *  atmVpCrossConnectH2LLastChange
 *
"The value of sysUpTime at the time this
 VP cross-connect entered its current operational
 in the high to low direction."
 */


class atmVpCrossConnectH2LLastChange: public SimMibLeaf {

public:
	atmVpCrossConnectH2LLastChange(const Oidx&);
	virtual ~atmVpCrossConnectH2LLastChange();

	virtual MibEntryPtr	clone();
};


/**
 *  atmVpCrossConnectRowStatus
 *
"The status of this entry in the
 atmVpCrossConnectTable. This object is used to
 create a cross-connect for cross-connecting
 VPLs which are created using the atmVplTable
 or to change or delete an existing cross-connect.
 This object must be initially set
 to `createAndWait' or 'createAndGo'.
 To turn on a VP cross-connect,
 the atmVpCrossConnectAdminStatus
 is set to `up'."
 */


class atmVpCrossConnectRowStatus: public snmpRowStatus {

public:
	atmVpCrossConnectRowStatus(const Oidx&);
	virtual ~atmVpCrossConnectRowStatus();

	virtual MibEntryPtr	clone();
};


/**
 *  atmVcCrossConnectIndexNext
 *
"This object contains an appropriate value to
 be used for atmVcCrossConnectIndex when creating
 entries in the atmVcCrossConnectTable. The value
 0 indicates that no unassigned entries are
 available. To obtain the atmVcCrossConnectIndex
 value for a new entry, the manager issues a
 management protocol retrieval operation to obtain
 the current value of this object. After each
 retrieval, the agent should modify the value to
 the next unassigned index.
 After a manager retrieves a value the agent will
 determine through its local policy when this index
 value will be made available for reuse."
 */


class atmVcCrossConnectIndexNext: public SimMibLeaf {

public:
	atmVcCrossConnectIndexNext();
	virtual ~atmVcCrossConnectIndexNext();

	static atmVcCrossConnectIndexNext* instance;

};


/**
 *  atmVcCrossConnectAdminStatus
 *
"The desired administrative status of this
 bi-directional VC cross-connect."
 */


class atmVcCrossConnectAdminStatus: public SimMibLeaf {

public:
	atmVcCrossConnectAdminStatus(const Oidx&);
	virtual ~atmVcCrossConnectAdminStatus();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual bool    	value_ok(const Vbx&);
};


/**
 *  atmVcCrossConnectL2HOperStatus
 *
"The current operational status of the
 VC cross-connect in one direction; (i.e.,
 from the low to high direction)."
 */


class atmVcCrossConnectL2HOperStatus: public SimMibLeaf {

public:
	atmVcCrossConnectL2HOperStatus(const Oidx&);
	virtual ~atmVcCrossConnectL2HOperStatus();

	virtual MibEntryPtr	clone();
};


/**
 *  atmVcCrossConnectH2LOperStatus
 *
"The current operational status of the
 VC cross-connect in one direction; (i.e.,
 from the high to low direction)."
 */


class atmVcCrossConnectH2LOperStatus: public SimMibLeaf {

public:
	atmVcCrossConnectH2LOperStatus(const Oidx&);
	virtual ~atmVcCrossConnectH2LOperStatus();

	virtual MibEntryPtr	clone();
};


/**
 *  atmVcCrossConnectL2HLastChange
 *
"The value of sysUpTime at the time this
 VC cross-connect entered its current
 operational state in low to high direction."
 */


class atmVcCrossConnectL2HLastChange: public SimMibLeaf {

public:
	atmVcCrossConnectL2HLastChange(const Oidx&);
	virtual ~atmVcCrossConnectL2HLastChange();

	virtual MibEntryPtr	clone();
};


/**
 *  atmVcCrossConnectH2LLastChange
 *
"The value of sysUpTime at the time this
 VC cross-connect entered its current
 operational state in high to low direction."
 */


class atmVcCrossConnectH2LLastChange: public SimMibLeaf {

public:
	atmVcCrossConnectH2LLastChange(const Oidx&);
	virtual ~atmVcCrossConnectH2LLastChange();

	virtual MibEntryPtr	clone();
};


/**
 *  atmVcCrossConnectRowStatus
 *
"The status of this entry in the
 atmVcCrossConnectTable. This object is used to
 create a new cross-connect for cross-connecting
 VCLs which are created using the atmVclTable
 or to change or delete existing cross-connect.
 This object must be initially set to
 `createAndWait' or 'createAndGo'.
 To turn on a VC cross-connect,
 the atmVcCrossConnectAdminStatus
 is set to `up'."
 */


class atmVcCrossConnectRowStatus: public snmpRowStatus {

public:
	atmVcCrossConnectRowStatus(const Oidx&);
	virtual ~atmVcCrossConnectRowStatus();

	virtual MibEntryPtr	clone();
};


/**
 *  aal5VccCrcErrors
 *
"The number of AAL5 CPCS PDUs received with
 CRC-32 errors on this AAL5 VCC at the
 interface associated with an AAL5 entity."
 */


class aal5VccCrcErrors: public SimMibLeaf {

public:
	aal5VccCrcErrors(const Oidx&);
	virtual ~aal5VccCrcErrors();

	virtual MibEntryPtr	clone();
};


/**
 *  aal5VccSarTimeOuts
 *
"The number of partially re-assembled AAL5
 CPCS PDUs which were discarded
 on this AAL5 VCC at the interface associated
 with an AAL5 entity because they
 were not fully re-assembled within the
 required time period. If the re-assembly
 timer is not supported, then this object
 contains a zero value."
 */


class aal5VccSarTimeOuts: public SimMibLeaf {

public:
	aal5VccSarTimeOuts(const Oidx&);
	virtual ~aal5VccSarTimeOuts();

	virtual MibEntryPtr	clone();
};


/**
 *  aal5VccOverSizedSDUs
 *
"The number of AAL5 CPCS PDUs discarded
 on this AAL5 VCC at the interface
 associated with an AAL5 entity because the
 AAL5 SDUs were too large."
 */


class aal5VccOverSizedSDUs: public SimMibLeaf {

public:
	aal5VccOverSizedSDUs(const Oidx&);
	virtual ~aal5VccOverSizedSDUs();

	virtual MibEntryPtr	clone();
};


/**
 *  atmTrafficDescrParamIndexNext
 *
"This object contains an appropriate value to
 be used for atmTrafficDescrParamIndex when
 creating entries in the
 atmTrafficDescrParamTable.
 The value 0 indicates that no unassigned
 entries are available. To obtain the
 atmTrafficDescrParamIndex value for a new
 entry, the manager issues a management
 protocol retrieval operation to obtain the
 current value of this object. After each
 retrieval, the agent should modify the value
 to the next unassigned index.
 After a manager retrieves a value the agent will
 determine through its local policy when this index
 value will be made available for reuse."
 */


class atmTrafficDescrParamIndexNext: public SimMibLeaf {

public:
	atmTrafficDescrParamIndexNext();
	virtual ~atmTrafficDescrParamIndexNext();

	static atmTrafficDescrParamIndexNext* instance;

};


/**
 *  atmInterfaceConfEntry
 *
"This list contains ATM interface configuration
 parameters and state variables and is indexed
 by ifIndex values of ATM interfaces."
 */


class atmInterfaceConfEntry: public MibTable {

public:
	atmInterfaceConfEntry();
	virtual ~atmInterfaceConfEntry();

	static atmInterfaceConfEntry* instance;

	virtual void       	set_row(MibTableRow* r, int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, char* p9, char* p10, char* p11, int p12, int p13, char* p14);
};


/**
 *  atmInterfaceDs3PlcpEntry
 *
"This list contains DS3 PLCP parameters and
 state variables at the ATM interface and is
 indexed by the ifIndex value of the ATM interface."
 */


class atmInterfaceDs3PlcpEntry: public MibTable {

public:
	atmInterfaceDs3PlcpEntry();
	virtual ~atmInterfaceDs3PlcpEntry();

	static atmInterfaceDs3PlcpEntry* instance;

	virtual void       	set_row(MibTableRow* r, int p0, int p1, int p2);
};


/**
 *  atmInterfaceTCEntry
 *
"This list contains TC Sublayer parameters
 and state variables at the ATM interface and is
 indexed by the ifIndex value of the ATM interface."
 */


class atmInterfaceTCEntry: public MibTable {

public:
	atmInterfaceTCEntry();
	virtual ~atmInterfaceTCEntry();

	static atmInterfaceTCEntry* instance;

	virtual void       	set_row(MibTableRow* r, int p0, int p1);
};


/**
 *  atmTrafficDescrParamEntry
 *
"This list contains ATM traffic descriptor
 type and the associated parameters."
 */


class atmTrafficDescrParamEntry: public MibTable {

public:
	atmTrafficDescrParamEntry();
	virtual ~atmTrafficDescrParamEntry();

	static atmTrafficDescrParamEntry* instance;

	virtual void       	set_row(MibTableRow* r, char* p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9);
};


/**
 *  atmVplEntry
 *
"An entry in the VPL table. This entry is
 used to model a bi-directional VPL.
 To create a VPL at an ATM interface,
 either of the following procedures are used:

 Negotiated VPL establishment

 (1) The management application creates
 a VPL entry in the atmVplTable
 by setting atmVplRowStatus to createAndWait(5).
 This may fail for the following reasons:
 - The selected VPI value is unavailable,
 - The selected VPI value is in use.
 Otherwise, the agent creates a row and
 reserves the VPI value on that port.

 (2) The manager selects an existing row(s) in the
 atmTrafficDescrParamTable,
 thereby, selecting a set of self-consistent
 ATM traffic parameters and the service category
 for receive and transmit directions of the VPL.

 (2a) If no suitable row(s) in the
 atmTrafficDescrParamTable exists,
 the manager must create a new row(s)
 in that table.

 (2b) The manager characterizes the VPL's traffic
 parameters through setting the
 atmVplReceiveTrafficDescrIndex and the
 atmVplTransmitTrafficDescrIndex values
 in the VPL table, which point to the rows
 containing desired ATM traffic parameter values
 in the atmTrafficDescrParamTable. The agent
 will check the availability of resources and
 may refuse the request.
 If the transmit and receive service categories
 are inconsistent, the agent should refuse the
 request.

 (3) The manager activates the VPL by setting the
 the atmVplRowStatus to active(1).
 If this set is successful, the agent has
 reserved the resources to satisfy the requested
 traffic parameter values and the service category
 for that VPL.

 (4) If the VPL terminates a VPC in the ATM host
 or switch, the manager turns on the
 atmVplAdminStatus to up(1) to turn the VPL
 traffic flow on. Otherwise, the
 atmVpCrossConnectTable must be used
 to cross-connect the VPL to another VPL(s)
 in an ATM switch or network.

 One-Shot VPL Establishment

 A VPL may also be established in one step by a
 set-request with all necessary VPL parameter
 values and atmVplRowStatus set to createAndGo(4).

 In contrast to the negotiated VPL establishment
 which allows for detailed error checking
 (i.e., set errors are explicitly linked to
 particular resource acquisition failures),
 the one-shot VPL establishment
 performs the setup on one operation but
 does not have the advantage of step-wise
 error checking.

 VPL Retirement

 A VPL is released by setting atmVplRowStatus to
 destroy(6), and the agent may release all
 associated resources."
 */


class atmVplEntry: public MibTable {

public:
	atmVplEntry();
	virtual ~atmVplEntry();

	static atmVplEntry* instance;

	virtual void       	set_row(MibTableRow* r, int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8);
};


/**
 *  atmVclEntry
 *
"An entry in the VCL table. This entry is
 used to model a bi-directional VCL.
 To create a VCL at an ATM interface,
 either of the following procedures are used:

 Negotiated VCL establishment

 (1) The management application creates
 a VCL entry in the atmVclTable
 by setting atmVclRowStatus to createAndWait(5).
 This may fail for the following reasons:
 - The selected VPI/VCI values are unavailable,
 - The selected VPI/VCI values are in use.
 Otherwise, the agent creates a row and
 reserves the VPI/VCI values on that port.

 (2) The manager selects an existing row(s) in the
 atmTrafficDescrParamTable,
 thereby, selecting a set of self-consistent
 ATM traffic parameters and the service category
 for receive and transmit directions of the VCL.

 (2a) If no suitable row(s) in the
 atmTrafficDescrParamTable exists,
 the manager must create a new row(s)
 in that table.

 (2b) The manager characterizes the VCL's traffic
 parameters through setting the
 atmVclReceiveTrafficDescrIndex and the
 atmVclTransmitTrafficDescrIndex values
 in the VCL table, which point to the rows
 containing desired ATM traffic parameter values
 in the atmTrafficDescrParamTable. The agent
 will check the availability of resources and
 may refuse the request.
 If the transmit and receive service categories
 are inconsistent, the agent should refuse the
 request.

 (3) The manager activates the VCL by setting the
 the atmVclRowStatus to active(1) (for
 requirements on this activation see the
 description of atmVclRowStatus).
 If this set is successful, the agent has
 reserved the resources to satisfy the requested
 traffic parameter values and the service category
 for that VCL.
 (4) If the VCL terminates a VCC in the ATM host
 or switch, the manager turns on the
 atmVclAdminStatus to up(1) to turn the VCL
 traffic flow on. Otherwise, the
 atmVcCrossConnectTable must be used
 to cross-connect the VCL to another VCL(s)
 in an ATM switch or network.

 One-Shot VCL Establishment

 A VCL may also be established in one step by a
 set-request with all necessary VCL parameter
 values and atmVclRowStatus set to createAndGo(4).

 In contrast to the negotiated VCL establishment
 which allows for detailed error checking
 (i.e., set errors are explicitly linked to
 particular resource acquisition failures),
 the one-shot VCL establishment
 performs the setup on one operation but
 does not have the advantage of step-wise
 error checking.
 VCL Retirement

 A VCL is released by setting atmVclRowStatus to
 destroy(6), and the agent may release all
 associated resources."
 */


class atmVclEntry: public MibTable {

public:
	atmVclEntry();
	virtual ~atmVclEntry();

	static atmVclEntry* instance;

	virtual void       	set_row(MibTableRow* r, int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12);
};


/**
 *  atmVpCrossConnectEntry
 *
"An entry in the ATM VP Cross Connect table.
 This entry is used to model a bi-directional
 ATM VP cross-connect which cross-connects
 two VPLs.

 Step-wise Procedures to set up a VP Cross-connect

 Once the entries in the atmVplTable are created,
 the following procedures are used
 to cross-connect the VPLs together.

 (1) The manager obtains a unique
 atmVpCrossConnectIndex by reading the
 atmVpCrossConnectIndexNext object.

 (2) Next, the manager creates a set of one
 or more rows in the ATM VP Cross Connect
 Table, one for each cross-connection between
 two VPLs. Each row is indexed by the ATM
 interface port numbers and VPI values of the
 two ends of that cross-connection.
 This set of rows specifies the topology of the
 VPC cross-connect and is identified by a single
 value of atmVpCrossConnectIndex.

 Negotiated VP Cross-Connect Establishment

 (2a) The manager creates a row in this table by
 setting atmVpCrossConnectRowStatus to
 createAndWait(5). The agent checks the
 requested topology and the mutual sanity of
 the ATM traffic parameters and
 service categories, i.e., the row creation
 fails if:
 - the requested topology is incompatible with
 associated values of atmVplCastType,
 - the requested topology is not supported
 by the agent,
 - the traffic/service category parameter values
 associated with the requested row are
 incompatible with those of already existing
 rows for this VP cross-connect.
 [For example, for setting up
 a point-to-point VP cross-connect, the
 ATM traffic parameters in the receive direction
 of a VPL at the low end of the cross-connect
 must equal to the traffic parameters in the
 transmit direction of the other VPL at the
 high end of the cross-connect,
 otherwise, the row creation fails.]
 The agent also checks for internal errors
 in building the cross-connect.

 The atmVpCrossConnectIndex values in the
 corresponding atmVplTable rows are filled
 in by the agent at this point.

 (2b) The manager promotes the row in the
 atmVpCrossConnectTable by setting
 atmVpCrossConnectRowStatus to active(1). If
 this set is successful, the agent has reserved
 the resources specified by the ATM traffic
 parameter and Service category values
 for each direction of the VP cross-connect
 in an ATM switch or network.

 (3) The manager sets the
 atmVpCrossConnectAdminStatus to up(1) in all
 rows of this VP cross-connect to turn the
 traffic flow on.


 One-Shot VP Cross-Connect Establishment

 A VP cross-connect may also be established in
 one step by a set-request with all necessary
 parameter values and atmVpCrossConnectRowStatus
 set to createAndGo(4).

 In contrast to the negotiated VP cross-connect
 establishment which allows for detailed error
 checking (i.e., set errors are explicitly linked
 to particular resource acquisition failures),
 the one-shot VP cross-connect establishment
 performs the setup on one operation but does not
 have the advantage of step-wise error checking.

 VP Cross-Connect Retirement

 A VP cross-connect identified by a particular
 value of atmVpCrossConnectIndex is released by:

 (1) Setting atmVpCrossConnectRowStatus of all
 rows identified by this value of
 atmVpCrossConnectIndex to destroy(6).
 The agent may release all
 associated resources, and the
 atmVpCrossConnectIndex values in the
 corresponding atmVplTable row are removed.
 Note that a situation when only a subset of
 the associated rows are deleted corresponds
 to a VP topology change.

 (2) After deletion of the appropriate
 atmVpCrossConnectEntries, the manager may
 set atmVplRowStatus to destroy(6) the
 associated VPLs. The agent releases
 the resources and removes the associated
 rows in the atmVplTable.

 VP Cross-connect Reconfiguration

 At the discretion of the agent, a VP
 cross-connect may be reconfigured by
 adding and/or deleting leafs to/from
 the VP topology as per the VP cross-connect
 establishment/retirement procedures.
 Reconfiguration of traffic/service category parameter
 values requires release of the VP cross-connect
 before those parameter values may by changed
 for individual VPLs."
 */


class atmVpCrossConnectEntry: public MibTable {

public:
	atmVpCrossConnectEntry();
	virtual ~atmVpCrossConnectEntry();

	static atmVpCrossConnectEntry* instance;

	virtual void       	set_row(MibTableRow* r, int p0, int p1, int p2, int p3, int p4, int p5);
};


/**
 *  atmVcCrossConnectEntry
 *
"An entry in the ATM VC Cross Connect table.
 This entry is used to model a bi-directional ATM
 VC cross-connect cross-connecting two end points.

 Step-wise Procedures to set up a VC Cross-connect
 Once the entries in the atmVclTable are created,
 the following procedures are used
 to cross-connect the VCLs together to
 form a VCC segment.

 (1) The manager obtains a unique
 atmVcCrossConnectIndex by reading the
 atmVcCrossConnectIndexNext object.

 (2) Next, the manager creates a set of one
 or more rows in the ATM VC Cross Connect
 Table, one for each cross-connection between
 two VCLs. Each row is indexed by the ATM
 interface port numbers and VPI/VCI values of
 the two ends of that cross-connection.
 This set of rows specifies the topology of the
 VCC cross-connect and is identified by a single
 value of atmVcCrossConnectIndex.

 Negotiated VC Cross-Connect Establishment

 (2a) The manager creates a row in this table by
 setting atmVcCrossConnectRowStatus to
 createAndWait(5). The agent checks the
 requested topology and the mutual sanity of
 the ATM traffic parameters and
 service categories, i.e., the row creation
 fails if:
 - the requested topology is incompatible with
 associated values of atmVclCastType,
 - the requested topology is not supported
 by the agent,
 - the traffic/service category parameter values
 associated with the requested row are
 incompatible with those of already existing
 rows for this VC cross-connect.
 [For example, for setting up
 a point-to-point VC cross-connect, the
 ATM traffic parameters in the receive direction
 of a VCL at the low end of the cross-connect
 must equal to the traffic parameters in the
 transmit direction of the other VCL at the
 high end of the cross-connect,
 otherwise, the row creation fails.]
 The agent also checks for internal errors
 in building the cross-connect.

 The atmVcCrossConnectIndex values in the
 corresponding atmVclTable rows are filled
 in by the agent at this point.

 (2b) The manager promotes the row in the
 atmVcCrossConnectTable by setting
 atmVcCrossConnectRowStatus to active(1). If
 this set is successful, the agent has reserved
 the resources specified by the ATM traffic
 parameter and Service category values
 for each direction of the VC cross-connect
 in an ATM switch or network.

 (3) The manager sets the
 atmVcCrossConnectAdminStatus to up(1)
 in all rows of this VC cross-connect to
 turn the traffic flow on.


 One-Shot VC Cross-Connect Establishment

 A VC cross-connect may also be established in
 one step by a set-request with all necessary
 parameter values and atmVcCrossConnectRowStatus
 set to createAndGo(4).

 In contrast to the negotiated VC cross-connect
 establishment which allows for detailed error
 checking i.e., set errors are explicitly linked to
 particular resource acquisition failures), the
 one-shot VC cross-connect establishment
 performs the setup on one operation but does
 not have the advantage of step-wise error
 checking.

 VC Cross-Connect Retirement

 A VC cross-connect identified by a particular
 value of atmVcCrossConnectIndex is released by:

 (1) Setting atmVcCrossConnectRowStatus of all rows
 identified by this value of
 atmVcCrossConnectIndex to destroy(6).
 The agent may release all
 associated resources, and the
 atmVcCrossConnectIndex values in the
 corresponding atmVclTable row are removed.
 Note that a situation when only a subset of
 the associated rows are deleted corresponds
 to a VC topology change.

 (2) After deletion of the appropriate
 atmVcCrossConnectEntries, the manager may
 set atmVclRowStatus to destroy(6) the
 associated VCLs. The agent releases
 the resources and removes the associated
 rows in the atmVclTable.

 VC Cross-Connect Reconfiguration

 At the discretion of the agent, a VC
 cross-connect may be reconfigured by
 adding and/or deleting leafs to/from
 the VC topology as per the VC cross-connect
 establishment/retirement procedures.
 Reconfiguration of traffic/service category parameter
 values requires release of the VC cross-connect
 before those parameter values may by changed
 for individual VCLs."
 */


class atmVcCrossConnectEntry: public MibTable {

public:
	atmVcCrossConnectEntry();
	virtual ~atmVcCrossConnectEntry();

	static atmVcCrossConnectEntry* instance;

	virtual void       	set_row(MibTableRow* r, int p0, int p1, int p2, int p3, int p4, int p5);
};


/**
 *  aal5VccEntry
 *
"This list contains the AAL5 VCC
 performance parameters and is indexed
 by ifIndex values of AAL5 interfaces
 and the associated VPI/VCI values."
 */


class aal5VccEntry: public MibTable {

public:
	aal5VccEntry();
	virtual ~aal5VccEntry();

	static aal5VccEntry* instance;

	virtual void       	set_row(MibTableRow* r, int p0, int p1, int p2);
};


class atm_mib: public MibGroup
{
  public:
	atm_mib();
	virtual ~atm_mib() { }
};

#ifdef AGENTPP_NAMESPACE
}
#endif



/**
 * atm_mib.h generated by AgentGen 1.3.5 for AGENT++v3.4 with simulation extensions 
 * Tue Apr 18 02:03:56 GMT+04:30 2000.
 */


#endif


