/*_############################################################################
  _## 
  _##  agentx_pdu.h  
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

#ifndef agentx_pdu_h_
#define agentx_pdu_h_

#include <agent_pp/agent++.h>
#include <agent_pp/snmp_pp_ext.h>
#include <agent_pp/List.h>

#include <agentx_pp/agentx_def.h>
#include <agentx_pp/agentx_peer.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

class AGENTXPP_DECL AgentXOctets: public NS_SNMP OctetStr {

public:
	/** 
	 * constructor using no arguments
	 */
	AgentXOctets(): OctetStr() { }

	/**
	 * constructor using a  string
	 */
        AgentXOctets(const char* string): OctetStr(string) { }

	/**
	 * constructor using an unsigned char *
	 */
	AgentXOctets(const unsigned char* string, unsigned long int size):
	  OctetStr(string, size) { }

	/**
	 * constructor using another octet object
	 */
	AgentXOctets(const AgentXOctets& octet): OctetStr((OctetStr)octet) { }

	/**
	 * appends a string
	 */
	virtual AgentXOctets& operator+=(const OctetStr&);	

	/**
	 * appends a unsigned character 
	 */
	virtual AgentXOctets& operator+=(const unsigned char);	

	/**
	 * Encode a string with specified length
	 *
	 * @param string
	 *    a octet string
	 * @param length
	 *    the length of the string
	 */
	virtual void append(const unsigned char*, unsigned int);

	/**
	 * Encode a sequence of null octets
	 *
	 * @param n
	 *    the number of null octets to encode 
	 */
	virtual void add_null_octets(const unsigned int);

	/**
	 * Encode an integer
	 *
	 * @param i
	 *    an integer value
	 * @param byteOrder
	 *    if TRUE the integer will be encoded in network byte order,
	 *    otherwise it will be encoded in host byte order
	 */
	/*
	virtual void encode_int(int, bool);
	*/
	/**
	 * Encode an unsigned integer
	 *
	 * @param i
	 *    an unsigned integer value
	 * @param byteOrder
	 *    if TRUE the integer will be encoded in network byte order,
	 *    otherwise it will be encoded in host byte order
	 */
	virtual void encode_int(u_int, bool);

	/**
	 * Decode an integer
	 *
	 * @param pos
	 *    the start position of the encoded integer within the receiver.
	 *    After successful decoding, pos will be incremented by 4
	 * @param byteOrder
	 *    if TRUE the integer will be decoded in network byte order,
	 *    otherwise it will be decoded in host byte order
	 * @return 
	 *    the decoded unsigned integer
	 */
	virtual unsigned int decode_int(u_int&, bool);

	/**
	 * Decode an integer
	 *
	 * @param pos
	 *    the start position of the encoded integer within the receiver.
	 *    After successful decoding, pos will be incremented by 4
	 * @param byteOrder
	 *    if TRUE the integer will be decoded in network byte order,
	 *    otherwise it will be decoded in host byte order
	 * @return 
	 *    the decoded unsigned integer
	 */
	/*
	virtual int decode_sint(u_int&, bool);
	*/
	/**
	 * Encode an unsigned integer of a given size
	 *
	 * @param ui
	 *    an unsigned short value
	 * @param byteOrder
	 *    if TRUE the integer will be encoded in network byte order,
	 *    otherwise it will be encoded in host byte order
	 */
	virtual void encode_short(u_short, bool);

	/**
	 * Decode a short
	 *
	 * @param pos
	 *    the start position of the encoded short within the receiver.
	 *    After successful decoding, pos will be incremented by 2
	 * @param byteOrder
	 *    if TRUE the short will be decoded in network byte order,
	 *    otherwise it will be decoded in host byte order
	 * @return 
	 *    the decoded unsigned short
	 */
	virtual unsigned short decode_short(u_int&, bool);

	/**
	 * Encode an oid
	 *
	 * @param oid
	 *    an object identifier
	 * @param networkOrder
	 *    if TRUE the integer will be encoded in network byte order,
	 *    otherwise it will be encoded in host byte order
	 * @param include
	 *    only used when encoding a search range, include determines
	 *    if the oid is included (TRUE) or excluded from the search 
	 *    range
	 */
	virtual void encode_oid(const Oidx&, bool, bool=FALSE);

	/**
	 * Decode an oid
	 *
	 * @param pos
	 *    the start position of the encoded oid within the receiver.
	 *    After successful decoding, pos will be incremented by the
	 *    length of the oid encoding.
	 * @param networkOrder
	 *    if TRUE the oid will be encoded in network byte order,
	 *    otherwise it will be encoded in host byte order
	 * @return 
	 *    the decoded oid
	 */
	virtual Oidx decode_oid(u_int&, bool);

	/**
	 * Decode an oid
	 *
	 * @param pos
	 *    the start position of the encoded oid within the receiver.
	 *    After successful decoding, pos will be incremented by the
	 *    length of the oid encoding.
	 * @param networkOrder
	 *    if TRUE the oid will be encoded in network byte order,
	 *    otherwise it will be encoded in host byte order
	 * @param include
	 *    only used when decoding a search range, if TRUE is returned
	 *    the oid is included in the search range, otherwise it is
	 *    excluded from the range
	 * @return 
	 *    the decoded oid
	 */
	virtual Oidx decode_oid(u_int&, bool, bool&);

	
	/**
	 * Encode a SNMP value
	 *
	 * @param value
	 *    a SNMP value
	 * @param byteOrder
	 *    if TRUE the integer will be encoded in network byte order,
	 *    otherwise it will be encoded in host byte order
	 */
	virtual void encode_data(SnmpSyntax*, bool);

	/**
	 * Decode a SNMP value
	 *
	 * @param pos
	 *    the start position of the encoded SNMP value.
	 *    After successful decoding, pos will be incremented by the
	 *    length of the SNMP value enconding.
	 * @param syntax
	 *    the syntax of the SNMP value to be decoded.
	 * @param byteOrder
	 *    if TRUE the value will be decoded in network byte order,
	 *    otherwise it will be decoded in host byte order
	 * @return 
	 *    a pointer to the decoded SNMP value, or 0 if the syntax
	 *    is SnmpNull or an exception.
	 */
	virtual SnmpSyntax* decode_data(u_int&, u_short, bool);

	/**
	 * Encode a string
	 *
	 * @param string
	 *    a octet string
	 * @param byteOrder
	 *    if TRUE the integer will be encoded in network byte order,
	 *    otherwise it will be encoded in host byte order
	 */
	virtual void encode_string(const OctetStr&, bool);

	/**
	 * Decode a string
	 *
	 * @param pos
	 *    the start position of the encoded string within the receiver.
	 *    After successful decoding, pos will be incremented by the
	 *    length of the string enconding.
	 * @param byteOrder
	 *    if TRUE the integer will be decoded in network byte order,
	 *    otherwise it will be decoded in host byte order
	 * @return 
	 *    a pointer to the decoded SNMP value
	 */
	virtual OctetStr decode_string(u_int&, bool);
 protected:

	static const Oidx* internet;

};



