/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - snmp_pp_ext.h  
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

#ifndef _SNMP_PP_EXT_H_
#define _SNMP_PP_EXT_H_

#include <libagent.h>

#include <agent_pp/agent++.h>
#include <snmp_pp/smi.h>
#include <snmp_pp/pdu.h>
#include <snmp_pp/vb.h>
#include <snmp_pp/octet.h>
#include <snmp_pp/oid.h>
#include <snmp_pp/snmpmsg.h>

#ifndef WIN32
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#endif

#ifdef SNMP_PP_NAMESPACE
#define NS_SNMP Snmp_pp::
#else
#define NS_SNMP
#endif

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


/*--------------------------- class Oidx -----------------------------*/

/**
 * The Oidx class is a sub class of Oid that provides additional
 * methods for manipulating OID values. AGENT++ uses Oidx instead
 * of Oid.
 *
 * @version 3.4
 * @author Frank Fock
 */

class AGENTPP_DECL Oidx: public NS_SNMP Oid {
public:
	/**
	 * Construct an empty Object Identifier.
	 */
	Oidx() : Oid() { }

	/**
	 * Construct an Object Identifier from a string.
	 *
	 * @param s - A oid string (e.g., ".x.y.z" or "x.y.z")
	 */
	Oidx(const char* s) : Oid(s) { }

	/**
	 * Construct an Object Identifier from a long array.
	 *
	 * @param data - An array of long values.
	 * @param length - The length of the array.
	 */
	Oidx(const unsigned long *data, int length) : Oid(data, length) { }

	/**
	 * Copy constructor.
	 *
	 * @param oid - Another object identifier.
	 */
	Oidx(const Oid& oid) : Oid(oid) { }

	/**
	 * Destructor
	 */
	virtual ~Oidx() { }

	/**
	 * Return a copy of the receiver oid without the n leftmost
	 * subidentifiers.
	 *
	 * @param n - The number of subidentifiers to cut of from left side.
	 * @return An Oidx object identifier.
	 */
	Oidx		cut_left(const unsigned int index) const
	{
		return cut_left(*this, index);
	}

	/**
	 * Return a copy of the receiver oid without the n rightmost
	 * subidentifiers.
	 *
	 * @param n - The number of subidentifiers to cut of from right side.
	 * @return An Oidx object identifier.
	 */
	Oidx		cut_right(const unsigned int index) const
	{
		return cut_right(*this, index);
	}


	/**
	 * Mask the receiver, i.e. zero those sub-identifiers for which
	 * a bit in the mask is set.
	 * Each bit of the bit mask corresponds to a sub-identifier,
	 * with to most significant bit of the i-th octet of the octet
	 * string value corresponding to the (8*i - 7)-th sub-identifier,
	 * and the least significant bit of the i-th octet corresponding
	 * to the (8*i)-th sub-identifier.
	 *
	 * @param mask
	 *    an octet string.
	 * @return a reference to the receiver.
	 */
	Oidx&		mask(const NS_SNMP OctetStr &mask)
	{
		for (unsigned int i=0; (i<len()) && (i<mask.len()*8); i++) {
			char m = 0x80 >> (i%8);
			if (!(mask[i/8] & m)) {
				(*this)[i] = 0ul;
			}
		}
		return *this;
	}


	/**
	 * Return a copy of the given oid without the n leftmost
	 * subidentifiers.
	 *
	 * @param o - An Oidx object identifier.
	 * @param n - The number of subidentifiers to cut of from left side.
	 * @return An Oidx object identifier.
	 */
	static Oidx	cut_left(const Oidx &oid, const unsigned int index)
	{
		if (oid.valid()) {
			Oidx retval( oid.smival.value.oid.ptr + index, oid.smival.value.oid.len - index );
#if 0
			unsigned int i;
			for (i=index; i<oid.smival.value.oid.len; i++)
				retval += oid.smival.value.oid.ptr[i];
#endif
			return retval;
		}
		else
		    return Oidx();
	}

