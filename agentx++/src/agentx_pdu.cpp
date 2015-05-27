/*_############################################################################
  _## 
  _##  agentx_pdu.cpp  
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

#include <agentx_pp/agentx_pdu.h>
#include <agent_pp/snmp_pp_ext.h>

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

// static const char *loggerModuleName = "agentx++.agentx_pdu";

/**
 * AgentXOctets
 */
const Oidx* AgentXOctets::internet = new Oidx("1.3.6.1");

AgentXOctets& AgentXOctets::operator+=(const OctetStr& other)
{
	*((OctetStr*)this) += other; 
	return *this;
}

AgentXOctets& AgentXOctets::operator+=(const u_char c)
{
	append(&c, 1);
	return *this;
}

void AgentXOctets::append(const unsigned char* buf, unsigned int sz) 
{
	unsigned char* ptr = smival.value.string.ptr;
	int l = len();
	smival.value.string.ptr = new unsigned char[l+sz];
	memcpy(smival.value.string.ptr, ptr, (size_t)l);
	memcpy(smival.value.string.ptr+l, buf, sz);
	smival.value.string.len = l + sz;
	delete[] ptr;
}

void AgentXOctets::add_null_octets(const unsigned int n)
{
	unsigned char* buf = new unsigned char[n]; 
	memset(buf, 0, (size_t)n);
	append(buf, n);
	delete[] buf;
}

void AgentXOctets::encode_int(u_int  integer, 
			      bool network_order)
{
	u_char buf[AGENTX_INT_SIZE];
	if (network_order) {
#ifndef WORDS_BIGENDIAN
		integer = ntohl(integer);
#endif
		memmove(buf, &integer, AGENTX_INT_SIZE);
	}
	else {
#ifndef WORDS_BIGENDIAN
		memmove(buf, &integer, AGENTX_INT_SIZE);
#else
		u_char* bufp = buf;
		*bufp = (u_char)integer & 0xff;	integer >>=8;	bufp++;
		*bufp = (u_char)integer & 0xff;	integer >>=8;	bufp++;
		*bufp = (u_char)integer & 0xff;	integer >>=8;	bufp++;
		*bufp = (u_char)integer & 0xff;
#endif		
	}
	this->append(buf, AGENTX_INT_SIZE);
}	

void AgentXOctets::encode_short(u_short integer,
				bool network_order)
{
	u_char buf[AGENTX_SHORT_SIZE];
	if (network_order) {
#ifndef WORDS_BIGENDIAN
		integer = ntohs(integer);
#endif
		memmove(buf, &integer, AGENTX_SHORT_SIZE);
	}
	else {
#ifndef WORDS_BIGENDIAN
		memmove(buf, &integer, AGENTX_SHORT_SIZE);
#else
		u_char* bufp = buf;
		*bufp = (u_char)integer & 0xff;	integer >>=8;	bufp++;
		*bufp = (u_char)integer & 0xff;
#endif		
	}
	this->append(buf, AGENTX_SHORT_SIZE);
}	

unsigned int AgentXOctets::decode_int(u_int& pos, bool networkOrder) 
{
	u_int value = 0;
	if (networkOrder) {
		memmove(&value, smival.value.string.ptr+pos, AGENTX_INT_SIZE);
#ifndef WORDS_BIGENDIAN
		value = ntohl(value);
#endif
	}
	else {
#ifndef WORDS_BIGENDIAN
		memmove(&value, smival.value.string.ptr+pos, AGENTX_INT_SIZE);
#else
		value += data()[pos+3];   value <<= 8;
		value += data()[pos+2];   value <<= 8;
		value += data()[pos+1];   value <<= 8;
		value += data()[pos];
#endif
	}
	pos += AGENTX_INT_SIZE;
	return value;
}

unsigned short AgentXOctets::decode_short(u_int& pos, bool networkOrder) 
{
	u_short value = 0;
	if (networkOrder) {
		memmove(&value, smival.value.string.ptr+pos, 
			AGENTX_SHORT_SIZE);
#ifndef WORDS_BIGENDIAN
		value = ntohs(value);
#endif
	}
	else {
#ifndef WORDS_BIGENDIAN
		memmove(&value, smival.value.string.ptr+pos, 
			AGENTX_SHORT_SIZE);
#else
		value += data()[pos+1];   value <<= 8;
		value += data()[pos];
#endif
	}
	pos += AGENTX_SHORT_SIZE;
	return value;
}