class AGENTXPP_DECL AgentXRegion {

public:
	AgentXRegion() { }

	/**
	 * Construct a subtree region.
	 *
	 * @param subtree
	 *    an oid denoting a registration subtree.
	 */ 
	AgentXRegion(const Oidx&);

	/**
	 * Construct a region with lower and upper bound.
	 *
	 * @param lower
	 *    the lower bound of the region. 
	 * @param upper
	 *    the upper bound of the region. 
	 */
	AgentXRegion(const Oidx&, const Oidx&);

	/**
	 * Construct a subtree region with a range.
	 *
	 * @param subtree
	 *    an oid denoting a registration subtree.
	 * @param range_subid
	 *    specifies the subid of subtree (starting from one) that
	 *    may vary in a given range.
	 * @param upper
	 *    the upper bound for the subid specified by range_subid. 
	 */
	AgentXRegion(const Oidx&, unsigned char, unsigned int);

	/**
	 * Copy constructor.
	 */
	AgentXRegion(const AgentXRegion&); 

	virtual ~AgentXRegion() { }

	virtual int		set(const Oidx&, const u_char, const u_int);
	virtual bool		is_range() const { return (subid > 0); }

	virtual void		set_upper_bound(u_int u); 
	virtual u_int		get_upper_bound();
	virtual u_char		get_subid() const { return subid; }
	virtual void		set_subid(u_char sid) { subid = sid; }

	virtual AgentXRegion&	operator=(const AgentXRegion&);

	virtual bool		operator<(const AgentXRegion&) const;
	virtual bool		operator>(const AgentXRegion&) const;
	virtual bool		operator==(const AgentXRegion&) const;

	virtual bool		covers(const AgentXRegion&) const;
	virtual bool		overlaps(const AgentXRegion&) const;
	virtual bool		includes(const Oidx&) const;
	virtual bool		includes_excl(const Oidx&) const;

	virtual bool		is_empty() const;
	
	void			set_single(bool s) { single = s; }
	bool			is_single() const { return single; } 