	/**
	 * Return a copy of the given oid without the n rightmost
	 * subidentifiers.
	 *
	 * @param o - An Oidx object identifier.
	 * @param n - The number of subidentifiers to cut of from right side.
	 * @return An Oidx object identifier.
	 */
	static Oidx	cut_right(const Oidx &oid, const unsigned int index)
	{
		if (oid.valid()) {
			unsigned int l = oid.smival.value.oid.len >= index ? oid.smival.value.oid.len - index : 0;
			Oidx retval( oid.smival.value.oid.ptr, l );
#if 0
			Oidx retval;
			unsigned int i;
			unsigned int s = index;
			if (s>oid.len())
				s = oid.len();
			for (i=0; i<oid.len()-s; i++)
				retval += oid.smival.value.oid.ptr[i];
#endif
			return retval;
		}
		else
		    return Oidx();
	}

	/**
	 * Return the last subidentifier of the receiver.
	 *
	 * @return A subidentifier or 0 if it does not exist.
	 */
	unsigned long	last() const
	{
		// check for len == 0
		if ((!Oid::valid()) || (smival.value.oid.len<1))
			return 0;

		return smival.value.oid.ptr[smival.value.oid.len-1];
	}

	using NS_SNMP Oid::operator = ;
	virtual Oidx&  	operator = (unsigned long l)
	{
		// delete the old value
		if ( smival.value.oid.ptr ) {
			delete [] smival.value.oid.ptr;
			smival.value.oid.ptr = NULL;
		}
		smival.value.oid.len = 1;
		smival.value.oid.ptr = (SmiLPUINT32)new unsigned long[1];
		smival.value.oid.ptr[0] = l;
		return *this;
	}

	using NS_SNMP Oid::operator += ;
	Oidx		&operator+=(NS_SNMP IpAddress const &ip)
	{
		for (int i = 0; i < ip.get_length(); i++)
			*this += (unsigned long)ip[i];
		return *this;
	}
#if 0
	Oidx		&operator+=(const char *);
	Oidx		&operator+=(const unsigned long i);
	Oidx		&operator+=(const Oid &);
#endif

	/**
	 * Check if the receiver is in the subtree of a given oid.
	 *
	 * @param o - An Oidx object identifier.
	 * @return TRUE if the receiver is in the subtree of o,
	 *         FALSE otherwise.
	 */
	bool		in_subtree_of(const Oidx& o) const
	{
		if (len() <= o.len()) return FALSE;
		for (unsigned int i=0; i<o.len(); i++)
			if ((*this)[i] != o[i]) return FALSE;
		return TRUE;
	}

	/**
	 * Check if the receiver is root of a given oid.
	 *
	 * @param o - An Oidx object identifier.
	 * @return TRUE if the receiver is root of o,
	 *         FALSE otherwise.
	 */
	bool		is_root_of(const Oidx& o) const
	{
		if (len() >= o.len()) return false;
		for (unsigned int i=0; i<len(); i++)
			if ((*this)[i] != o[i]) return false;
		return true;
	}

	/**
	 * Compare the receiver with another object identifier
	 * using a mask which defines to subidentifiers to be compared.
	 * The mask is used like defined in RFC2273 for the
	 * snmpNotifyFilterMask.
	 *
	 * @param other
	 *    another object identifier to compare the receiver with.
	 * @param mask
	 *    an octet string containing a mask.
	 * @return 0 if both (masked) object identifiers are equal,
	 *    return 1 if the given object identifier is in the subtree
	 *    of the receiver, and return -1 otherwise.
	 */
	int		compare(const Oidx&, const NS_SNMP OctetStr&) const;

	/**
	 * Compare the receiver with another object identifier
	 * using a wildcard at the given subidentifier
	 *
	 * @param other
	 *    another object identifier to compare the receiver with.
	 * @param wildcard_subid
	 *    the subidentifier position (>=0) that is not compared
	 * @return 0 if both (masked) object identifiers are equal,
	 *    return 1 if the given object identifier is in the subtree
	 *    of the receiver, and return -1 otherwise.
	 */
	int		compare(const Oidx&, u_int) const;

