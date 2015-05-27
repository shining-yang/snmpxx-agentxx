/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - snmp_request.cpp  
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

#include <libagent.h>

#include <agent_pp/snmp_request.h>
#include <agent_pp/snmp_counters.h>
#include <agent_pp/system_group.h>
#include <snmp_pp/log.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

static const char *loggerModuleName = "agent++.snmp_request";

//------------------------------ SnmpRequest -------------------------------

#ifdef _THREADS
#ifdef _WIN32THREADS
static void inform_caller(void* ptr)
#else
extern "C" {
static void *inform_caller(void* ptr)
#endif
{
	InformInfo* callInfo = (InformInfo*)ptr;
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	LOG("SnmpRequest: inform thread started");
	LOG_END;
	int status;
	status = SnmpRequest::process_trap(callInfo->target, callInfo->vbs,
					   callInfo->sz, callInfo->oid, "", TRUE);
	delete callInfo;
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	LOG("SnmpRequest: inform thread finished (status)");
	LOG(status);
	LOG_END;
#ifndef _WIN32THREADS
	return 0;
}
#endif
}
#endif

InformInfo::InformInfo(CTarget& t, Vbx* v, int s, const Oidx& o)
{
	target = t;
	vbs = new Vbx[s];
	// deep copy here because v may be delete while inform thread
	// is running
	for (int i=0; i<s; i++)
		vbs[i] = v[i];
	sz = s;
	oid = o;
}

InformInfo::~InformInfo()
{
	delete[] vbs;
}


int SnmpRequest::process(int rtype, const UdpAddress& address,
			 Vbx* vbs, int& sz, Vbx* out, int& errindex,
			 const OctetStr& community,
			 const int non_repeaters,
			 const int max_reps)
{
	if (!address.valid()) {
		return SNMP_CLASS_INVALID_ADDRESS;
	}
	CTarget target(address);

	int retries = DEFAULT_RETRIES;
	int timeout = DEFAULT_TIMEOUT;

	snmp_version version;
	if (rtype == sNMP_PDU_GETBULK) {
	  version = version2c;
	  timeout *= 2;
	}
	else
	  version = version1;

	int status;

	Snmpx* snmp = get_new_snmp(status);
	if (status != SNMP_CLASS_SUCCESS) {
		if (snmp) delete snmp;
		return status;
	}
	Pdux pdu;
	// the request id is set by Snmpx

	for (int i=0; i<sz; i++)
		pdu += vbs[i];

	target.set_version(version);    // set the SNMP version SNMPV1 or V2
        target.set_retry(retries);      // set the number of auto retries
        target.set_timeout(timeout);    // set timeout
        target.set_readcommunity(community);   // set read community

	switch (rtype) {
	case sNMP_PDU_GET:
		status = snmp->get(pdu,target);
		break;
	case sNMP_PDU_GETBULK:
		status = snmp->get_bulk(pdu,target,non_repeaters,max_reps);
		sz = pdu.get_vb_count();
		break;
	case sNMP_PDU_GETNEXT:
		status = snmp->get_next(pdu,target);
		break;
	case sNMP_PDU_SET:
		target.set_writecommunity(community);
		status = snmp->set(pdu,target);
		break;
	}

	MibIIsnmpCounters::incOutPkts();
	if (status != SNMP_CLASS_TIMEOUT)
		MibIIsnmpCounters::incInPkts();

	switch (pdu.get_error_status()) {
	case SNMP_ERROR_NO_SUCH_NAME: {
		MibIIsnmpCounters::incInNoSuchNames();
		break;
	}
	case SNMP_ERROR_BAD_VALUE: {
		MibIIsnmpCounters::incInBadValues();
		break;
	}
	case SNMP_ERROR_TOO_BIG: {
		MibIIsnmpCounters::incInTooBigs();
		break;
	}
	default: {
		if (pdu.get_error_status() != SNMP_ERROR_SUCCESS)
			MibIIsnmpCounters::incInGenErrs();
		break;
	}
	}
	delete snmp;
	for (int j=0; j < sz; j++) {
		pdu.get_vb(out[j], j);
	}

	errindex = pdu.get_error_index();

	return status;
}

int SnmpRequest::process_trap(SnmpTarget& target,
			      Vbx* vbs, int sz,
			      const Oidx& oid,
			      const Oidx& enterprise, bool ack)
{
	int status;

	Snmpx* snmp = get_new_snmp(status);
	// check construction status

	if (status != SNMP_CLASS_SUCCESS) {
		if (snmp) delete snmp;
		return status;
	}
	Pdux pdu;
	// the request id is set by Snmpx

	for (int i=0; i<sz; i++)
		pdu += vbs[i];

	pdu.set_notify_timestamp(sysUpTime::get());
	pdu.set_notify_id(oid);
	pdu.set_notify_enterprise(enterprise);

	if (ack)
		status = snmp->inform(pdu, target);
	else
		status = snmp->trap(pdu, target);

	if (status == SNMP_CLASS_SUCCESS) {
		MibIIsnmpCounters::incOutPkts();
		MibIIsnmpCounters::incOutTraps();
	}
	delete snmp;

	return status;
}




