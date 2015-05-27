/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - mib_complex_entry.h  
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


#ifndef _mib_complex_entry_h_
#define _mib_complex_entry_h_

#include <agent_pp/mib_entry.h>
#include <agent_pp/mib.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

/*------------------------ class MibComplexEntry ------------------------*/

/**
 * The MibComplexEntry class is an abstract class that represents complex
 * MIB nodes. Complex nodes access their managed objects (e.g., MibLeafs) 
 * via an implementation specific way. This interface can be used to 
 * implement light weight tables, i.e. tables that access their management
 * information through a proprietary protocol (other than SNMP or AgentX).
 *
 * @author Frank Fock
 * @version 3.4
 */
class AGENTPP_DECL MibComplexEntry: public MibEntry {
friend class Mib;
public:
	/**
	 * Construct a MibComplexEntry instance with a given 
	 * object identifier and maximum access rights.
	 * 
	 * @param oid
	 *    an object identifier.
	 * @param access
	 *    the maximum access rights for the receiver.
	 */   
	MibComplexEntry(const Oidx&, mib_access);

	/**
	 * Copy constructor.
	 * 
	 * @param other
	 *    another MibComplexEntry instance.
	 */
	MibComplexEntry(const MibComplexEntry&);

	/**
	 * Destructor
	 */
	virtual	~MibComplexEntry();


	/**
	 * Return the type of the receiver MIB node.
	 *
	 * @return 
	 *    One of the following types: AGENTPP_NONE, 
	 *    AGENTPP_PROXY, AGENTPP_LEAF, 
	 *    AGENTPP_TABLE, AGENTPP_GROUP, or AGENTPP_COMPLEX.
	 */
	virtual mib_type	type() const { return AGENTPP_COMPLEX; }

	/**
	 * Return a clone of the receiver.
	 *
	 * @return
	 *    a pointer to a clone of the MibComplexEntry object.  
	 */
	virtual MibEntry*	clone() = 0;

	/**
	 * Return the successor of a given object identifier within the 
	 * receiver's scope and the context of a given Request.
	 *
	 * @param oid
	 *    an object identifier
	 * @param request
	 *    a pointer to a Request instance.
	 * @return 
	 *    an object identifier if a successor could be found,
	 *    otherwise (if no successor exists or is out of scope) 
	 *    a zero length oid is returned
	 */
	virtual Oidx		find_succ(const Oidx&, Request* req = 0) = 0;

 	// interfaces dispatch table <-> management instrumentation
	
	/**
	 * Let the receiver process a SNMP GET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP GET request.
	 * @param ind - The index of the subrequest to be processed.
	 */
	virtual void		get_request(Request*, int) = 0;

	/**
	 * Let the receiver process a SNMP GETNEXT subrequest
	 * 
	 * @param req - A pointer to the whole SNMP GETNEXT request.
	 * @param ind - The index of the subrequest to be processed.
	 */
	virtual void		get_next_request(Request*, int) = 0;
	
	/**
	 * Let the receiver commit a SNMP SET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP SET request.
	 * @param ind - The index of the subrequest to be processed.
	 * @return SNMP_ERROR_SUCCESS on success and 
	 *         SNMP_ERROR_COMITFAIL on failure.
	 */
	virtual int		commit_set_request(Request*, int) 
				  { return SNMP_ERROR_COMITFAIL; }

	/**
	 * Let the receiver prepare a SNMP SET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP SET request.
	 * @param ind - The index of the subrequest to be processed.
	 * @return SNMP_ERROR_SUCCESS on success and SNMP_ERROR_WRONG_VALUE,
	 *         SNMP_ERROR_WRONG_TYPE, or 
	 *         SNMP_ERROR_NOT_WRITEABLE on failure.
	 */
	virtual int		prepare_set_request(Request*, int&) 
				  { return SNMP_ERROR_NOT_WRITEABLE; }

	/**
	 * Let the receiver undo a SNMP SET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP SET request.
	 * @param ind - The index of the subrequest to be processed.
	 * @return SNMP_ERROR_SUCCESS on success and 
	 *         SNMP_ERROR_UNDO_FAIL on failure.
	 */
	virtual int	        undo_set_request(Request*, int&) 
				  { return SNMP_ERROR_SUCCESS; }

	/**
	 * Clean up resources used for performing (or undoing) set requests. 
	 *
	 * @param req - A pointer to the whole SNMP SET request.
	 * @param ind - The index of the subrequest to be processed.
	 */
	virtual void	        cleanup_set_request(Request*, int&) { }

	/**
	 * Serialize the value of the receiver.
	 * 
	 * @param buf - A pointer to byte stream buffer returned.
	 * @param sz - The size of the buffer returned.
	 * @return TRUE if serialization was successful, FALSE otherwise.
	 */
	virtual bool      	serialize(char*&, int&) { return FALSE; }

	/**
	 * Read the value of the receiver from a byte stream.
	 * 
	 * @param buf 
	 *    a pointer to the input byte stream.
	 * @param sz
	 *    the size of the input buffer. On output it contains the
	 *    size remaining unread in the input buffer.
	 * @return 
	 *    TRUE if deserialization was successful, FALSE otherwise.
	 */
	virtual bool      	deserialize(char*, int&) { return FALSE; }

	/**
	 * Check whether the receiver node contains any instance of a
	 * managed object.
	 *
	 * @return TRUE if the node currently manages an instance.
	 */  
	virtual bool		is_empty() { return FALSE; }