	/**
	 * Return the receiver as an OctetStr. Every subidentifier is
	 * interpreted as one char. Thus, all subidentifiers must be
	 * between 0 and 255.
	 *
	 * @param withoutLength
	 *    if TRUE there will be no preceeding subid containing
	 *    the length of the string
	 * @return An OctetStr.
	 */
	NS_SNMP OctetStr	as_string(bool withoutLength = false) const
	{
		OctetStr str;

		int i = 0;
		// check if the len is implied and should be ignored!
		if (withoutLength && len() > 0 && len() == ((*this)[0] + 1))
		    i++;    // first oid seems to be the len
		for (; i<(int)len(); i++) {
			str += (unsigned char)(*this)[i];
		}
		return str;
	}

	/**
	 * Return an object identifier from a string. The first
	 * subidentifier will contain the length of the string, each
	 * following subidentifier represents one character.
	 *
	 * @param withExplicitLength
	 *    if FALSE there will be no preceeding subid containing
	 *    the length of the string will be generated.
	 * @return An Oidx.
	 */
	static Oidx	from_string(const NS_SNMP OctetStr &str, bool withLength = TRUE)
	{
		Oidx oid;
		if (withLength)
			oid += (long)str.len();
		for (unsigned int i=0; i<str.len(); i++)
			oid += (long)str[i];
		return oid;
	}

	/**
	 * Return the immediate lexicographic successor of the receiver.
	 *
	 * @return
	 *    an Oidx instance (receiver.0)
	 */
	Oidx		successor() const
	{
		Oidx o(*this);
		if (o.len()==MAX_OID_LEN) {
			if (o[MAX_OID_LEN-1] == 0xFFFFFFFFul) {
				int i = MAX_OID_LEN-2;
				while ((i>=0) && (o[i] == 0xFFFFFFFFul)) {
					i--;
				}
				if (i>=0) {
					o.trim(MAX_OID_LEN-1-i);
					o[i]++;
				}
			}
			else
				o[MAX_OID_LEN-1]++;
		}
		else
			o += 0ul;
		return o;
	}

	/**
	 * Return the immediate lexicographic predecessor of the receiver.
	 *
	 * @return
	 *    an Oidx instance immediate preceding the receiver.
	 */
	Oidx		predecessor() const
	{
		Oidx o(*this);
		if (o.len()==0) return o;
		if (o[o.len()-1] == 0) {
			o.trim();
			return o;
		}
		o[o.len()-1]--;
		for (int i=o.len(); i<MAX_OID_LEN; i++)
			o += 0xFFFFFFFFul;
		return o;
	}

	/**
	 * Return the next peer of the receiver. Thus, add one to the
	 * last sub-identifier.
	 * Note: If the receiver's last sub-identifier is 0xFFFFFFFFul
	 * the returned object ID may not be valid.
	 *
	 * @return
	 *    an Oidx instance.
	 */
	Oidx		next_peer() const
	{
		Oidx o(*this);
		o[o.len()-1]++;
		return o;
	}
};


/*--------------------------- class Vbx -----------------------------*/

/**
 * The Vbx class is a sub class of Vb that provides additional
 * methods for manipulating SNMP variable bindings values.
 * AGENT++ uses Vbx instead of Vb. Most of the base methods of Vb
 * are redefined as const methods.
 *
 * @version 3.4
 * @author Frank Fock
 */


class AGENTPP_DECL Vbx: public NS_SNMP Vb {
public:
	Vbx(): Vb() { }
	Vbx(const NS_SNMP Vb& vb): Vb(vb) { }
	Vbx(const NS_SNMP Oid& oid): Vb(oid) { }