	/**
	 * Get the lower bound.
	 *
	 * @return
	 *    a Oidx specifying the lower bound of the search range.
	 */
	Oidx		get_lower() const { return range.lower; }

	Oidx&           lower() { return range.lower; }

	/**
	 * Get the upper bound.
	 *
	 * @return
	 *    a Oidx specifying the upper bound of the search range.
	 */
	Oidx		get_upper() const { return range.upper; }

	Oidx&           upper() { return range.upper; }

	/**
	 * Set the upper bound.
	 *
	 * @param upper
	 *    an Oidx instance.
	 */
	void		set_upper(const Oidx& o) { range.upper = o; }

	/**
	 * Set the upper bound.
	 *
	 * @param upper
	 *    an Oidx instance.
	 */
	void		set_lower(const Oidx& o) { range.lower = o; }

protected:
	OidxRange	range;
	u_char		subid;
	bool		single;
};

/*----------------------- class AgentXSearchRange-------------------*/

/**
 * The AgentXSearchRange class represents an item in an AgentX 
 * SearchRangeList
 *
 * @author Frank Fock
 * @version 1.0
 */

class AGENTXPP_DECL AgentXSearchRange {

 public:
	/**
	 * Default constructor
	 */
	AgentXSearchRange();

	/**
	 * Create a SearchRange with a given lower and a upper bound,
	 * optionally define the lower bound as inclusive or exclusive.
	 *
	 * @param lower
	 *    the lower bound of the search range
	 * @param upper
	 *    the upper bound of the search range
	 * @param includeLower
	 *    if TRUE the lower bound is included in the search range,
	 *    otherwise it is excluded.
	 */
	AgentXSearchRange(const Oidx&, const Oidx&, bool = FALSE); 

	/**
	 * Copy constructor
	 *
	 * @param other
	 *    another SearchRange
	 */
	AgentXSearchRange(const AgentXSearchRange&);
	
	/**
	 * Copy constructor from OidxRange
	 *
	 * @param other
	 *    an OidxRange instance
	 */
	AgentXSearchRange(const OidxRange&);

	/**
	 * Destructor
	 */
	virtual ~AgentXSearchRange();

	/**
	 * Clone
	 *
	 * @return
	 *    a pointer to a clone of the receiver.
	 */
	virtual AgentXSearchRange* clone() const;

	virtual AgentXSearchRange& operator=(const AgentXSearchRange&); 

	/**
	 * Compare the receiver with another OID range for equality
	 *
	 * @param other
	 *    another OidxRange instance
	 * @return
	 *    TRUE if lower and upper bounds of both OID ranges are equal
	 */
	virtual bool	operator==(const AgentXSearchRange&) const;

	/**
	 * Compare the receiver with another OID range
	 *
	 * @param other
	 *    another OidxRange instance
	 * @return
	 *    TRUE if the upper bound of the receiver is less than the
	 *    lower bound of other
	 */
	virtual bool	operator<(const AgentXSearchRange&) const;

	/**
	 * Compare the receiver with another OID range
	 *
	 * @param other
	 *    another OidxRange instance
	 * @return
	 *    TRUE if the lower bound of the receiver is greater than the
	 *    upper bound of other
	 */
	virtual bool	operator>(const AgentXSearchRange&) const;

	/**
	 * Set whether the lower bound is included or not.
	 *
	 * @param include
	 *    if TRUE, the lower bound is included in the search range,
	 *    if FALSE, the lower bound is excluded.
	 */
	void		set_include(bool inc) { include = inc; }

	/**
	 * Check whether an OID is within the receiver's range
	 *
	 * @param oid
	 *    an object identifier
	 * @return
	 *    TRUE if oid is greater or equal to the receiver's lower 
	 *    bound and less or equal to its upper bound.
	 */
	virtual bool	includes(const Oidx&) const;

	/**
	 * Check whether an OID is within the receiver's range
	 *
	 * @param oid
	 *    an object identifier
	 * @return
	 *    TRUE if oid is greater as the receiver's lower 
	 *    bound and less than its upper bound.
	 */
	virtual bool	includes_excl(const Oidx&) const;

	/**
	 * Check whether the lower bound is is itself included or
	 * excluded from the search range
	 *
	 * @return
	 *    TRUE if the lower bound is included
	 */
	virtual bool is_lower_included() const { return include; }

	/**
	 * Unset upper bound.
	 */
	void		no_upper_bound() { range.upper.clear(); }

	/**
	 * Return TRUE if search range has no upper bound.
	 *
	 * @return 
	 *    TRUE if the receiver has no upper bound, FALSE otherwise.
	 */
	bool		is_unbounded() { return (range.upper.len() == 0); }

