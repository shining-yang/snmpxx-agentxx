/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - mib_entry.h  
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

#ifndef mib_entry_h_
#define mib_entry_h_


#include <agent_pp/agent++.h>

#include <agent_pp/snmp_pp_ext.h>
#include <agent_pp/oidx_ptr.h>
#include <agent_pp/threads.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

typedef enum { NOACCESS, READONLY, READWRITE, READCREATE } mib_access; 

typedef enum { AGENTPP_NONE, AGENTPP_PROXY, AGENTPP_LEAF, AGENTPP_TABLE, 
	       AGENTPP_GROUP, AGENTPP_COMPLEX, 
	       AGENTX_NODE, AGENTX_LEAF } mib_type;

#define REMOVE	0x01
#define CREATE  0x02
#define CHANGE  0x04
#define UPDATE  (REMOVE | CREATE | CHANGE)
 


typedef unsigned char mib_change;

/*--------------------------- class MibEntry --------------------------*/


class Mib;

/**
 * The MibEntry class is an abstract class that represents MIB nodes.
 * Every entry in an AGENT++ Mib instance has to be derived from MibEntry.
 * MibEntry provides interfaces for message processing, object persistency,
 * and node handling.
 *
 * @author Frank Fock
 * @version 3.5.22
 */
#if !defined (AGENTPP_DECL_TEMPL_LIST_MIBENTRY)
#define AGENTPP_DECL_TEMPL_LIST_MIBENTRY
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL List<MibEntry>;
#endif

class AGENTPP_DECL MibEntry: public ThreadManager {
friend class Mib;
public:
	/**
	 * Default constructor.
	 */ 
	MibEntry();

	/**
	 * Construct a MibEntry instance with a given object identifier
	 * and maximum access rights.
	 * 
	 * @param o - An object identifier.
	 * @param a - The maximum access of the receiver.
	 */   
	MibEntry(const Oidx&, mib_access);

	/**
	 * Copy constructor.
	 * 
	 * @param other - A MibEntry object.
	 */
	MibEntry(const MibEntry&);

	/**
	 * Destructor
	 */
	virtual		~MibEntry();


	/**
	 * Return the type of the receiver MIB node.
	 *
	 * @return 
	 *    One of the following types: AGENTPP_NONE, 
	 *    AGENTPP_PROXY, AGENTPP_LEAF, 
	 *    AGENTPP_TABLE, AGENTPP_GROUP, or AGENTPP_COMPLEX.
	 */
	virtual mib_type	type() const;

	/**
	 * Return a clone of the receiver.
	 *
	 * @return A pointer to a clone of the MibEntry object.  
	 */
	virtual MibEntry*	clone();

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
	virtual Oidx		find_succ(const Oidx&, Request* req = 0) 
						{ (void)req; return Oidx(); }

 	// interfaces dispatch table <-> management instrumentation
	
	/**
	 * Let the receiver process a SNMP GET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP GET request.
	 * @param ind - The index of the subrequest to be processed.
	 */
	virtual void		get_request(Request*, int) { }

	/**
	 * Let the receiver process a SNMP GETNEXT subrequest. The
	 * OID of the subrequest identified by the given index is
	 * already set to the next OID found. In other words, the
	 * OID of the current subrequest is different from the OID
	 * in the original request. So the implementation of this
	 * method does not need to search the appropriate "next" OID.
	 * 
	 * @param req - A pointer to the whole SNMP GETNEXT request.
	 * @param ind - The index of the subrequest to be processed.
	 */
	virtual void		get_next_request(Request*, int) { }
	
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
				  { return SNMP_ERROR_NO_SUCH_NAME; }

	/**
	 * Clean up resources used for performing (or undoing) set requests. 
	 *
	 * @param req - A pointer to the whole SNMP SET request.
	 * @param ind - The index of the subrequest to be processed.
	 */
	virtual int	        undo_set_request(Request*, int&) 
				  { return SNMP_ERROR_SUCCESS; }