void AgentXOctets::encode_oid(const Oidx& o, 
	      		      bool networkOrder, bool include)
{
	unsigned int s = 0;
	if ((o.len()>=5) && (o.in_subtree_of(*internet))) {
		(*this) += (u_char)o.len()-5;
		*this += (unsigned char)o[4];
		s = 5;
	}
	else {
		(*this) += (u_char)o.len();
		add_null_octets(1);
	}
	if ((include) && (o.len()>0)) {
		(*this) += (u_char)1;
		add_null_octets(1);		
	}
	else	add_null_octets(2);
	
	for (unsigned int i=s; i < o.len(); i++) {
		this->encode_int((unsigned int)o[i], networkOrder);
	}
}

Oidx AgentXOctets::decode_oid(u_int& pos, bool networkOrder)
{
	bool include;
	return decode_oid(pos, networkOrder, include);
}

Oidx AgentXOctets::decode_oid(u_int& pos, bool networkOrder, 
			      bool& include)
{
	Oidx oid;
	u_char l = (*this)[pos];
	if ((*this)[pos+1]) {
		oid = *internet;
		oid += (*this)[pos+1];
	}
	include = ((*this)[pos+2]) > 0;
	pos += 4;
	for (int i=0; i<l; i++) {
		oid += (unsigned long)decode_int(pos, networkOrder);
	}
	return oid;
}

void AgentXOctets::encode_data(SnmpSyntax* v, bool networkOrder)
{
	if (!v) return;
	switch (v->get_syntax()) {
	  //case sNMP_SYNTAX_INT:
	case sNMP_SYNTAX_INT32: {
		int value = *((SnmpInt32*) v);
		encode_int(value, networkOrder);
		break;
	}
	case sNMP_SYNTAX_TIMETICKS:
	case sNMP_SYNTAX_CNTR32: 
	  //	case sNMP_SYNTAX_GAUGE32: 
	case sNMP_SYNTAX_UINT32: {
	        u_int value = *((SnmpUInt32*)v);
		encode_int(value, networkOrder);
		break;
	}
	case sNMP_SYNTAX_CNTR64: {
		u_int low, high;
	  	low  = ((Counter64*)v)->low();
		high = ((Counter64*)v)->high();
		if (networkOrder) {
			encode_int(high, networkOrder);
			encode_int(low , networkOrder);
		}
		else {
			encode_int(low , networkOrder);
			encode_int(high, networkOrder);
		}
		break;
	}
	case sNMP_SYNTAX_OCTETS: 
	case sNMP_SYNTAX_BITS:
	case sNMP_SYNTAX_OPAQUE: {
		encode_string(*(OctetStr*)v, networkOrder);
		break;
	}
	case sNMP_SYNTAX_IPADDR: {
		OctetStr ipaddr((const unsigned char*)"\x00\x00\x00\x00", 4);
		for (int i=0; i<4; i++) ipaddr[i] = (*(IpAddress*)v)[i];
		encode_string(ipaddr, networkOrder);
		break;
	}
	case sNMP_SYNTAX_OID: {
	  	encode_oid(*((Oid*)v), networkOrder);
		break;
	}
	/*
	case sNMP_SYNTAX_NULL:  
	case sNMP_SYNTAX_NOSUCHINSTANCE:
	case sNMP_SYNTAX_NOSUCHOBJECT: 
	case sNMP_SYNTAX_ENDOFMIBVIEW:
	case sNMP_SYNTAX_SEQUENCE:		
		break;
	*/
	}
       
}