	/**
	 * Set the reference to a SNMP subrequest index.
	 *
	 * @param index
	 *    an index into a SNMP request PDU.
	 */
	void		set_reference(int ref) { reference = ref; }

	/**
	 * Get the reference for the receiver.
	 *
	 * @return
	 *    an index into a SNMP request PDU.
	 */
	int		get_reference() const { return reference; }

	/**
	 * Get the lower bound.
	 *
	 * @return
	 *    a Oidx specifying the lower bound of the search range.
	 */
	Oidx		get_lower() const { return range.lower; }

	/**
	 * Get the upper bound.
	 *
	 * @return
	 *    a Oidx specifying the upper bound of the search range.
	 */
	Oidx		get_upper() const { return range.upper; }

 protected:
	OidxRange	range;
	bool		include; 
	int		reference;
};
	
#if !defined (AGENTXPP_DECL_TEMPL_LIST_AGENTXSEARCHRANGE)
#define AGENTXPP_DECL_TEMPL_LIST_AGENTXSEARCHRANGE
	AGENTXPP_DECL_TEMPL template class AGENTXPP_DECL List<AgentXSearchRange>;
#endif

class AGENTXPP_DECL AgentXPdu: public Pdux {
  
public:
	/**
	 * Constructors
	 */ 
        AgentXPdu();
	/**
	 * Construct a pdu with a given byte order and type
	 *
	 * @param networkByteOrder
	 *    if TRUE the pdu uses network byte order, if FALSE
	 *    host byte order
	 * @param type
	 *    the type of the PDU
	 */
	AgentXPdu(bool, u_char);

	/**
	 * Copy constructor
	 */
	AgentXPdu(const AgentXPdu&);

	/**
	 * Clone the receiver.
	 */
	virtual Pdux*   clone() const { return new AgentXPdu(*this); }

	/**
	 * get/set Pdu's version
	 */ 
	
	virtual void	set_version(u_char v)           { version = v; }
	virtual u_char	get_version() const	      	{ return version; }

	/**
	 * Get/Set flags
	 */
	virtual u_char  get_flags() const      		{ return flags; }
	virtual void    set_flags(const u_char f)      	{ flags = f; } 
	
	/**
	 * get/set session id
	 */
	virtual u_int	get_session_id() const		{ return session_id; }
	virtual void	set_session_id(const u_int i)  	{ session_id = i; }
	
	/**
	 * get/set transaction id
	 */
	virtual u_int	get_transaction_id() const   
					{ return (u_int)get_request_id(); }
	virtual void	set_transaction_id(const u_int i)
						 { set_request_id(i); }
	
	/**
	 * get/set packet id
	 */
	virtual u_int	get_packet_id()	const		{ return packet_id; }
	virtual void	set_packet_id(const u_int i)   	{ packet_id = i; }
	
	/**
	 * get/set context
	 */
	virtual NS_SNMP OctetStr	get_context() const    	{ return context; }
	virtual void		set_context(const NS_SNMP OctetStr& s)	
							{ context = s; }
	virtual bool		non_default_context() 
					{ return (context.len()>0); }
	
	/**
	 * get/set timeout
	 */
	virtual u_char		get_timeout() const   	{ return timeout; }
	virtual void		set_timeout(const u_char c) { timeout = c; }
	
	/**
	 * get/set priority
	 */
	virtual u_char		get_priority() const  	{ return priority; }
	virtual void		set_priority(const u_char c) { priority = c; }
	
	/**
	 * get/set reason
	 */
	virtual u_char		get_reason() const  	{ return reason; }
	virtual void		set_reason(const u_char c) { reason = c; }
	
	/**
	 * get/set region (set returns SNMP_CLASS_SUCCESS on success)
	 */
	virtual AgentXRegion	get_region() const      { return region; }
	virtual int		set_region(const Oidx& o, const int sid, 
					   const u_long u)
				{ return region.set(o, sid, u); }
	virtual void		set_region(const AgentXRegion& r) 
							{ region = r; }
	
	/**
	 * get/add oid range
	 */
	virtual int		get_range_count() const 
						{ return ranges.size(); }
	AgentXSearchRange*	range(const int i) { return ranges.getNth(i);}
	virtual AgentXSearchRange	get_range(const int i) const  
						{ return *ranges.getNth(i); }
	virtual void		add_range(const AgentXSearchRange& r)
						{ ranges.add(r.clone()); }
	virtual int		trim_ranges(const int n)
						{ return ranges.trim(n); }
	virtual void		set_range(AgentXSearchRange* r, const int i) 
						{ ranges.overwriteNth(i, r); }
	virtual ListCursor<AgentXSearchRange> get_ranges() { 
		       	return ListCursor<AgentXSearchRange>(&ranges); }
	