	// communication between mib objects

	/**
	 * Return the immediate successor of the greatest object identifier 
	 * within the receiver's scope
	 * 
	 * @return 
	 *    a pointer to the non including upper bound of the receiver's 
	 *    scope.
	 */
	virtual OidxPtr      	max_key() { return &upperBound; }

protected:
	Oidx			upperBound;
};


/*------------------------ class MibStaticEntry ------------------------*/

/**
 * The MibStaticEntry class represents an entry (instance) within a
 * MibStaticTable.
 *
 * @author Frank Fock
 * @version 3.4 
 */ 

class AGENTPP_DECL MibStaticEntry: public Vbx {
 public:
	MibStaticEntry(const Vbx& v): Vbx(v) { }
	MibStaticEntry(const Oidx& o, const NS_SNMP SnmpSyntax& v): Vbx(o)
	  { set_value(v); } 
	MibStaticEntry(const MibStaticEntry& other): Vbx(other) { }

	OidxPtr		key() { return (Oidx*)&iv_vb_oid; }
};

/*------------------------ class MibStaticTable ------------------------*/

/**
 * The MibStaticTable class is a light weight table implementation for
 * static (read-only) tables and scalar groups. 
 * This class may be used as an example showing how 
 * the abstract class MibComplexEntry can be extended by subclassing
 * to implement tables that access their management information through 
 * a proprietary protocol (other than SNMP or AgentX).
 *
 * Caution: From version 3.5.2 on, MibStaticEntries added to a 
 * MibStaticTable should contain their respective OID suffix to
 * the table. The scalar group snmpGroup would thus be defined as 
 * follows:
 *
 * MibStaticTable* st = new MibStaticTable("1.3.6.1.2.1.11");
 * st->add(new MibStaticTable("1.0", Counter32(0))); // snmpInPkts
 * st->add(new MibStaticTable("2.0", Counter32(0))); // snmpOutPkts
 * st->add(new MibStaticTable("3.0", Counter32(0))); // snmpInBadVersions
 * 
 * The old format where the full OID had to be specified is still
 * support for compatibility reasons but should no longer be used,
 * because it might be removed in future versions!
 * 
 * @author Frank Fock
 * @version 3.5.2
 */
#if !defined (AGENTPP_DECL_TEMPL_OIDLIST_MIBSTATICENTRY)
#define AGENTPP_DECL_TEMPL_OIDLIST_MIBSTATICENTRY
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL OidList<MibStaticEntry>;
#endif

class AGENTPP_DECL MibStaticTable: public MibComplexEntry {
 public:
	/**
	 * Construct a static table (entry) instance with a given 
	 * object identifier. 
	 * 
	 * @param oid
	 *    an object identifier.
	 */   
	MibStaticTable(const Oidx&);

	/**
	 * Copy constructor.
	 * 
	 * @param other
	 *    another MibComplexEntry instance.
	 */
	MibStaticTable(MibStaticTable&);

	/**
	 * Destructor
	 */
	virtual ~MibStaticTable();

	/**
	 * Return a clone of the receiver.
	 *
	 * @return
	 *    a pointer to a clone of the MibStaticTable object.  
	 */
	virtual MibEntry*	clone() { return new MibStaticTable(*this); }

	/**
	 * Add an instance to the table. If such an instance already
	 * exists, it will be removed. (SYNCHRONIZED)
	 *
	 * @param instance
	 *    a MibStaticEntry instance.
	 */
	virtual void		add(const MibStaticEntry&);

	/**
	 * Remove an instance from the table. (SYNCHRONIZED)
	 *
	 * @param oid
	 *    the object ID of the entry to be removed.
	 */
	virtual void		remove(const Oidx&);

	/**
	 * Get the entry instance with the given OID. If suffixOnly 
	 * is FALSE (the default), the specified OID must be the full 
	 * OID of the entry, including the OID prefix from the 
	 * MibStaticTable. (NOT SYBCHRONIZED)
	 *
	 * @param oid
	 *    the OID (or OID suffix) of the requested entry.
	 * @param suffixOnly
	 *    determines whether the given OID should be interpreted
	 *    as suffix appended to the table's OID or whether the 
	 *    given OID fully specifies the requested entry.
	 * @return
	 *    the entry with the given OID or 0 if such an object does
	 *    not exist.
	 */
	virtual MibStaticEntry* get(const Oidx&, bool suffixOnly=FALSE);

	/**
	 * Return the successor of a given object identifier within the 
	 * receiver's scope and the context of a given Request.
	 *
	 * @param oid
	 *    an object identifier
	 * @param request
	 *    a pointer to a Request instance.
	 * @return 
	 *    an object identifier if a successor could be found,
	 *    otherwise (if no successor exists or is out of scope) 
	 *    a zero length oid is returned
	 */
	virtual Oidx		find_succ(const Oidx&, Request* req = 0);

	/**
	 * Let the receiver process a SNMP GET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP GET request.
	 * @param ind - The index of the subrequest to be processed.
	 */
	virtual void		get_request(Request*, int);

	/**
	 * Let the receiver process a SNMP GETNEXT subrequest
	 * 
	 * @param req - A pointer to the whole SNMP GETNEXT request.
	 * @param ind - The index of the subrequest to be processed.
	 */
	virtual void		get_next_request(Request*, int);
	
 protected:
	
	OidList<MibStaticEntry>		contents;
};
#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