SnmpSyntax* AgentXOctets::decode_data(u_int& pos, u_short type,
				      bool networkOrder)  
{
	SnmpSyntax* value = 0;
	switch (type) {
	  //case sNMP_SYNTAX_INT:
	case sNMP_SYNTAX_INT32: {
		value = new SnmpInt32((int)decode_int(pos, networkOrder));
		break;
	}
	case sNMP_SYNTAX_TIMETICKS: {
		value = new TimeTicks(decode_int(pos, networkOrder));
		break;
	}
	case sNMP_SYNTAX_CNTR32: {
		value = new Counter32(decode_int(pos, networkOrder));
		break;
	}		
	/*
	case sNMP_SYNTAX_GAUGE32: {
		value = new Gauge32(decode_int(pos, networkOrder));
		break;
	}	
	*/			
	case sNMP_SYNTAX_UINT32: {
		value = new SnmpUInt32(decode_int(pos, networkOrder));
		break;
	}
	case sNMP_SYNTAX_CNTR64: {
		u_int low, high;
	  	low  = decode_int(pos, networkOrder);
		high = decode_int(pos, networkOrder);
		if (networkOrder) {
			value = new Counter64(low, high);
		}
		else {
			value = new Counter64(high, low);
		}
		break;
	}
	case sNMP_SYNTAX_OCTETS: 
	case sNMP_SYNTAX_BITS: {
	  	value = new OctetStr(decode_string(pos, networkOrder));
		break;
	}
	case sNMP_SYNTAX_OPAQUE: {
	  	value = new OpaqueStr(decode_string(pos, networkOrder));
		break;
	}
	case sNMP_SYNTAX_IPADDR: {
		IpAddress* ipaddr = new IpAddress("0.0.0.0");
		OctetStr addr(decode_string(pos, networkOrder));
		for (int i=0; i<4; i++) (*ipaddr)[i] = addr[i];
		value = ipaddr;
		break;
	}
	case sNMP_SYNTAX_OID: {
	  	value = new Oid(decode_oid(pos, networkOrder));
		break;
	}
	case sNMP_SYNTAX_NULL:  
	case sNMP_SYNTAX_NOSUCHINSTANCE:
	case sNMP_SYNTAX_NOSUCHOBJECT: 
	case sNMP_SYNTAX_ENDOFMIBVIEW:
	case sNMP_SYNTAX_SEQUENCE: {	
		break;
	}
	}
	return value;
}
	

void AgentXOctets::encode_string(const OctetStr& str, bool networkOrder)
{
	encode_int((unsigned int)str.len(), networkOrder);
	(*this) += str;
	add_null_octets((4 - (str.len() % 4)) % 4);
}

OctetStr AgentXOctets::decode_string(u_int& pos, bool networkOrder) 
{
	u_int l = decode_int(pos, networkOrder);
	OctetStr str(data()+pos, l);
	pos += l + ((4 - (l % 4)) % 4);
	return str;
}



/**
 * AgentXRegion
 */

AgentXRegion::AgentXRegion(const Oidx& o): range(o, o)
{
	subid = 0;
	single = FALSE;
}

AgentXRegion::AgentXRegion(const Oidx& l, const Oidx& u): range(l, u)
{
	subid = 0;
	single = FALSE;
}


AgentXRegion::AgentXRegion(const Oidx& o, u_char sid, u_int u): range(o, o)
{
	single = FALSE;
	subid = sid;
	if (sid > 0) {
		if (sid<=range.upper.len())
			range.upper[sid-1] = u;
	}
	else {
		if (range.upper.len()>0)
			range.upper[o.len()-1]++;
	}
}

AgentXRegion::AgentXRegion(const AgentXRegion& other) 
{
	range = other.range;
	subid = other.subid;
	single = other.single;
}

int AgentXRegion::set(const Oidx& o, const u_char sid, const u_int u)
{
	if ((sid <= o.len()) && (u >= o[sid-1])) {
		range.lower = o; 
		subid = sid; 
		range.upper = o; range.upper[sid-1] = u;
		return SNMP_CLASS_SUCCESS;
	}
	return SNMP_CLASS_INTERNAL_ERROR;
}

void AgentXRegion::set_upper_bound(u_int u) 
{
	range.upper = range.lower;
	if (subid > 0) 
		range.upper[subid-1] = u;
}

u_int AgentXRegion::get_upper_bound()
{
	if ((subid > 0) && (subid < range.upper.len())) {
		return range.upper[subid-1];
	}
	return 0;
}

AgentXRegion& AgentXRegion::operator=(const AgentXRegion& other)
{
	range = other.range;
	subid = other.subid;
	single = other.single;
	return *this;
}

bool AgentXRegion::operator<(const AgentXRegion& other) const
{
	return (range < other.range);
}

bool AgentXRegion::operator>(const AgentXRegion& other) const
{
	return (range > other.range);
}

bool AgentXRegion::operator==(const AgentXRegion& other) const
{
	return ((range.lower == other.range.lower) && 
		(range.upper == other.range.upper) && 
		(subid == other.subid));
}

bool AgentXRegion::covers(const AgentXRegion& other) const
{
	if (subid) {
		return ((range.lower.compare(other.range.lower, subid) >= 0) &&
			(range.lower[subid-1] <= other.range.lower[subid-1]) &&
			(range.upper[subid-1] >= other.range.upper[subid-1]));
	}
	else if ((!subid) && (range.lower == range.upper)) {
		return (other.range.lower.in_subtree_of(range.lower));
	}
	return range.covers(other.range);
}