	/**
	 * get/set non repeaters
	 */
	u_int		get_non_repeaters() const { return get_error_status();}
	void		set_non_repeaters(const u_int i) 
						{ set_error_status(i); }
	void		inc_non_repeaters() { 
			       	set_error_status(get_error_status()+1); }
	
	/**
	 * get/set max repetitions
	 */
	virtual u_int	get_max_repetitions() const {return get_error_index();}
	virtual void	set_max_repetitions(u_int i)  { set_error_index(i); }
	
	/**
	 * get/set error status
	 */
	/*
	virtual u_int	get_error_status()  
				{ return (u_int)pdu.get_error_status(); }
	virtual void	set_error_status(u_int i) { pdu.set_error_status(i); }
	*/
	/**
	 * get/set error index
	 */
	/*
	virtual u_int	get_error_index() 
				{ return (u_int)pdu.get_error_index(); }
	virtual void	set_error_index(u_int i)  { pdu.set_error_index(i); }
	*/

	/**
	 * get/set timestamp	
	 */
	virtual time_t	get_timestamp()	const		{ return timestamp; }
	virtual void	set_timestamp(time_t t)		{ timestamp = t; }
	
	/**
	 * get/set sysUpTime	
	 */
	virtual time_t	get_time()	const		{ return sysUpTime; }
	virtual void	set_time(u_int i)		{ sysUpTime = i; }
	
	virtual bool	network_byte_order() const	{ return byteOrder; }

	u_char		get_agentx_type() const { return ax_type; }
	void		set_agentx_type(u_char t);

	Oidx		get_id();
	NS_SNMP OctetStr        get_descr();

	/**
	 * get/set AgentXPeer
	 */
	virtual AgentXPeer*     get_peer() { return peer; }
	virtual void		set_peer(AgentXPeer* p) { peer = p; }

	/**
	 * operator =
	 */
	virtual AgentXPdu&	operator=(const AgentXPdu&);

	/**
	 * Adds a variable binding even if its OID is a zero length OID,
	 * because these are valid OIDs in AgentX.
	 * 
	 * @since 1.4.11
	 */
	virtual AgentXPdu&	operator+=(const Vbx&);

	/**
	 * operators
	 */
	virtual bool		operator<(const AgentXPdu&) const;
	virtual bool		operator==(const AgentXPdu&) const;
	virtual bool		operator>(const AgentXPdu&) const;

	/**
	 * encode/decode the Pdu
	 */
	virtual int	   	encode(AgentXOctets&) const; 
	virtual	int		decode(AgentXOctets&);

	/**
	 * For each search range in the receiver, create a variable binding
	 * with the lower bound of the search range as oid and null as value.
	 */
	virtual void		build_vbs_from_ranges();

protected:
	
	virtual void		copy_from(const AgentXPdu&);
	
	virtual void      	encode_header(AgentXOctets&, int) const;
	virtual void		encode_ranges(AgentXOctets&) const;
	virtual void		decode_ranges(AgentXOctets&, u_int&);
	virtual void		encode_region(AgentXOctets&, 
					      const AgentXRegion&) const;
	virtual AgentXRegion   	decode_region(AgentXOctets&, u_int&,
					      u_char) const;
	virtual void		encode_vbs(AgentXOctets&, Vbx*, 
					   const int) const;
	virtual bool		decode_vbs(AgentXOctets&, u_int&);
	virtual void		encode_oids(AgentXOctets&, Vbx*, 
					    const int) const;
	virtual void		decode_oids(AgentXOctets&, u_int&); 

	static AgentXOctets    	null_octets(const int);

      	u_char			version;
	u_int			session_id;
	u_int			packet_id;

	u_char			ax_type;
	u_char			flags;
	NS_SNMP OctetStr	context;
	u_char			timeout;
	u_char			priority;
	u_char			reason;
	AgentXRegion		region;
	
	List<AgentXSearchRange>	ranges;
	/*
	u_int			non_repeaters;
	u_int			max_repetitions;
	Pdux			pdu;
	*/

	time_t			timestamp;
	u_int			sysUpTime;

	bool			byteOrder;

	AgentXPeer*		peer;

private:
	void			copy(const AgentXPdu&);

	int			encode_open_pdu(AgentXOctets&) const;
	void			init();
};

#ifdef AGENTPP_NAMESPACE
}
#endif


#endif