	/**
	 * Constructor with oid and value.
	 *
	 * @param oid
	 *    the oid part of the new variable binding.
	 * @param value
	 *    the value part of the new variable binding.
	 */
	Vbx(const NS_SNMP Oid& oid, const NS_SNMP SnmpSyntax &val): Vb(oid) {
	  set_value(val);
	}

	/**
	 * Return the oid value of the receiver.
	 *
	 * @return
	 *    a copy of the oid value of the receiver.
	 */
	Oidx		get_oid() const { return iv_vb_oid; };

	void		get_oid(Oidx& oid) const { oid = iv_vb_oid; };

	/**
	 * Clear the content of the variable binding. The content of
	 * the receiver will be the same as after creating it by Vbx()
	 */
	void		clear();

	Vbx*		clone() const { return new Vbx(*this); }

	friend int	operator==(const Vbx&, const Vbx&);

	/**
	 * Compare two Vbx arrays for equality.
	 *
	 * @param vbxArray1
	 *    an array of n Vbx variable bindings.
	 * @param vbxArray2
	 *    an array of n Vbx variable bindings.
	 * @param n
	 *    the number of elements in the above arrays.
	 * @return
	 *    TRUE if the oids and the values of both variable binding
	 *    arrays are equal.
	 */
	static bool	equal(Vbx*, Vbx*, int);

	/**
	 * Encode a given array of variable bindings into a octet stream
	 * using ASN.1 (BER). Note: The list of variable bindings is
	 * encoded as a sequence of oid/value pairs.
	 *
	 * @param vbs
	 *    a pointer to an array of variable bindings.
	 * @param size
	 *    the size of the above array.
	 * @param buf
	 *    returns a pointer to the character stream that holds the
	 *    BER encoding. Note that the user of this method is
	 *    responsible to free the memory allocated for buf.
	 * @param bufsize
	 *    the size of the returned character stream.
	 * @return
	 *    0 on success.
	 */
	static int	to_asn1(Vbx*, int, unsigned char*&, int&);

	/**
	 * Decode a character stream that holds BER encoded
	 * sequence of variable bindings into an array of variable
	 * bindings.
	 *
	 * @param vbs
	 *    returns a pointer to an array of variable bindings.
	 *    The caller has to delete the pointer if success is returned.
	 * @param size
	 *    returns the size of the above array.
	 * @param buf
	 *    a pointer to a character stream holding a BER encoded
	 *    sequence of variable bindings. After execution, this
	 *    pointer will point to first character in buf after the
	 *    decoded sequence.
	 * @param bufsize
	 *    the number of characters left in the buffer (for decoding).
	 *    This will be decremented by the amount of decoded bytes.
	 */
	static int	from_asn1(Vbx*&, int&, unsigned char*&, int&);

 private:
	static unsigned char* asn_build_long_len_sequence(unsigned char *,
							  int*,
							  unsigned char,
							  int, int);
	static unsigned char* asn_build_long_length(unsigned char*,
						    int*,
						    int,
						    int);
};


/*------------------------- class OidxRange ---------------------------*/

/**
 * The OidxRange class provides functionality to define and compare
 * OID ranges.
 *
 * @version 3.03
 * @author Frank Fock
 */

class AGENTPP_DECL OidxRange {

public:
	/**
	 * Default constructor
	 */
	OidxRange();

	/**
	 * Create an OID range with lower and upper bound.
	 *
	 * @param lower
	 *    the lower bound of the oid range (must be lexicographically
	 *    less than upper)
	 * @param upper
	 *    the upper bound of the oid range
	 */
	OidxRange(const Oidx&, const Oidx&);

	/**
	 * Copy constructor
	 */
	OidxRange(const OidxRange&);

	/**
	 * Destructor
	 */
	virtual ~OidxRange();

	/**
	 * Clone
	 *
	 * @return
	 *    a pointer to a clone of the receiver.
	 */
	virtual OidxRange* clone() const;