bool AgentXRegion::overlaps(const AgentXRegion& other) const
{
	if (subid) {
	  return ((range.lower.compare(other.range.lower, subid) >= 0) &&
		  (((range.lower[subid-1] >= other.range.lower[subid-1]) &&
		    (range.upper[subid-1] >  other.range.upper[subid-1])) ||
		   ((range.upper[subid-1] <= other.range.upper[subid-1]) &&
		    (range.lower[subid-1] >  other.range.lower[subid-1]))));
	}
	else if ((!subid) && (range.lower == range.upper)) {
		return (other.range.lower.in_subtree_of(range.lower));
	}
	return range.overlaps(other.range);
}

bool AgentXRegion::includes(const Oidx& id) const
{
	if (subid) {
		return ((range.lower.compare(id, subid) >= 0) && 
			(id[subid-1] >= range.lower[subid-1]) && 
			(id[subid-1] <= range.upper[subid-1]));
	}
	else if (range.lower == range.upper) {
		return id.in_subtree_of(range.lower);
	}
	return range.includes(id);
}


bool AgentXRegion::includes_excl(const Oidx& id) const
{
	if (subid) {
		return ((range.lower.compare(id, subid) >= 0) && 
			(id[subid-1] >= range.lower[subid-1]) && 
			(id[subid-1] <= range.upper[subid-1]));
	}
	else if (range.lower == range.upper) {
		return FALSE;
	}
	return range.includes_excl(id);
}

bool AgentXRegion::is_empty() const
{
	return (range.lower >= range.upper);
}

	
/*----------------------- class AgentXSearchRange-------------------*/

AgentXSearchRange::AgentXSearchRange(): range()
{
	include = FALSE;
}

AgentXSearchRange::AgentXSearchRange(const Oidx& l, const Oidx& u, 
				     bool incl): range(l, u)
{
	include = incl;
}

AgentXSearchRange::AgentXSearchRange(const AgentXSearchRange& other)
{
	range = other.range;
	include = other.include;
	reference = other.reference;
}

AgentXSearchRange::AgentXSearchRange(const OidxRange& other)
{
	range = other;
	include = FALSE;
}

AgentXSearchRange::~AgentXSearchRange() 
{
}

AgentXSearchRange* AgentXSearchRange::clone() const
{
	return new AgentXSearchRange(*this);
}

AgentXSearchRange& AgentXSearchRange::operator=(const AgentXSearchRange& other)
{
	range = other.range;
	include = other.include;
	reference = other.reference;
	return *this;
}

bool AgentXSearchRange::operator==(const AgentXSearchRange& other) const
{
	return ((range == other.range) && 
		((include) == (other.include)));
}

bool AgentXSearchRange::operator<(const AgentXSearchRange& other) const
{
	return ((range.upper < other.range.lower) || 
		((range.upper <= other.range.lower) &&  (!other.include)));
}

bool AgentXSearchRange::operator>(const AgentXSearchRange& other) const
{
	return ((range.lower > other.range.upper) || 
		((range.lower >= other.range.upper) && (include)));
}

bool AgentXSearchRange::includes(const Oidx& o) const
{
	return (((range.lower < o) || ((include) && (range.lower <= o))) && 
		((range.upper.len()==0) || (o < range.upper)));
}

bool AgentXSearchRange::includes_excl(const Oidx& o) const
{
	return ((range.lower < o) && 
		((range.upper.len()==0) || (o < range.upper)));
}



/**
 * AgentXPdu
 */


AgentXPdu::AgentXPdu(): Pdux()
{
	byteOrder = FALSE;
	init();
}

AgentXPdu::AgentXPdu(bool networkByteOrder, u_char t): Pdux()
{
	byteOrder = networkByteOrder;
	init();
	set_agentx_type(t);
}

void AgentXPdu::init()
{
	version = 1;
	timeout = 0;
	timestamp = 0;
	priority = AGENTX_DEFAULT_PRIORITY;
	reason = 0;
	flags = 0;
	ax_type = 0;
	packet_id = 0;
	session_id = 0;
	peer = 0;
	sysUpTime = 0;
}

AgentXPdu::AgentXPdu(const AgentXPdu& other): Pdux(other)
{
	byteOrder       = other.byteOrder;
	session_id	= other.session_id;
	packet_id	= other.packet_id;
	version		= other.version;

	ax_type		= other.ax_type;
	flags		= other.flags;
	context		= other.context;
	timeout		= other.timeout;
	priority	= other.priority;
	region		= other.region;

	ranges.clearAll();
	ListCursor<AgentXSearchRange> cur;
	for (cur.init(&other.ranges); cur.get(); cur.next()) {
		ranges.add(cur.get()->clone());
	} 

	timestamp	= other.timestamp;
	peer		= other.peer;
	sysUpTime	= other.sysUpTime;
}