int SnmpRequest::get(const UdpAddress& address, Vbx* vbs, int sz, int& errind)
{
	MibIIsnmpCounters::incOutGetRequests();
	return process(sNMP_PDU_GET, address, vbs, sz, vbs, errind, "public");
}

int SnmpRequest::next(const UdpAddress& address, Vbx* vbs, int sz, int& errind)
{
	MibIIsnmpCounters::incOutGetNexts();
	return process(sNMP_PDU_GETNEXT, address, vbs, sz, vbs,
		       errind, "public");
}

int SnmpRequest::getbulk(const UdpAddress& address, Vbx* vbs, int& sz,
			 Vbx* out, int& errind,
			 const int non_repeater, const int max_reps)
{
	MibIIsnmpCounters::incOutGetNexts();
	return process(sNMP_PDU_GETBULK, address, vbs, sz, out,
		       errind, "public",
		       non_repeater, max_reps);
}

int SnmpRequest::set(const UdpAddress& address, Vbx* vbs, int sz, int& errind)
{
	MibIIsnmpCounters::incOutSetRequests();
	return process(sNMP_PDU_SET, address, vbs, sz, vbs, errind, "public");
}

int SnmpRequest::get(const UdpAddress& address, Vbx* vbs, int sz, int& errind,
		     const OctetStr& community)
{
	MibIIsnmpCounters::incOutGetRequests();
	return process(sNMP_PDU_GET, address, vbs, sz, vbs, errind, community);
}

int SnmpRequest::next(const UdpAddress& address, Vbx* vbs, int sz, int& errind,
		     const OctetStr& community)
{
	MibIIsnmpCounters::incOutGetNexts();
	return process(sNMP_PDU_GETNEXT, address, vbs, sz, vbs,
		       errind, community);
}

int SnmpRequest::getbulk(const UdpAddress& address, Vbx* vbs, int& sz,
			 Vbx* out, int& errind, const OctetStr& community,
			 const int non_repeater, const int max_reps)
{
	MibIIsnmpCounters::incOutGetNexts();
	return process(sNMP_PDU_GETBULK, address, vbs, sz, out, errind,
		       community,
		       non_repeater, max_reps);
}

int SnmpRequest::set(const UdpAddress& address, Vbx* vbs, int sz, int& errind,
		     const OctetStr& community)
{
	MibIIsnmpCounters::incOutSetRequests();
	return process(sNMP_PDU_SET, address, vbs, sz, vbs, errind, community);
}

int SnmpRequest::trap(SnmpTarget& target, Vbx* vbs, int sz,
		      const Oidx& oid, const Oidx& enterprise)
{
	return process_trap(target, vbs, sz, oid, enterprise);
}

void SnmpRequest::inform(CTarget& target, Vbx* vbs, int sz, const Oidx& oid)
{
	MibIIsnmpCounters::incOutTraps();

#ifdef _THREADS
	InformInfo* callInfo = new InformInfo(target, vbs, sz, oid);

#ifdef _WIN32THREADS
	_beginthread(inform_caller, 0, callInfo);
#else
    static pthread_attr_t* attr = 0;

	pthread_t thread;
	if (!attr) {
	  attr = new pthread_attr_t;
	  pthread_attr_init(attr);
	  pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED);
	}
	pthread_create(&thread, attr,
		       &inform_caller,
		       (void*) callInfo);
#endif
#else
	process_trap(target, vbs, sz, oid, "", TRUE);
#endif
}

int SnmpRequest::gettable(const UdpAddress& address, Vbx* vbs, int sz,
			  Vbx* buf, int& bufsz, const OctetStr& community,
			  const int max_reps)
{
	Vbx* out = new Vbx[max_reps*sz];
	Vbx* in  = new Vbx[sz];

	for (int j=0; j<sz; j++)
	  in[j] = vbs[j];

	int bufptr = 0;
	int osz = 0;
	int errind;
	int err = 0;

	while (err == SNMP_ERROR_SUCCESS) {
	  MibIIsnmpCounters::incOutGetNexts();
	  osz = sz;
	  err = process(sNMP_PDU_GETBULK, address, in, osz, out, errind,
			community, 0, max_reps);

	  if (err == SNMP_ERROR_SUCCESS) {
	    int i=0;
	    for (i=0; i<osz; i++, bufptr++) {

	      if (!out[i].get_oid().in_subtree_of(vbs[i%sz].get_oid())) break;

	      buf[bufptr] = out[i];

	      if (out[i].get_syntax() == sNMP_SYNTAX_ENDOFMIBVIEW)
		break;
	    }
	    if (i<osz) break;
	    for (int k=0; k<sz; k++) {
	      in[k] = buf[bufptr-sz+k];
	    }
	  }
	}
	bufsz = bufptr;

	delete [] in;
	delete [] out;

	return err;
}