	/**
	 * Set the receiver's value and backup its old value for a later undo. 
	 *
	 * @param vb - The variable binding that holds the new value.
	 * @return SNMP_ERROR_SUCCESS if the new value has been set,
	 *         SNMP_ERROR_WRONG_TYPE or SNMP_ERROR_BAD_VALUE otherwise.
	 */
	virtual void	        cleanup_set_request(Request*, int&) { }


	// load/save mib objects from/to hard disk

	/**
	 * Load the value(s) of the receiver node from a file.
	 *
	 * @param fname - A file name.
	 */ 
	virtual void		load_from_file(const char*);

	/**
	 * Save the value(s) of the receiver node to a file.
	 *
	 * @param fname - A file name.
	 */
	virtual void		save_to_file(const char*);

	/**
	 * Serialize the value of the receiver.
	 * 
	 * @param buf - A pointer to byte stream buffer returned.
	 * @param sz - The size of the buffer returned.
	 * @return TRUE if serialization was successful, FALSE otherwise.
	 */
	virtual bool      	serialize(char*&, int&);

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
	virtual bool      	deserialize(char*, int&);

	/**
	 * Check whether the receiver node contains any instance of a
	 * managed object.
	 *
	 * @return TRUE if the node currently manages an instance.
	 */  
	virtual bool		is_empty() { return FALSE; }


	// communication between mib objects

	/**
	 * Notify all registered nodes of changes to an object
	 * managed by the receiver node.
	 *
	 * @param o - The object identifier of the object changed.
	 * @param change - The type of the change (REMOVE, CREATE, CHANGE,
	 *                 or UPDATE)
	 */
	virtual void		notify_change(const Oidx&, mib_change);

	/**
	 * Register an MibEntry object to receive notifications about
	 * changes of the receiver node.
	 *
	 * @param entry - A MibEntry to receive notifications.
	 */  
	virtual void		register_for_notifications(MibEntry*);

	/**
	 * Wrapper function for register_for_notifications.
	 * @depricated
	 */
	virtual void		add_change_notification(MibEntry* e)
					{ register_for_notifications(e); }
	/**
	 * Receive a notification about changes to a managed object. This
	 * method will be called for all registered objects of an object
	 * that called notify_change().
	 *
	 * @param o - The object identifier of the object changed.
	 * @param change - The type of the change (REMOVE, CREATE, CHANGE,
	 *                 or UPDATE)
	 */
	virtual void		change_notification(const Oidx&, mib_change) {}


	/**
	 * Return a pointer to the key (object identifier) of the receiver.
	 *
	 * @return A pointer to an object identifier.
	 */
	OidxPtr			key();

	/**
	 * Return the immediate successor of the greatest object identifier 
	 * within the receiver's scope
	 * 
	 * @return 
	 *    a pointer to the non including upper bound of the receiver's 
	 *    scope
	 */
	virtual OidxPtr      	max_key();

	/**
	 * Return the maximum access rights for the managed object
	 * represented by the receiver node.
	 * 
	 * @return The maximum access (one of the following values:
	 *         NOACCESS, READONLY, READWRITE, or READCREATE)
	 */
	virtual mib_access     	get_access();

	/**
	 * Check whether the entry is volatile or not.
	 *
	 * @return
	 *    TRUE if the entry contains volatile data, FALSE otherwise.
	 */
	virtual bool		is_volatile();

	/**
	 * Resets (clears) the content of this entry.
	 *
	 * @since 3.5.22 
	 */
	virtual void		reset() { }


	/**
	 * @name comparison operators 
	 */
	//@{
	int			operator<(const MibEntry&) const;
	int			operator>(const MibEntry&) const;
	int			operator==(const MibEntry&) const;
	
	int			operator<(const Oidx&) const;
	int			operator>(const Oidx&) const;
	int			operator<=(const Oidx&) const;
	int			operator>=(const Oidx&) const;
	int			operator==(const Oidx&) const;
	//@}

protected:

	/**
	 * Set the object identifier of the receiver node.
	 * 
	 * @param o - An object identifier.
	 */
	void			set_oid(const Oidx&);

	//	Oidx			oid;
	Oidx			oid;
	mib_access		access;
	List<MibEntry>		notifies;
	
};

typedef MibEntry* MibEntryPtr;

#ifdef AGENTPP_NAMESPACE
}
#endif


#endif