AgentXOctets AgentXPdu::null_octets(const int n)
{
	AgentXOctets str;
	str.add_null_octets(n);
	return str;
}

AgentXPdu& AgentXPdu::operator=(const AgentXPdu& other)
{
	copy_from(other);
	return *this;
}

AgentXPdu& AgentXPdu::operator+=(const Vbx& vb)
{
#ifdef SNMP_PP_V3
	if (vb_count + 1 > vbs_size)
	{
	    if (!extend_vbs()) return *this;
	}
#else
	if (vb_count + 1 > PDU_MAX_VBS) return *this; // do we have room?
#endif

	vbs[vb_count] = new Vbx(vb);  // add the new one

	if (vbs[vb_count])   // up the vb count on success
	{
	    ++vb_count;
	    validity = true;   // set up validity
	}

	return *this;        // return self reference
}

bool AgentXPdu::operator<(const AgentXPdu& other) const
{
	return (timestamp < other.timestamp);
}

bool AgentXPdu::operator==(const AgentXPdu& other) const
{
	return (timestamp == other.timestamp);
}

bool AgentXPdu::operator>(const AgentXPdu& other) const
{
	return (timestamp == other.timestamp);
}

void AgentXPdu::copy_from(const AgentXPdu& other)
{
	byteOrder       = other.byteOrder;
	session_id	= other.session_id;
	packet_id	= other.packet_id;
	version		= other.version;

	ax_type		= other.ax_type;
	flags		= other.flags;
	context		= other.context;
	timeout		= other.timeout;
	priority	= other.priority;
	region		= other.region;

	// copy vbs
	clear();
	int count = other.get_vb_count();
	Vbx* _vbs = new Vbx[count];
	other.get_vblist(_vbs, count);
	for (int i=0; i<count; i++) {
		*this += _vbs[i];
	}
	delete[] _vbs;

	ranges.clearAll();
	ListCursor<AgentXSearchRange> cur;
	for (cur.init(&other.ranges); cur.get(); cur.next()) {
		ranges.add(cur.get()->clone());
	} 

	timestamp	= other.timestamp;
	peer		= other.peer;
	sysUpTime	= other.sysUpTime;

	//non_repeaters	= other.non_repeaters;
	//max_repetitions = other.max_repetitions;

	//	pdu		= other.pdu;

	// copy Pdu attributes
	set_error_status(other.get_error_status());
	set_error_index(other.get_error_index());
	set_request_id(other.get_request_id());
	set_type(other.get_type());
	TimeTicks t;
	other.get_notify_timestamp(t);
	set_notify_timestamp(t);
	Oid o;
	other.get_notify_id(o);
	set_notify_id(o);
	other.get_notify_enterprise(o);
	set_notify_enterprise(o);
#ifdef _SNMPv3
	set_security_level(other.get_security_level());
	OctetStr str;
	other.get_context_name(str);
	set_context_name(str);
	other.get_context_engine_id(str);
	set_context_engine_id(str);
	set_message_id(other.get_message_id());
#endif
}

Oidx AgentXPdu::get_id()
{
	Vbx vb;
	get_vb(vb, 0);
	return vb.get_oid();
}

OctetStr AgentXPdu::get_descr()
{
	Vbx vb;
	get_vb(vb, 0);
	OctetStr r;
	vb.get_value(r);
	return r;
}

void AgentXPdu::set_agentx_type(u_char t) 
{
	ax_type = t;
	// for backward (class hierarchy) compatibility
	switch (ax_type) {
	case AGENTX_GET_PDU: 
		set_type(sNMP_PDU_GET);
		break;
	case AGENTX_GETNEXT_PDU: 
		set_type(sNMP_PDU_GETNEXT);
		break;
	case AGENTX_GETBULK_PDU: 
		set_type(sNMP_PDU_GETBULK);
		break;
	case AGENTX_RESPONSE_PDU:
		set_type(sNMP_PDU_RESPONSE);
		break;
	default:
		set_type(sNMP_PDU_SET);
		break;
	}
}