Snmpx *SnmpRequest::get_new_snmp(int &status)
{
    Snmpx *snmpx;

    status = SNMP_CLASS_ERROR;

#ifdef USE_LISTEN_ADDRESS_FOR_SENDING_TRAPS
    if (Mib::instance &&
	Mib::instance->get_request_list() &&
	Mib::instance->get_request_list()->get_snmp())
    {
	UdpAddress addr = Mib::instance->get_request_list()->get_snmp()->
                                         get_listen_address();
	addr.set_port(0);
	snmpx = new Snmpx(status, addr);
    }
    else
    {
	LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
	LOG("SnmpRequestV3: Binding to all interfaces");
	LOG_END;
	snmpx = new Snmpx(status, 0);
    }
#else
    snmpx = new Snmpx(status, 0);
#endif
    return snmpx;
}

#ifdef _SNMPv3

//------------------------------ SnmpRequestV3 -------------------------------

SnmpRequestV3::SnmpRequestV3()
{
	int status;
	snmp = SnmpRequest::get_new_snmp(status);
	if (status != 0) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
		LOG("SnmpRequestV3: cannot init SNMP port (status)");
		LOG(status);
		LOG_END;
	}
}

SnmpRequestV3::~SnmpRequestV3()
{
	if (snmp) delete snmp;
}

int SnmpRequestV3::send(UTarget& target, Pdux& pdu)
{
	int status;

	Snmpx* snmp = SnmpRequest::get_new_snmp(status);
	if (status != SNMP_CLASS_SUCCESS) {
		if (snmp) delete snmp;
		return status;
	}

	switch (pdu.get_type()) {
	case sNMP_PDU_GET: {
		MibIIsnmpCounters::incOutGetRequests();
		status = snmp->get(pdu, target);
		break;
	}
    case sNMP_PDU_GETNEXT:
    case sNMP_PDU_GETBULK: {
        MibIIsnmpCounters::incOutGetNexts();
        status = snmp->get_next(pdu, target);
        break;
    }
	case sNMP_PDU_SET: {
		MibIIsnmpCounters::incOutSetRequests();
		status = snmp->set(pdu, target);
		break;
	}
	case sNMP_PDU_V1TRAP:
	case sNMP_PDU_TRAP: {
		MibIIsnmpCounters::incOutTraps();
		status = snmp->trap(pdu, target);
		break;
	}
	case sNMP_PDU_INFORM: {
		MibIIsnmpCounters::incOutTraps();
		status = snmp->inform(pdu, target);
		break;
	}
	}
	if (status == SNMP_CLASS_SUCCESS) MibIIsnmpCounters::incOutPkts();
	delete snmp;

	return status;
}

int SnmpRequestV3::send_request(UTarget& target, Pdux& pdu,
                                const int non_repeaters, const int repetitions)
{
	int status = SNMP_CLASS_INVALID_PDU;
	switch (pdu.get_type()) {
	case sNMP_PDU_GET: {
		MibIIsnmpCounters::incOutGetRequests();
		status = snmp->get(pdu, target);
		break;
	}
	case sNMP_PDU_GETNEXT: {
		MibIIsnmpCounters::incOutGetNexts();
		status = snmp->get_next(pdu, target);
		break;
    }
    case sNMP_PDU_GETBULK: {
        MibIIsnmpCounters::incOutGetNexts();
        if (target.get_version() == version1)
            status = snmp->get_next(pdu, target);
        else
            status = snmp->get_bulk(pdu, target, non_repeaters, repetitions);
        break;
	}
	case sNMP_PDU_SET: {
		MibIIsnmpCounters::incOutSetRequests();
		status = snmp->set(pdu, target);
		break;
	}
	case sNMP_PDU_V1TRAP:
	case sNMP_PDU_TRAP: {
		MibIIsnmpCounters::incOutTraps();
		status = snmp->trap(pdu, target);
		break;
	}
	case sNMP_PDU_INFORM: {
		MibIIsnmpCounters::incOutTraps();
		status = snmp->inform(pdu, target);
		break;
	}
	}
	if (status == SNMP_CLASS_SUCCESS) MibIIsnmpCounters::incOutPkts();

	return status;
}

#endif

#ifdef AGENTPP_NAMESPACE
}
#endif