	/**
	 * Compare the receiver with another OID range for equality
	 *
	 * @param other
	 *    another OidxRange instance
	 * @return
	 *    TRUE if lower and upper bounds of both OID ranges are equal
	 */
	virtual bool	operator==(const OidxRange&) const;
	/**
	 * Compare the receiver with another OID range
	 *
	 * @param other
	 *    another OidxRange instance
	 * @return
	 *    TRUE if the upper bound of the receiver is less or equal to the
	 *    lower bound of other
	 */
	virtual bool	operator<(const OidxRange&) const;
	/**
	 * Compare the receiver with another OID range
	 *
	 * @param other
	 *    another OidxRange instance
	 * @return
	 *    TRUE if the lower bound of the receiver is greater or equal to
	 *    the upper bound of other
	 */
	virtual bool	operator>(const OidxRange&) const;

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
	 *    TRUE if oid is greater or equal the receiver's lower
	 *    bound and less than its upper bound.
	 */
	virtual bool	includes_excl(const Oidx&) const;

	/**
	 * Check whether a range is covered by the receiver's range
	 *
	 * @param range
	 *    a OidxRange instance
	 * @return
	 *    TRUE if the lower bound of range is greater or equal the
	 *    receiver's lower bound and if the upper bound of range is
	 *    less or equal the upper bound of the receiver, FALSE otherwise
	 */
	virtual bool	covers(const OidxRange&) const;

	/**
	 * Check whether a range overlaps the receiver's range
	 *
	 * @param range
	 *    a OidxRange instance
	 * @return
	 *    TRUE if at least one oid is part of both ranges, FALSE otherwise
	 */
	virtual bool	overlaps(const OidxRange&) const;

	/**
	 * Get lower bound
	 *
	 * @return
	 *    the lower bound object identifier
	 */
	virtual Oidx	get_lower() const;

	/**
	 * Get upper bound
	 *
	 * @return
	 *    the upper bound object identifier
	 */
	virtual Oidx	get_upper() const;

	Oidx lower, upper;
};


/*--------------------------- class Pdux -----------------------------*/

/**
 * The Pdux class is a sub class of Pdu that provides additional
 * methods for manipulating SNMP PDUs.
 * AGENT++ uses Pdux instead of Pdu, because Pdu does not allow to set
 * error status, error index and request id of a PDU.
 *
 * @version 3.4
 * @author Frank Fock
 */


class AGENTPP_DECL Pdux: public NS_SNMP Pdu {
public:
	Pdux(): Pdu() { }
	Pdux(NS_SNMP Vb* pvbs, const int pvb_count): Pdu(pvbs, pvb_count) { }
	Pdux(const Pdu& pdu): Pdu(pdu) { }
	Pdux(const Pdux& pdu): Pdu(pdu) { }

	virtual ~Pdux() { }

	/**
	 * Clear the Pdu contents (destruct and construct in one go)
	 */
	void    clear();

	// const redefinitions of originals: 
	Pdux&   operator+=(const NS_SNMP Vb&);

	/**
	 * Clone the receiver.
	 *
	 * @return
	 *    a pointer to a copy of the receiver.
	 */
	virtual Pdux*   clone() const { return new Pdux(*this); }
};


/*--------------------------- class Snmpx -----------------------------*/

/**
 * The Snmpx class is a sub class of Snmp that provides additional
 * methods for sending and receiving SNMP messages.
 * AGENT++ uses Snmpx instead of Snmp, because Snmp does not allow to
 * listen on an UDP port for incoming SNMP requests.
 *
 * @version 3.4.4
 * @author Frank Fock
 */