void AgentXPdu::encode_header(AgentXOctets& header, int length) const
{
	int f = flags;
	f |= (context.len()>0) ? AGENTX_NON_DEFAULT_CONTEXT : 0; 
	f |= ((ax_type == AGENTX_REGISTER_PDU) && 
	      (region.is_single())) ? AGENTX_INSTANCE_REGISTRATION : 0; 
	f |= (network_byte_order()) ? AGENTX_NETWORK_BYTE_ORDER : 0;

	header += version;
	header += ax_type;
	header += f;
	header.add_null_octets(1);

	header.encode_int(session_id, network_byte_order());
	header.encode_int(get_transaction_id(), network_byte_order());
	header.encode_int(packet_id, network_byte_order());
	header.encode_int(length, network_byte_order());
}


void AgentXPdu::encode_region(AgentXOctets& octets, 
			      const AgentXRegion& r) const
{
	octets.encode_oid(r.get_lower(), byteOrder);
	if (r.is_range())
		octets.encode_int((u_int)r.get_upper()[r.get_subid()-1], 
				  byteOrder);
}

AgentXRegion AgentXPdu::decode_region(AgentXOctets& octets, u_int& pos, 
				      u_char rangeid) const
{
	Oidx oid(octets.decode_oid(pos, network_byte_order()));
	u_int range = 0;
	if ((rangeid>0) && (pos<octets.len()))
		range = octets.decode_int(pos, byteOrder);
	AgentXRegion _region(oid, rangeid, range);
	return _region;
}

void AgentXPdu::encode_vbs(AgentXOctets& octets, Vbx* _vbs, 
			   const int len) const
{
	for (int i=0; i<len; i++) {
		octets.encode_short((unsigned short)_vbs[i].get_syntax(), 
				    network_byte_order());
		octets.add_null_octets(AGENTX_SHORT_SIZE);
		octets.encode_oid(_vbs[i].get_oid(), network_byte_order());
		SnmpSyntax* v = _vbs[i].clone_value();
		octets.encode_data(v, network_byte_order());
		delete v;
	}
}

bool AgentXPdu::decode_vbs(AgentXOctets& octets, u_int& pos) 
{
	while (pos+2*AGENTX_INT_SIZE <= octets.len()) {
		Vbx vb;
		u_short syntax = 
		  octets.decode_short(pos, network_byte_order());
		pos += AGENTX_SHORT_SIZE; // skip reserved
		vb.set_oid(octets.decode_oid(pos, network_byte_order()));
		SnmpSyntax* data = octets.decode_data(pos, syntax,
						      network_byte_order());
		if (!data) {
			vb.set_syntax(syntax);
		} 
		else {
			vb.set_value(*data);
			delete data;
		}
		*this += vb;
	}
	return TRUE;
}

void AgentXPdu::encode_oids(AgentXOctets& octets, Vbx* _vbs, 
			    const int len) const
{
	for (int i=0; i<len; i++) 
		octets.encode_oid(_vbs[i].get_oid(), byteOrder);
}

void AgentXPdu::decode_oids(AgentXOctets& octets, u_int& pos)
{
	while (pos < octets.len()) {
		Vbx vb(octets.decode_oid(pos, byteOrder));
		*this += vb;
	}
}

void AgentXPdu::encode_ranges(AgentXOctets& octets) const
{
	ListCursor<AgentXSearchRange> cur;
	for (cur.init(&ranges); cur.get(); cur.next()) {
		octets.encode_oid(cur.get()->get_lower(), byteOrder, 
				  cur.get()->is_lower_included());
		octets.encode_oid(cur.get()->get_upper(), byteOrder);
	}
}

void AgentXPdu::decode_ranges(AgentXOctets& octets, u_int& pos) 
{
	while (pos<octets.len()) {
		bool include;
		Oidx lower(octets.decode_oid(pos, byteOrder, include));
		Oidx upper(octets.decode_oid(pos, byteOrder));
		ranges.add(new AgentXSearchRange(lower, upper, include));
	}
}

int AgentXPdu::encode_open_pdu(AgentXOctets& payload) const
{
	payload += timeout;
	payload += (u_char)0; payload += (u_char)0; payload += (u_char)0;
	
	if (get_vb_count() != 1) return SNMP_CLASS_INTERNAL_ERROR;
	Vbx      vb;
	Oidx	 id;
	OctetStr str;
 
	// TODO: error handling
	get_vb(vb, 0);
	vb.get_oid(id);
	vb.get_value(str);

	payload.encode_oid(id, byteOrder);
	payload.encode_string(str, byteOrder);
	return AGENTX_SUCCESS;
}

int AgentXPdu::encode(AgentXOctets& raw_pdu) const
{
	raw_pdu = "";
	AgentXOctets header;
	AgentXOctets payload;
	int status;

	switch(ax_type) {
	case AGENTX_OPEN_PDU: {
		if ((status = encode_open_pdu(payload)) != 
		    AGENTX_SUCCESS) return status;
		break;
	}
	case AGENTX_CLOSE_PDU: {
		payload += reason;
		payload += null_octets(3);
		break;
	}
	case AGENTX_REGISTER_PDU: 
	case AGENTX_UNREGISTER_PDU: {
		if (context.len()>0)
			payload.encode_string(context, byteOrder);
		if (ax_type == AGENTX_REGISTER_PDU)
			payload += timeout;
		else 
			payload += null_octets(1);
		payload += priority;
		payload += (u_char)region.get_subid();
		payload += null_octets(1);
		encode_region(payload, region);
		break;
	}
	case AGENTX_GET_PDU: {
		if (context.len()>0)
			payload.encode_string(context, byteOrder);
		encode_ranges(payload);
		break;
	}
	case AGENTX_GETNEXT_PDU: 
	case AGENTX_GETBULK_PDU: {
		if (context.len()>0)
			payload.encode_string(context, byteOrder);
		if (ax_type == AGENTX_GETBULK_PDU) {
			payload.encode_short(get_non_repeaters(), byteOrder);
			payload.encode_short(get_max_repetitions(), byteOrder);
		} 
		encode_ranges(payload);
		break;
	}
	case AGENTX_INDEXALLOCATE_PDU: 
	case AGENTX_INDEXDEALLOCATE_PDU: 
	case AGENTX_NOTIFY_PDU: 
	case AGENTX_TESTSET_PDU: {
		if (context.len()>0)
			payload.encode_string(context, byteOrder);
		Vbx* _vbs = new Vbx[get_vb_count()];
		get_vblist(_vbs, get_vb_count());
		encode_vbs(payload, _vbs, get_vb_count());
		delete[] _vbs;
		break;
	}		
	case AGENTX_COMMITSET_PDU:
	case AGENTX_UNDOSET_PDU:
	case AGENTX_CLEANUPSET_PDU: {
		// these PDUs consist of the AgentX header only
		break;
	}
	case AGENTX_PING_PDU: {
		if (context.len()>0)
			payload.encode_string(context, byteOrder);
		break;
	}
	case AGENTX_ADDAGENTCAPS_PDU: {
		if (context.len()>0)
			payload.encode_string(context, byteOrder);
		Vbx vb;
		if (get_vb_count() < 1) return AGENTX_MISSING_VB;
		get_vb(vb, 0);
		payload.encode_oid(vb.get_oid(), byteOrder);
		OctetStr str;
		vb.get_value(str);
		payload.encode_string(str, byteOrder);
		break;
	}
	case AGENTX_REMOVEAGENTCAPS_PDU: {
		if (context.len()>0)
			payload.encode_string(context, byteOrder);
		if (get_vb_count() < 1) return AGENTX_MISSING_VB;
		Vbx vb;
		get_vb(vb, 0);
		payload.encode_oid(vb.get_oid(), byteOrder);
		break;
	}
	case AGENTX_RESPONSE_PDU: {
		payload.encode_int(sysUpTime, network_byte_order());
		payload.encode_short((unsigned short)get_error_status(), 
				     network_byte_order());
		payload.encode_short((unsigned short)get_error_index(), 
				     network_byte_order());
		if (get_vb_count() > 0) {
			Vbx* _vbs = new Vbx[get_vb_count()];
			get_vblist(_vbs, get_vb_count());
			encode_vbs(payload, _vbs, get_vb_count());
			delete[] _vbs;
		}
		break;
	}
	}		
	encode_header(header, payload.len());
	raw_pdu += header;
	if (payload.len() > 0)
		raw_pdu += payload;

	return AGENTX_SUCCESS;
}	