class AGENTPP_DECL Snmpx: public NS_SNMP Snmp {
public:
	/**
	 * Construct a new SNMP session using the given UDP port.
	 *
	 * @param status
	 *    after creation of the session this parameter will
	 *    hold the creation status.
	 * @param port
	 *    an UDP port to be used for the session
	 */
	Snmpx (int &status , u_short port): Snmp(status, port) {};

#ifdef SNMP_PP_WITH_UDPADDR
	/**
	 * Construct a new SNMP session using the given UDP address.
	 * Thus, binds the session on a specific IP address.
	 *
	 * @param status
	 *    after creation of the session this parameter will
	 *    hold the creation status.
	 * @param address
	 *    an UDP address to be used for the session
	 */
	Snmpx(int& status, const NS_SNMP UdpAddress& addr): Snmp(status, addr) { }
#endif

#ifdef _SNMPv3
	/**
	 * Receive a SNMP PDU
	 *
	 * @param timeout
	 *    wait for an incoming PDU until timeout is exceeded
	 * @param pdu
	 *    will contain the received SNMP PDU
	 * @param target
         *    the target (either CTarget or UTarget) will contain
	 *    - the UDP address of the sender of the received PDU
	 *    - the SNMP version of the received PDU
         *    - UTarget: security_model(community), security_name and
         *               (authoritative) engine_id
	 * @return
	 *   SNMP_CLASS_SUCCESS on success and SNMP_CLASS_ERROR,
	 *   SNMP_CLASS_TL_FAILED on failure.
	 */
	int receive(struct timeval*, Pdux&, NS_SNMP UTarget&);
#else
	/**
	 * Receive a SNMP PDU
	 *
	 * @param timeout
	 *    wait for an incoming PDU until timeout is exceeded
	 * @param pdu
	 *    will contain the received SNMP PDU
	 * @param form_address
	 *    the UDP address of the sender of the received PDU
	 * @param version
	 *    will contain the SNMP version of the received PDU
	 * @param community
	 *   will conatin the community (in case of SNMPv3 the
	 *   security information) of the received PDU
	 * @return
	 *   SNMP_CLASS_SUCCESS on success and SNMP_CLASS_ERROR,
	 *   SNMP_CLASS_TL_FAILED on failure.
	 */
	int receive(struct timeval*, Pdux&, NS_SNMP UdpAddress&,
		    NS_SNMP snmp_version&, NS_SNMP OctetStr&);
#endif
#ifdef _SNMPv3
	/**
	 * Send o SNMP PDU
	 *
	 * @param pdu
	 *    the SNMP PDU to send
	 * @param target
	 *    the target (actually a CTarget or UTarget) that contains
         *      - the receiver's UDP address
	 *      - the SNMP version to be used
         *      - CTarget: the community
         *      - UTarget: security_model, security_name and
         *                 (if known) engine_id
	 * @return
	 *   SNMP_CLASS_SUCCESS on success and SNMP_CLASS_ERROR,
	 *   SNMP_CLASS_TL_FAILED on failure.
	 */
        int send (Pdux const &, NS_SNMP SnmpTarget*);
#else
	/**
	 * Send o SNMP PDU
	 *
	 * @param pdu
	 *    the SNMP PDU to send
	 * @param address
	 *    the receiver's UDP address
	 * @param version
	 *    the SNMP version to be used
	 * @param community
	 *    the community / security information to be used
	 * @return
	 *   SNMP_CLASS_SUCCESS on success and SNMP_CLASS_ERROR,
	 *   SNMP_CLASS_TL_FAILED on failure.
	 */
        int send (Pdux const &, NS_SNMP UdpAddress const &, NS_SNMP snmp_version, NS_SNMP OctetStr const &);
#endif
	/**
	 * Get the port the request list is listening on.
	 *
	 * @return
	 *    a UDP port.
	 */
	u_short get_port();

	/**
	 * Return the socket descriptor of the socket used for
	 * incoming SNMP request. This is the socket the select()
	 * of the receive method listens on. This socket descriptor
	 * may be used to be included in other select controlled
	 * polling loops.
	 *
	 * @return
	 *    a socket descriptor.
	 */
	SnmpSocket	get_session_fds() { return iv_snmp_session; }


protected:
	unsigned long ProcessizedReqId(unsigned short);
	unsigned long MyMakeReqId();
};

#ifdef AGENTPP_NAMESPACE
}
#endif

#endif