int AgentXPdu::decode(AgentXOctets& raw_pdu) 
{
	if (raw_pdu.len()<20) return AGENTX_SHORT_PDU;

	version = raw_pdu[0];
	set_agentx_type(raw_pdu[1]);
	flags   = raw_pdu[2];
	
	byteOrder = (flags & AGENTX_NETWORK_BYTE_ORDER) ? TRUE : FALSE;
	bool isContext =(flags & AGENTX_NON_DEFAULT_CONTEXT) ? TRUE : FALSE;
	
	u_int pos = 4;
	session_id = raw_pdu.decode_int(pos, byteOrder);
	set_transaction_id(raw_pdu.decode_int(pos, byteOrder));
	packet_id =  raw_pdu.decode_int(pos, byteOrder);
	u_int plen = raw_pdu.decode_int(pos, byteOrder);
	
	if (raw_pdu.len() < 20+plen) return  AGENTX_SHORT_PDU;

	switch(ax_type) {
	case AGENTX_OPEN_PDU: {
		timeout = raw_pdu[pos];
		pos += 4; // skip 3 reserved bytes
		Vbx vb;
		vb.set_oid(raw_pdu.decode_oid(pos, byteOrder));
		vb.set_value(raw_pdu.decode_string(pos, byteOrder)); 
		*this += vb;
		break;
	}
	case AGENTX_CLOSE_PDU: {
	        reason = raw_pdu[pos];
		//pos += 4; // skip 3 reserved bytes
		break;
	}
	case AGENTX_REGISTER_PDU: 
	case AGENTX_UNREGISTER_PDU: {
		if (isContext)
			context = raw_pdu.decode_string(pos, byteOrder);
		if (AGENTX_REGISTER_PDU)
			timeout = raw_pdu[pos++];
		else
			pos++;
	        priority = raw_pdu[pos++];
		u_char rsubid = raw_pdu[pos++];
		pos++;
		set_region(decode_region(raw_pdu, pos, rsubid));
		region.set_single(flags & AGENTX_INSTANCE_REGISTRATION);
		break;
	}
	case AGENTX_GET_PDU: {
		if (isContext)
			context = raw_pdu.decode_string(pos, byteOrder);
		decode_ranges(raw_pdu, pos);
		break;
	}
	case AGENTX_GETNEXT_PDU: 
	case AGENTX_GETBULK_PDU: {
		if (isContext)
			context = raw_pdu.decode_string(pos, byteOrder);
		if (ax_type == AGENTX_GETBULK_PDU) {
			set_non_repeaters(raw_pdu.decode_short(pos, 
							       byteOrder));
			set_max_repetitions(raw_pdu.decode_short(pos, 
								 byteOrder)); 
		} 
		decode_ranges(raw_pdu, pos);
		break;
	}
	case AGENTX_INDEXALLOCATE_PDU: 
	case AGENTX_INDEXDEALLOCATE_PDU: 
	case AGENTX_NOTIFY_PDU: 
	case AGENTX_TESTSET_PDU: {
		if (isContext)
			context = raw_pdu.decode_string(pos, byteOrder);
		if (pos < raw_pdu.len()) {
			if (!decode_vbs(raw_pdu, pos)) 
				return AGENTX_REASON_PARSE_ERROR;
		}
		break;
	}	  
	case AGENTX_ADDAGENTCAPS_PDU: {
		if (isContext)
			context = raw_pdu.decode_string(pos, byteOrder);
		Vbx vb;
		vb.set_oid(raw_pdu.decode_oid(pos, byteOrder));
		OctetStr str = raw_pdu.decode_string(pos, byteOrder);
		vb.set_value(str);
		set_vblist(&vb, 1);
		break;
	}		
	case AGENTX_REMOVEAGENTCAPS_PDU: {
		if (isContext)
			context = raw_pdu.decode_string(pos, byteOrder);
		Vbx vb;
		vb.set_oid(raw_pdu.decode_oid(pos, byteOrder));
		set_vblist(&vb, 1);		
		break;
	}
	case AGENTX_COMMITSET_PDU:
	case AGENTX_UNDOSET_PDU:
	case AGENTX_CLEANUPSET_PDU: {
		// these PDUs consist of the AgentX header only
		break;
	}
	case AGENTX_PING_PDU: {
		if (isContext)
			context = raw_pdu.decode_string(pos, byteOrder);
		break;
	}
	case AGENTX_RESPONSE_PDU: {
		sysUpTime = raw_pdu.decode_int(pos, byteOrder);
		set_error_status(raw_pdu.decode_short(pos, byteOrder));
		set_error_index(raw_pdu.decode_short(pos, byteOrder));
		if (pos < raw_pdu.len()) {
			if (!decode_vbs(raw_pdu, pos)) 
			  return AGENTX_REASON_PARSE_ERROR;
		}
		break;
	}
	}		
	return AGENTX_OK;
}


void AgentXPdu::build_vbs_from_ranges()
{
	ListCursor<AgentXSearchRange> cur;
	for (cur.init(&ranges); cur.get(); cur.next()) {
		Vbx vb(cur.get()->get_lower());
		*this += vb;
	}
}

#ifdef AGENTPP_NAMESPACE
}
#endif

