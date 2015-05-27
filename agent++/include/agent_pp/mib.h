/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - mib.h  
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


#ifndef mib_h_
#define mib_h_

#include <libagent.h>

#include <agent_pp/agent++.h>
#include <agent_pp/List.h>
#include <agent_pp/threads.h>
#include <agent_pp/mib_avl_map.h>
#include <agent_pp/snmp_pp_ext.h>
#include <agent_pp/mib_entry.h>
#include <agent_pp/oidx_ptr.h>
#include <agent_pp/tools.h>
#include <agent_pp/request.h>
#include <agent_pp/mib_context.h>

#ifdef _USE_PROXY
#include <agent_pp/proxy_forwarder.h>
#endif

#define DEFAULT_PATH_PERSISTENT_MO	"config/"
#define VARIABLE_INDEX_LENGTH		0

#define DEFAULT_ROW_CREATION_TIMEOUT	300

#define VMODE_NONE			0
#define VMODE_DEFAULT			1
#define VMODE_LOCKED			2

#define LEAF_VALUE_INITIALIZED          1
#define LEAF_VALUE_SET                  2

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

/*--------------------------- class MibLeaf --------------------------*/

class AGENTPP_DECL MibTableRow;
class AGENTPP_DECL MibTable;

/**
 * An instance of the class MibLeaf represents a leaf object in the
 * managed object registration tree. MibLeaf objects can be used to model
 * scalar managed objects as well as objects of SNMP tables.  The objects
 * of a table are defined by defining their corresponding columnar
 * objects. The instances of each columnar object (which is itself not
 * accessible) are then cloned from it.
 *
 * In order to save memory space, from version 3.4.7 on the key()
 * method does no longer return the complete OID (instance OID) of
 * columnar MibLeaf objects (thus MibLeaf objects that are part of
 * a table). Instead, MibLeaf::key() only returns the last subid
 * that identifies the column the MibLeaf is part of. From v3.4.7
 * on, there is the get_oid() method that returns the entiry OID
 * value for columnar objects as well as for other MibLeaf objects.
 * For columnar objects this method computes the OID value from the
 * table's OID plus the MibLeaf's column (key()), and the row's 
 * index value.
 * 
 * MibLeaf is a sub-class of the abstract MibEntry class. As MibLeaf
 * represents a managed object instance a MibLeaf object contains
 * management information. So each MibLeaf object contains a pointer
 * "value" to that management information which can be any object derived
 * from SnmpSyntax. 
 * 
 * @author Frank Fock
 * @version 3.4.7
 */

class AGENTPP_DECL MibLeaf: public MibEntry {
friend class MibTable;
friend class MibTableRow;
public:

  /**
   * Default constructor.
   */
  MibLeaf();

  /**
   * Construct a MibLeaf object.
   *
   * @param o - The object identifier of the managed object. If it is part
   *            of a table, only the column subidentifier is given. If it
   *            is a scalar object the object identifier given includes the
   *            full registration oid including the suffix ".0".
   * @param a - The maximum access rights for the managed object.
   * @param s - The syntax of the managed object.
   * @see Oidx, mib_access
   */
  MibLeaf(const Oidx&, mib_access, NS_SNMP SmiUINT32);

  /**
   * Construct a MibLeaf object.
   *
   * @param o - The object identifier of the managed object. If it is part
   *            of a table, only the column subidentifier is given. If it
   *            is a scalar object the object identifier given includes the
   *            full registration oid including the suffix ".0".
   * @param a - The maximum access rights for the managed object.
   * @param s - A pointer to the SnmpSyntax object to be used for storing
   *            the value of the managed object. NOTE: Do not delete this
   *            pointer from outside MibLeaf. It will be deleted by ~MibLeaf
   * @see Oidx, mib_access
   */
  MibLeaf(const Oidx&, mib_access, NS_SNMP SnmpSyntax*);

  /**
   * Construct a MibLeaf object.
   *
   * @param oid
   *    the object identifier of the managed object. If it is part
   *    of a table, only the column subidentifier is given. If it
   *    is a scalar object the object identifier given includes the
   *    full registration oid including the suffix ".0".
   * @param access
   *    the maximum access rights for the managed object.
   * @param value
   *    a pointer to the SnmpSyntax object to be used for storing
   *    the value of the managed object. NOTE: Do not delete this
   *    pointer from outside MibLeaf. It will be deleted by ~MibLeaf
   * @param valueMode
   *    contains an ored value of one or more of the following modes:
   *    VMODE_DEFAULT: the value of the object pointed by value will be used
   *                   as default value, otherwise the managed object has 
   *                   no default value.
   *    VMODE_LOCKED:  value is locked (cannot be modified) while
   *                   the row status of the receiver's row is active(1).
   *                   This mode has no effect, if the the leaf is a
   *                   scalar or otherwise if the receiver's row has no
   *                   snmpRowStatus.  
   */
  MibLeaf(const Oidx&, mib_access, NS_SNMP SnmpSyntax*, int);

  /**
   * Copy Constructor.
   */
  MibLeaf(const MibLeaf&);

  /**
   * Destructor.
   */
  virtual ~MibLeaf();

  /**
   * Initialize a MibLeaf object. This method is typically called either
   * from a constructor with value or when loading a value from persistent
   * storage.
   *
   * @param initialValue
   *    a pointer to a SnmpSyntax object which will hold the value
   *    of this MibLeaf object.
   * @param valueMode
   *    contains an ored value of one or more of the following modes:
   *    VMODE_DEFAULT: the value of the object pointed by value will be used
   *                   as default value, otherwise the managed object has 
   *                   no default value.
   *    VMODE_LOCKED:  value is locked (cannot be modified) while
   *                   the row status of the receiver's row is active(1).
   *                   This mode has no effect, if the the leaf is a
   *                   scalar or otherwise if the receiver's row has no
   *                   snmpRowStatus.  
   */
  virtual void        	init(NS_SNMP SnmpSyntax*, int);

  /**
   * Return the type of the receiver.
   *
   * @return LEAF
   */
  virtual mib_type     	type() const;

  /**
   * Clone the receiver object.
   * 
   * @return A pointer to the clone.
   */
  virtual MibEntryPtr	clone();


  /**
   * Serialize the value of the receiver.
   * 
   * @param buf - A pointer to byte stream buffer returned.
   * @param sz - The size of the buffer returned.
   * @return TRUE if serialization was successful, FALSE otherwise.
   */
  virtual bool	serialize(char*&, int&);

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
  virtual bool	deserialize(char*, int&);
  

  /**
   * Get the syntax of the receiver's value.
   *
   * @return The syntax of a MibLeaf object's value or sNMP_SYNTAX_NULL,
   *         if the object has no value. 
   */ 
  virtual NS_SNMP SmiUINT32	get_syntax() const; 

  /**
   * Get the value of the receiver.
   *
   * @return A variable binding that holds the object identifier and the 
   *         value of the receiver. 
   */
  virtual Vbx		get_value() const;
  /**
   * Get the value of the receiver.
   * 
   * @param s - A reference to a SnmpSyntax object to hold a copy of 
   *            the receiver's value.  
   * @return SNMP_ERROR_SUCCESS on success.
   */
  virtual int		get_value(NS_SNMP SnmpSyntax& s) const 
					 { return get_value().get_value(s); }
  /**
   * Get the value of the receiver.
   * 
   * @param i - A reference to an int the receiver's value will be copied in.  
   * @return SNMP_ERROR_SUCCESS on success.
   */
  virtual int		get_value(int& i) const   
					 { return get_value().get_value(i); }
  /**
   * Get the value of the receiver.
   * 
   * @param i - A reference to a long the receiver's value will be copied in.  
   * @return SNMP_ERROR_SUCCESS on success.
   */
  virtual int		get_value(long& i) const 
					 { return get_value().get_value(i); }
  /**
   * Get the value of the receiver.
   * 
   * @param i - A reference to an unsigned long the receiver's value 
   *            will be copied in.  
   * @return SNMP_ERROR_SUCCESS on success.
   */
  virtual int		get_value(unsigned long& i) const 
				         { return get_value().get_value(i); }
  /**
   * Get the value of the receiver.
   * 
   * @param c - A pointer to a string the receiver's value will be copied in.  
   * @return SNMP_ERROR_SUCCESS on success.
   */
  virtual int		get_value(char* c) const 
					 { return get_value().get_value(c); }

  /**
   * Set the value of the receiver by value.
   *
   * @param v - The value.
   */ 
  virtual void		set_value(const NS_SNMP SnmpSyntax&);

  /**
   * Set the unsigned integer value of the receiver. Caution: The syntax
   * of this MibLeaf instance will be set to sNMP_SYNTAX_INT32! Thus, it
   * should be only used for objects with an INTEGER syntax.
   *
   * @param l - The new integer value.
   * @deprecated Use set_value(const SnmpSyntax&) instead.
   */ 
  virtual void		set_value(const unsigned long);

  /**
   * Set the value of the receiver from a variable binding and checks
   * for validity of the variable binding and compatible syntax and oid.
   *
   * @param vb - The variable binding including the new value.
   * @return SNMP_ERROR_SUCCESS if the new value has been set,
   *         SNMP_ERROR_WRONG_TYPE or SNMP_ERROR_BAD_VALUE otherwise.
   */ 
  virtual int		set_value(const Vbx&);
  /**
   * Set the value of the receiver by reference.
   *
   * @param v - A pointer to the new value.
   */ 
  virtual void		replace_value(NS_SNMP SnmpSyntax* s);


  /**
   * Return the immediate successor of the greatest object identifier 
   * within the receiver's scope
   * 
   * @return 
   *    a pointer to the non including upper bound of the receiver's 
   *    scope
   */
  virtual OidxPtr	max_key()	{ return key(); }


  /**
   * Interface methods dispatch table <-> instrumentation
   */

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

  /**
   * Let the receiver commit a SNMP SET subrequest
   * 
   * @param req - A pointer to the whole SNMP SET request.
   * @param ind - The index of the subrequest to be processed.
   * @return SNMP_ERROR_SUCCESS on success and SNMP_ERROR_COMITFAIL on failure.
   */
  virtual int		commit_set_request(Request*, int);

  /**
   * Let the receiver prepare a SNMP SET subrequest
   * 
   * @param req - A pointer to the whole SNMP SET request.
   * @param ind - The index of the subrequest to be processed.
   * @return SNMP_ERROR_SUCCESS on success and SNMP_ERROR_WRONG_VALUE,
   *         SNMP_ERROR_WRONG_TYPE, or SNMP_ERROR_NOT_WRITEABLE on failure.
   */
  virtual int		prepare_set_request(Request*, int&);

  /**
   * Let the receiver undo a SNMP SET subrequest
   * 
   * @param req - A pointer to the whole SNMP SET request.
   * @param ind - The index of the subrequest to be processed.
   * @return SNMP_ERROR_SUCCESS on success and SNMP_ERROR_UNDO_FAIL on failure.
   */
  virtual int		undo_set_request(Request*, int&);

  /**
   * Clean up resources used for performing (or undoing) set requests. 
   *
   * @param req - A pointer to the whole SNMP SET request.
   * @param ind - The index of the subrequest to be processed.
   */
  virtual void		cleanup_set_request(Request*, int&);


  /**
   * Check whether the receiver's value may be set to the given new value.
   * @note Override this mehtod in subclasses.
   *
   * @param v - A new value.
   * @return TRUE if the new value is valid, otherwise FALSE.
   */
  virtual bool	value_ok(const Vbx&)	{ return TRUE; }

  /**
   * Set the receiver's value and backup its old value for a later undo. 
   * @note Override this mehtod in subclasses.
   *
   * @param vb - The variable binding that holds the new value.
   * @return SNMP_ERROR_SUCCESS if the new value has been set,
   *         SNMP_ERROR_WRONG_TYPE or SNMP_ERROR_BAD_VALUE otherwise.
   */
  virtual int	set(const Vbx& vb);
  /** 
   * Undo a previous set.
   *
   * @return SNMP_ERROR_SUCCESS on success and SNMP_ERROR_UNDO_FAIL on failure.
   */
  virtual int	unset();


  /**
   * Return a pointer to the table the receiver belongs to.
   *
   * @return A Pointer to the table the receiver is in, if the receiver is a
   *         scalar managed object return 0.
   */
  MibTable*	get_reference_to_table() { return my_table; }
  /**
   * Return a pointer to the row the receiver belongs to.
   *
   * @return A Pointer to the row the receiver is in, if the receiver is a
   *         scalar managed object return 0.
   */
  MibTableRow*	get_reference_to_row()   { return my_row; }

  /**
   * Gets the flags (bits) that determine the type/stage of validity
   * of the leaf's value. If any bit is set the leaf is supposed to
   * contain a valid value. AGENT++ only uses the first 4 bits (0-3).
   * The other bits can be used by the API programmer to track internal
   * states of the leaf.
   * 
   * @return 
   *    an integer value != 0 if the receiver has a valid value.
   */
  int		valid()			{ return validity; }

  /**
   * Return whether the receiver has a default value.
   *
   * @return TRUE if the receiver has a default value, FALSE otherwise.
   */
  bool	has_default()	       	{ return ((value_mode & VMODE_DEFAULT) > 0);}

  /**
   * Return whether the receiver has a default value.
   *
   * @return TRUE if the receiver has a default value, FALSE otherwise.
   */
  bool	is_locked()	       	{ return ((value_mode & VMODE_LOCKED) > 0); }

  /**
   * Set the receiver's reference to its table.
   * 
   * @param t - A pointer to a MibTable instance.
   */ 
  void		set_reference_to_table(MibTable* t)  { my_table = t; } 
  /**
   * Set the receiver's reference to its row.
   * 
   * @param t - A pointer to a MibTableRow instance.
   */ 
  void		set_reference_to_row(MibTableRow* r)   { my_row = r; } 

  /**
   * Set the maximum access rights of the receiver.
   *
   * @param a - The maximum access rights.
   */   
  void		set_access(mib_access a)	  { access = a; }

  /**
   * Get the instance OID of the MibLeaf object. If the leaf object
   * is a columnar object then the instancce OID will be computed
   * from the table's OID, plus the subidentifier of the leaf's
   * column, and the row's index OID.
   *
   * @return
   *    the instance OID of the leaf object.
   */
  Oidx		get_oid() const;


protected:

  /**
   * Return the value handle mode. 
   *
   * @return 
   *    a integer value describing how to handle the receiver's value.
   * @see constructor for details
   */
  int		get_value_mode()       	{ return value_mode; }

  /**
   * Sets the flags (bits) that determine the type/stage of validity
   * of the leaf's value. If any bit is set the leaf is supposed to
   * contain a valid value. AGENT++ only uses the first 4 bits (0-3).
   * The other bits can be used by the API programmer to track internal
   * states of the leaf.
   * 
   * @param flags
   *    an integer value representing the validity of the receiver.
   */
  void		set_validity(int flags) { validity = flags; }

  /**
   * Set the syntax of the receiver's value and delete the old value.
   *
   * @param syntax - The syntax of a MibLeaf object's value.
   */ 
  void		set_syntax(NS_SNMP SmiUINT32);

  /**
   * Frees the receiver's value and sets it to NULL. 
   */ 
  void		free_value();


  NS_SNMP SnmpSyntax*	value;
  NS_SNMP SnmpSyntax*   undo;

  int		value_mode;

  /**
   * Set of flags (bits) that determine the type/stage of validity
   * of the leaf's value. If any bit is set the leaf is supposed to
   * contain a valid value. AGENT++ only uses the first 4 bits (0-3).
   * The other bits can be used by the API programmer to track internal
   * states of the leaf.
   */
  int      	validity;

  // pointer to its table (row), if this MibLeaf object is part of a table
  // otherwise my_table and my_row will be 0
  MibTable*	my_table;
  MibTableRow*	my_row;

};


/*--------------------------- class snmpRowStatus ------------------------*/

#define rowEmpty		0
#define rowActive		1
#define rowNotInService		2
#define rowNotReady		3
#define rowCreateAndGo		4
#define rowCreateAndWait	5
#define rowDestroy		6

/** 
 * The snmpRowStatus class is derived from MibLeaf and provides
 * functionality to control the manipulation of MibTable rows. The
 * snmpRowStatus class is an encapsulation of the SMIv2 row status
 * textual convention. Figure RowStatusStates demonstrates the states
 * which the snmpRowStatus object can traverse.
 *
 * @author Frank Fock
 * @version 3.5
 */

class AGENTPP_DECL snmpRowStatus: public MibLeaf {

public:
	/**
	 * Default constructor
	 */
	snmpRowStatus() { }

	/**
	 * Construct a snmpRowStatus instance with default access 
	 * READCREATE.
	 * 
	 * @param o - The row status column subidentifier.
	 */
	snmpRowStatus(const Oidx&);

	/**
	 * Construct a snmpRowStatus instance.
	 * 
	 * @param o - The row status column subidentifier.
	 * @param a - The maximum access rights for the managed object.
	 */
	snmpRowStatus(const Oidx&, mib_access);


	/**
	 * Destructor
	 */
	virtual ~snmpRowStatus();

	/**
	 * Check whether the receiver's value may be set to 
	 * a given new value. NOTE: This checks only if the value
	 * may be set at all. Thus notReady(3) will return FALSE.
	 * All other RowStatus enumerated values will return TRUE.
	 *
	 * @param value 
	 *    a new value.
	 * @return 
	 *    TRUE if the new value is valid, otherwise FALSE.
	 */
	virtual bool	       	value_ok(const Vbx&);

	/**
	 * Check whether the a transition to a given new value is
	 * possible in general. This does not check any values of the
	 * row. Use check_state_change to determine whether a row
	 * has sufficient information to actually perform a transition.
	 *
	 * @param value
	 *    a new value.
	 * @return 
	 *    TRUE if the new value is valid, otherwise FALSE.
	 */
	virtual bool	       	transition_ok(const Vbx&);

	/**
	 * Clone the receiver.
	 *
	 * @return A pointer to the clone.
	 */
	virtual MibEntryPtr    	clone();

	/**
	 * Check whether the state of the receiver's row may be changed.
	 * 
	 * @param v
	 *    a variable binding that holds the requested new state.
	 * @param req
	 *    an optional reference to the request being processed.
	 *    If req is not 0 the snmpRowStatus attempts to look forward
	 *    within the request for sets that will affect its row.
	 * @return 
	 *    TRUE if the requested state can be set, otherwise FALSE.
	 */ 
	virtual bool		check_state_change(const Vbx&, Request* req=0);

	/**
	 * Set the receiver's value and backup its old value for a later undo. 
	 *
	 * @param vb - The variable binding that holds the new value.
	 * @return SNMP_ERROR_SUCCESS if the new value has been set,
	 *         SNMP_ERROR_WRONG_TYPE or SNMP_ERROR_BAD_VALUE otherwise.
	 */
	virtual int		set(const Vbx&);

	/** 
	 * Undo a previous set.
	 *
	 * @return SNMP_ERROR_SUCCESS on success and 
	 *         SNMP_ERROR_UNDO_FAIL on failure.
	 */
	virtual int		unset();

	/**
	 * Get the receiver's value.
	 * 
	 * @return The row status of the receiver's row.	
	 */
	long			get();

	/**
	  * Let the receiver prepare a SNMP SET subrequest
	  * 
	  * @param req
	  *    a pointer to the whole SNMP SET request.
	  * @param ind 
	  *    the index of the subrequest to be processed.
	  * @return SNMP_ERROR_SUCCESS on success and SNMP_ERROR_WRONG_VALUE,
	  *         SNMP_ERROR_WRONG_TYPE, or 
	  *         SNMP_ERROR_NOT_WRITEABLE on failure.
	  */
	virtual int		prepare_set_request(Request*, int&);
};



/*--------------------------- class MibTableRow ------------------------*/

/** 
 * The MibTableRow class is a container class for MibLeaf objects. A
 * MibTableRow instance represents a row of a SNMP table. The MibTableRow
 * class provides functions to add MibLeaf objects to a row and functions
 * to find and get them again. Normally a user of the AGENT++ API does
 * not have to be concerned with MibTableRow as the MibTable class
 * provides the corresponding wrapper member functions for the above
 * listed operations on rows.
 * 
 * @author Frank Fock
 * @version 3.5.22
 */

#if !defined (AGENTPP_DECL_TEMPL_LIST_MIBLEAF)
#define AGENTPP_DECL_TEMPL_LIST_MIBLEAF
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL List<MibLeaf>;
#endif
#if !defined (AGENTPP_DECL_TEMPL_ORDEREDLIST_MIBLEAF)
#define AGENTPP_DECL_TEMPL_ORDEREDLIST_MIBLEAF
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL OrderedList<MibLeaf>;
#endif
#if !defined (AGENTPP_DECL_TEMPL_ORDEREDARRAY_MIBLEAF)
#define AGENTPP_DECL_TEMPL_ORDEREDARRAY_MIBLEAF
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL OrderedArray<MibLeaf>;
#endif

class AGENTPP_DECL MibTableRow {
friend class MibTable;

public:
	/**
	 * Default constructor
	 */
	MibTableRow();

	/**
	 * Construct a row with specified base.
	 * 
	 * @param b - A base object identifier (the object identifier 
	 *            of the table's entry object type).
	 */
	MibTableRow(const Oidx& base);

	/**
	 * Copy Constructor
	 *
	 * @param other - Another MibTableRow instance.
	 */
	MibTableRow(const MibTableRow&);

	/**
	 * Destructor - destroys the row and all the MibLeaf 
	 * objects it contains.
	 */
	virtual ~MibTableRow();


	/**
	 * Clone the receiver.
	 *
	 * @return A pointer to the clone.
	 */
	MibTableRow*	clone();


	/**
	 * Append a MibLeaf instance to the end of the receiver row.
	 *
	 * @param l - A pointer to the MibLeaf object to append.
	 * @return The pointer to the added MibLeaf object 
	 *         (always the same as the input pointer).  
	 */
	MibLeaf*       	add(MibLeaf* l); 

	/**
	 * Append a snmpRowStatus instance to the end of the receiver row.
	 * Each row can only contain zero or one snmpRowStatus object, but
	 * that's not checked here.
	 *
	 * @param l - A pointer to the snmpRowStatus object to append.
	 * @return The pointer to the added snmpRowStatus object 
	 *         (always the same as the input pointer).  
	 */
	snmpRowStatus*  add(snmpRowStatus* l);
 

	/**
	 * Get the MibLeaf instance at the n-th (starting from 0) column
	 * of this row.
	 *
	 * @param i - The column index (starting from 0).
	 * @return A pointer to a MibLeaf object.
	 */
	MibLeaf*	get_nth(int i)  { return row.getNth(i); }  

	/**
	 * Get the MibLeaf object at the first column of this row.
	 *
	 * @return A pointer to a MibLeaf object.
	 */
	MibLeaf*	first()		{ return row.first(); }

	/**
	 * Get the MibLeaf object at the last column of this row.
	 *
	 * @return A pointer to a MibLeaf object.
	 */
	MibLeaf*	last()		{ return row.last(); }

	/**
	 * Get the size of this row.
	 *
	 * @return The size of the row.
	 */
	int		size()		{ return row.size(); }

	 /**
	  * Return the element of the receiver row with a given oid.
	  *
	  * @param oid - The object identifier to search for.
	  * @return A pointer to the found MibLeaf object or 0 if no such
	  *         object could be found.
	  */   
	MibLeaf*        get_element(const Oidx&);

	/** 
	 * Remove the object at a specified column from this row.
	 *
	 * @param i - The index (starting from 0) of the column to be removed.
	 * @return TRUE if a such a column existed and has been removed, 
	 *         FALSE otherwise.
	 */  
	bool		remove(int);

	/**
	 * Replace the element at the given column.
	 *
	 * @param column
	 *    the column to be replaced (counted from 0).
	 * @param entry
	 *    a MibLeaf instance.
	 */
	void		replace_element(unsigned int, MibLeaf*);

	/**
	 * Operator <
	 *
	 * @param other - The MibTableRow the receiver is compared with.
	 * @return TRUE if the receiver is less than the comparate, 
	 *         FALSE otherwise.
	 */
	int     	operator<(const MibTableRow&);

	/**
	 * Operator >
	 *
	 * @param other - The MibTableRow the receiver is compared with.
	 * @return TRUE if the receiver is greater than the comparate, 
	 *         FALSE otherwise.
	 */
	int     	operator>(const MibTableRow&);

	/**
	 * Operator ==
	 *
	 * @param other - The MibTableRow the receiver is compared with.
	 * @return TRUE if the receiver is equals the comparate, 
	 *         FALSE otherwise.
	 */
	int     	operator==(const MibTableRow&);

	/**
	 * Operator =
	 *
	 * @param other - The MibTableRow the receiver is assigned to.
	 * @return A reference to the receiver itself.
	 */
	MibTableRow&   	operator=(const MibTableRow&);

	/**
	 * Return whether the receiver row contains an object with a given oid.
	 *
	 * @param oid - The object identfier to search for.
	 * @return TRUE if the receiver row contains an object with the 
	 *         given oid, FALSE otherwise.
	 */
	bool		contains(const Oidx&) const;

	/**
	 * Return the index (counted from 0) of the row's object whose OID 
	 * equals the given oid.
	 *
	 * @param oid - The object id to search for.
	 * @return The index (counted from 0) of the found column or -1 if
	 *         such a column can't be found.
	 */
	int		index_of(const Oidx&) const;

	/**
	 * Return the index (counted from 0) of the row's object whose that 
	 * equals the given object.
	 *
	 * @param column 
	 *    a pointer to an MibLeaf column to search for.
	 * @return 
	 *    the index (counted from 0) of the found column or -1 if
	 *    such a column can't be found.
	 * @since 3.4.7
	 */
	int		index_of(MibLeaf* l) const { return row.index(l); }

	/**
	 * Return the index (counted from 0) of the row's object whose OID 
	 * is greater or equal the given oid.
	 *
	 * @param oid - The object id to search for.
	 * @return The index (counted from 0) of the found column or -1 if
	 *         such a column can�t be found.
	 */
	int		index_of_upper(const Oidx&) const;

	/**
	 * Return the index (counted from 0) of the row's object whose OID 
	 * is less or equal the given oid.
	 *
	 * @param oid - The object id to search for.
	 * @return The index (counted from 0) of the found column or -1 if
	 *         such a column can�t be found.
	 */
	int		index_of_lower(const Oidx&) const;


	/**
	 * Set the index of the receiver�s row. 
	 *
	 * @param ind - The (new) index of the receiver�s row.
	 * @note Currently only multi-column Integer (scalar) indecies and 
	 *       single column OID and String indecies are supported for 
	 *	 automatic index object initialization.
	 */
	virtual void   	set_index(const Oidx& ind);

	/**
	 * Return the index of the receiver row.
	 *
	 * @return The index of the receiver row.
	 */
	Oidx		get_index() { return index; }

	/**
	 * Return a pointer to the index of the receiver row.
	 * 
	 * @return 
	 *    a pointer to the index of the receiver row.
	 */
	Oidx*		key() { return &index; }

	/**
	 * Returns a list of variable bindings representing the 
	 * object identifiers and values of this row. 
	 *
	 * @param vbs
	 *    the array of Vbx instances to store the values.
	 * @param sz
	 *    the size of the array.
	 * @param returnVolatileAsNull 
	 *    if set to TRUE (FALSE is the default), volatile columns
	 *    will return a Null value instead of their real value.
	 * @version 3.5.22
	 */ 
	void		get_vblist(Vbx*, int, 
				   bool returnVolatileAsNull=FALSE);

	/**
	 * Return the row status object of the receiver row, 
	 * if it has one.
	 * 
	 * @return A pointer to the row�s snmpRowStatus object, or
	 *         0 if the row has no row status.
	 */
	snmpRowStatus*	get_row_status() { return row_status; }

	/**
	 * Convert the row to a read-only row.
	 */
	void		read_only();

	/** 
	 * Sets the access value of all columnar objects (MibLeaf instances)
	 * of this row.
	 *
	 * @param access
	 *    the mib_access value that should be applied to the row.
	 */
	void		set_access(mib_access);

protected:
	/**
	 * Set the base (i.e. the table's entry object's oid) oid value.
	 * An oid of an object of an SNMP table consists of the following
	 * three parts:  base + column + index 
	 *
	 * @param b - The base oid.
	 */
	virtual void   	set_base(const Oidx& b) { base = b; }

	/**
	 * Set a reference to the receiver�s table in all of its columns.
	 *
	 * @param t - The MibTable object the reciever belongs to.
	 */
	void		set_reference_to_table(MibTable*);
	/**
	 * Set a reference to the receiver in all of its columns.
	 */
	void		set_reference_to_row();	

	Oidx			index;
	Oidx			base;
#ifdef USE_ARRAY_TEMPLATE
	OrderedArray<MibLeaf>  	row;
#else
	OrderedList<MibLeaf>    row;
#endif

	snmpRowStatus*		row_status;
};

/*----------------------- class MibTableVoter -------------------------*/

/**
 * The MibTableVoter class defines an interface for objects that
 * want to be ask before a status transition of a MibTableRow
 * is performed by a MibTable instance. Each MibTable instance
 * implements this interface in order to allow the user voting
 * for row status changes directly by subclassing.
 *
 * @author Frank Fock
 * @version 3.5
 */

class AGENTPP_DECL MibTableVoter {
 public:
	MibTableVoter() { }
	virtual ~MibTableVoter() { }
	
	/**
	 * Check whether a transition from an old to a new
	 * RowStatus value is allowed. Possible return values
	 * that will abort a transition are shown by the following
	 * table:
	 *
	 * Requested transition      | Posible return values
	 * -------------------------------------------------------
	 * rowEmpty -> rowDestroy    | SNMP_ERROR_INCONSIST_NAME
	 * -------------------------------------------------------
	 * rowEmpty -> notReady      | SNMP_ERROR_RESOURCE_UNAVAIL
	 *                           | SNMP_ERROR_INCONSIST_VAL
	 *                           | SNMP_ERROR_NO_CREATION
	 *                           | SNMP_ERROR_INCONSIST_NAME
	 * -------------------------------------------------------
	 * rowNotInService/          | SNMP_ERROR_RESOURCE_UNAVAIL
	 * rowNotReady ->            | SNMP_ERROR_INCONSIST_VAL
	 * rowActive                 |
	 * -------------------------------------------------------
	 * rowActive -> notInService | SNMP_ERROR_INCONSIST_VAL
	 *              rowDestroy   | 
	 *
	 * By returning SNMP_ERROR_SUCCESS the MibTableVoter instance
	 * votes for the transition to be performed.
	 *
	 * @param table
	 *    a pointer to a MibTable instance firing the event. 
	 *    The parameter 'table' can be 0 if the local table
	 *    is source and target of the event.
	 * @param row
	 *    a pointer to the MibTableRow instance whose status is to
	 *    be changed.
	 * @param index
	 *    the index of the row that is changed.
	 * @param currentStatus
	 *    the current status of the row.
	 * @param requestedStatus
	 *    the requested new status of the row. 
	 * @return
	 *    a SNMP error status or SNMP_ERROR_SUCCESS if the transition
	 *    is acknowledged by the voter.
	 */
	virtual int is_transition_ok(MibTable*, 
				     MibTableRow*,
				     const Oidx&,
				     int,
				     int) { return SNMP_ERROR_SUCCESS; } 
};

/*--------------------------- class MibTable --------------------------*/

struct index_info {
  NS_SNMP SmiUINT32 type;
  bool   implied;
  unsigned int	    min;
  unsigned int	    max;
};	


/**
 * Instances of MibTable represent SNMP tables.  The MibTable class is a
 * container class for MibTableRow objects, but seen from the users view
 * point a MibTable seems to contain only MibLeaf objects. A MibTable
 * object must be initialised by adding to it a set of MibLeaf objects
 * called columnar objects. This is best done in the constructor of a
 * sub-class of MibTable by using the add_col member function.
 * 
 * Each columnar object is the master copy for any columnar object
 * instance of its column. Whenever a new row is created MibTable will
 * clone each columnar object once to build the new row. Hence, it is
 * necessary to redefine the clone method of every class derived from
 * MibLeaf.
 * 
 * The index of a SNMP table may consist of a fixed or a variable size 
 * object identifier representing one or more string, unsigned integer, 
 * IP address, or object identifier values. If the size of an object
 * (oid or string) is of variable length, then this object is converted
 * into an oid value by first encoding the length of the object value
 * and then its value. For example
 *
 * "012" is encoded as .3.48.49.50
 * "1.3.6.1" is encoded as .4.1.3.6.1
 *
 * In SMIv2 the last variable length string or oid value of an index 
 * may be of implied length. Thus, the length subidentifier can be 
 * omitted. 
 *
 * In AGENT++, the structure of a table's index is given by an array
 * of index_info structures. Each element represents an object of the
 * index. The type attribute denotes the SNMP type of the sub-index
 * object. The implied flag may be true for the last sub-index object
 * only. In addition, only variable length sub-index objects may have
 * implied length. Variable length sub-index objects may be strings
 * or OIDs. These objects may have a minimum length which is less than
 * their maximum length. For fixed length sub-index objects the 
 * minimum length equals the maximum length. In either case, the 
 * length is measured in subidentifiers. 
 * 
 * As soon as all columnar objects have been added, rows can be added to
 * the empty table by using the add_row member function. The add_row
 * member function needs an object identifier representing the row's
 * index as single parameter. The MibLeaf objects cloned from the
 * columnar objects are then responsible for answering SNMP
 * requests. Rows can be added automatically by SNMP SET requests if the
 * MibTable object contains a snmpRowStatus columnar object. See
 * snmpRowStatus for more information about the SMIv2 row status
 * mechanism. Rows can be removed with the remove_row member function.
 * 
 * Whenever the status of a row is changed the fire_row_changed method
 * is called. This calls the row_added, row_delete, row_activated, or
 * row_deactivated method of the owner (accordingly to the ocurred
 * event) and same method for of all registered listeners.
 *
 * @author Frank Fock
 * @version 3.5.22
 */

#if !defined (AGENTPP_DECL_TEMPL_LIST_MIBTABLE)
#define AGENTPP_DECL_TEMPL_LIST_MIBTABLE
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL List<MibTable>;
#endif
#if !defined (AGENTPP_DECL_TEMPL_LIST_MIBTABLEROW)
#define AGENTPP_DECL_TEMPL_LIST_MIBTABLEROW
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL List<MibTableRow>;
#endif
#if !defined (AGENTPP_DECL_TEMPL_LIST_MIBTABLEVOTER)
#define AGENTPP_DECL_TEMPL_LIST_MIBTABLEVOTER
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL List<MibTableVoter>;
#endif
#if !defined (AGENTPP_DECL_TEMPL_OIDLIST_MIBTABLEROW)
#define AGENTPP_DECL_TEMPL_OIDLIST_MIBTABLEROW
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL OidList<MibTableRow>;
#endif
#if !defined (AGENTPP_DECL_TEMPL_ORDEREDLIST_MIBTABLEROW)
#define AGENTPP_DECL_TEMPL_ORDEREDLIST_MIBTABLEROW
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL OrderedList<MibTableRow>;
#endif

class AGENTPP_DECL MibTable: public MibEntry, public MibTableVoter {
  friend class Mib;		// needs access to find_next() and get()
  friend class snmpRowStatus;	// needs access to _ready_for_service()
public:

	/** 
	 * Copy constructor.
	 */
	MibTable(const MibTable&);

	/**
	 * Constructor with object identifier with single index object
	 * like string or oid with implied length.
	 *
	 * @param o 
	 *    the object identifier of the table, which has to be
	 *    the oid of the the SMI table entry object (table.1).
	 */
	MibTable(const Oidx&);

	/**
	 * Constructor with object identifier and single index object's
	 * index length (meassured in subidentifiers).
	 *
	 * @param o 
	 *    the object identifier of the table, which has to be
	 *    the oid of the the SMI table entry object (table.1).
	 * @param ilen
	 *    the length of the index meassured in subidentifiers.
	 *    0 represents a variable length string or oid index object.
	 *    -1 represents a variable length string or oid index object
	 *    with implied length. 
	 */
	MibTable(const Oidx&, int);

	/**
	 * Constructor with object identifier and given fixed index length.
	 *
	 * @deprecated 
	 *    Use MibTable(const Oidx&, unsigned long[][], unsigned int) 
	 *    instead
	 *
	 * @param o - The object identifier of the table, which has to be
	 *            the oid of the the SMI table entry object (table.1).
	 * @param ilen - The length of the index meassured in subidentifiers.
	 * @param a - not used
	 */
	MibTable(const Oidx&, int, bool);


	/**
	 * Constructor with object identifier and given index structure.
	 * In AGENT++ the structure of a table's index is given by an array
	 * of integer values. Each element represents an object of the index.
	 * A value of -1 represents a implied variable length string or oid.
	 * A value of 0 a variable length string or oid index object.
	 * A value greater than 0 determines the length of the index object 
	 * measured in subidentifiers. 
	 * 
	 * @param o
	 *    the object identifier of the table, which has to be
	 *    the oid of the the SMI table entry object (table.1).
	 * @param index
	 *    an integer array. The length of the array corresponds to
	 *    the number of objects in the INDEX clause.
	 * @param size
	 *    the size of the above index array.
	 */
	MibTable(const Oidx&, const index_info*, unsigned int);	

	/**
	 * Destructor.
	 */
	virtual		~MibTable();
 
	 /**
	  * Return the type of the receiver.
	  *
	  * @return AGENTPP_TABLE
	  */
	mib_type       	type() const		{ return AGENTPP_TABLE; }

	/**
	 * Clone the receiver object.
	 * 
	 * @return A pointer to the clone.
	 */
	virtual MibEntryPtr    	clone()	{ return new MibTable(*this); }	

	
	/**
	 * Add a column to the receiver table.
	 * 
	 * @param l - A pointer to a MibLeaf object which should be added  
	 *            to the columnar objects of the receiver.
	 */ 
	virtual void 	add_col(MibLeaf*); 

	/**
	 * Add a row status column to the receiver table.
	 * 
	 * @param l - A pointer to a snmpRowStatus object which should be  
	 *            added to the columnar objects of the receiver.
	 * @note A table can only have one row status columnar object.   
	 */ 
	virtual void   	add_col(snmpRowStatus*);

	/**
	 * Replace a column of the table.
	 *
	 * @param columnIndex
	 *    the index of the column counted from 0.
	 * @param newColumn
	 *    the new column MibLeaf instance.
	 */
	virtual void    replace_col(unsigned int, MibLeaf*);

	/**
	 * Serialize the values of the receiver table.
	 * 
	 * @param buf - A pointer to byte stream buffer returned.
	 * @param sz - The size of the buffer returned.
	 * @return TRUE if serialization was successful, FALSE otherwise.
	 */
	virtual bool		serialize(char*&, int&);

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
	virtual bool		deserialize(char*, int&);

	/**
	 * Return the immediate successor of the greatest object identifier 
	 * within the receiver's scope
	 * 
	 * @return 
	 *    a pointer to the non including upper bound of the receiver's 
	 *    scope
	 */
	virtual OidxPtr max_key();

	/**
	 * This method is called by a Mib instance whenever it needs to know
	 * how many rows the receiver contains. This method can be used to
	 * update the receiver before a request accessing the receiver is
	 * processed.
	 * Note: The method may be called once for each subrequest of the
	 * given request. So, it is the implementor's responsibility to 
	 * check whether an update is needed or not. 
	 * For example, the pointer to the last request could be 
	 * saved within the receiver. Only if req is different from the
	 * last request pointer, an update will actually be performed.
	 *
	 * @param req
	 *    the request that needs to update the receiver.
	 */
	virtual void	update(Request*) { }

	/**
	 * Return whether the table is empty or not.
	 *
	 * @return TRUE if the table is empty, FALSE otherwise.
	 */
	virtual bool is_empty() { return content.empty(); }

	 /**
	  * Set a value of column in a row of the receiver table.
	  * If the row does not exist, try to create one.
	  *
	  * @param req - A pointer to the whole SNMP GET request.
	  * @param ind - The index of the subrequest to be processed.
	  * @return SNMP_ERROR_SUCCESS on success, 
	  *         SNMP_ERROR_WRONG_TYPE or SNMP_ERROR_BAD_VALUE otherwise.
	  */
	virtual int    	set_value(Request*, int);


	/**
	 * Let the receiver process a SNMP GET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP GET request.
	 * @param ind - The index of the subrequest to be processed.
	 */
	virtual void   	get_request(Request*, int); 
	
	/**
	 * Let the receiver process a SNMP GETNEXT subrequest
	 * 
	 * @param req - A pointer to the whole SNMP GETNEXT request.
	 * @param ind - The index of the subrequest to be processed.
	 */
	virtual void   	get_next_request(Request*, int);
	
	/**
	 * Let the receiver commit a SNMP SET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP SET request.
	 * @param ind - The index of the subrequest to be processed.
	 * @return SNMP_ERROR_SUCCESS on success and 
	 *         SNMP_ERROR_COMITFAIL on failure.
	 */
	virtual int    	commit_set_request(Request*, int);
	
	/**
	 * Let the receiver prepare a SNMP SET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP SET request.
	 * @param ind - The index of the subrequest to be processed.
	 * @return SNMP_ERROR_SUCCESS on success and SNMP_ERROR_WRONG_VALUE,
	 *         SNMP_ERROR_WRONG_TYPE, or 
	 *         SNMP_ERROR_NOT_WRITEABLE on failure.
	 */
	virtual int    	prepare_set_request(Request*, int&);
  
	/**
	 * Let the receiver undo a SNMP SET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP SET request.
	 * @param ind - The index of the subrequest to be processed.
	 * @return SNMP_ERROR_SUCCESS on success and 
	 *         SNMP_ERROR_UNDO_FAIL on failure.
	 */
	virtual int    	undo_set_request(Request*, int&);

	/**
	 * Free any resources allocated for a SET request (i.e., undo
	 * information).
	 *
	 * @param req
	 *    a pointer to the SNMP SET request.
	 * @param ind
	 *    the index of the subrequest to be processed.
	 * @return SNMP_ERROR_SUCCESS if the new value has been set,
	 *         SNMP_ERROR_WRONG_TYPE or SNMP_ERROR_BAD_VALUE otherwise.
	 */
	virtual void   	cleanup_set_request(Request*, int&);

	/**
	 * Check whether the specified row is ready to set in service.
	 * This method is left empty for override by the API user.
	 *
	 * @param pvbs - A pointer to an array of Vbx objects containing
	 *               the values and oids of the columns of a row to check.
	 * @param sz - The size of the array.
	 * @return TRUE if the specified row is ready to set for service.
	 */
	virtual bool	ready_for_service(Vbx* pvbs, int sz);

	/**
	 * Add a row with the given index to the table.
	 * by cloning the generator row and setting the oids accordingly. 
	 * Then call row_added.
	 *
	 * @param ind The index of the new row.
	 * @return A pointer to the added row.
	 */
	virtual MibTableRow*   	add_row(const Oidx&);

	/**
	 * Initialize a row with values. This method is typically called
	 * when a row is loaded from persistent storage.
	 *
	 * @param index
	 *   the index of the row.
	 * @param vbs
	 *   a pointer to an array of variable bindings that provides
	 *   initial values for all columns in the newly created row.
	 *   The size of the array must be the same as the size of the row.
	 * @return 
	 *   a pointer to the added row.
	 */
	virtual MibTableRow*   	init_row(const Oidx&, Vbx*);

	/**
	 * Remove a row with the given index from the table and call row_delete
	 * before.
	 *
	 * @param ind 
	 *    the index of the row.
	 */
	virtual void	       	remove_row(const Oidx&);

	/**
	 * Is called after a new row has been initialized (i.e., after
	 * it has been loaded from persistent storage) 
	 * 
	 * @param row 
	 *    a pointer to MibTableRow that has been created.
	 * @param index
	 *    the index of the row.
	 * @param source
	 *    a pointer to the source MibTable of the event, or 0 if the
	 *    event is local.
	 */
	virtual void   	row_init(MibTableRow*, const Oidx&, 
				 MibTable* t=0) { (void)t; }

	/**
	 * Is called after a new row has been created and automatic index
	 * generation has taken place.
	 * 
	 * @param row A pointer to MibTableRow that has been created.
	 * @param index - The index of the row.
	 * @param source
	 *    a pointer to the source MibTable of the event, or 0 if the
	 *    event is local.
	 */
	virtual void   	row_added(MibTableRow*, const Oidx&, 
				  MibTable* t=0) { (void)t; }
	/**
	 * Is called before a row is deleted by MibTable
	 *
	 * @param row A pointer to MibTableRow that will be deleted.
	 * @param index - The index of the row.
	 * @param source
	 *    a pointer to the source MibTable of the event, or 0 if the
	 *    event is local.
	 */
	virtual void   	row_delete(MibTableRow*, const Oidx&, 
				   MibTable* t=0) { (void)t; }

	/**
	 * Is called when a row is activated by a SET request setting
	 * the row�s snmpRowStatus object to active(1).
	 *
	 * @param row - A pointer to MibTableRow that has been activated.
	 * @param index - The index of the activated row.
	 * @param source
	 *    a pointer to the source MibTable of the event, or 0 if the
	 *    event is local.
	 */
	virtual void	row_activated(MibTableRow*, const Oidx&, 
				      MibTable* t=0) { (void)t; } 

	/**
	 * Is called when a row is deactivated by a SET request setting
	 * the row�s snmpRowStatus object to notInService(2).
	 *
	 * @param row - A pointer to MibTableRow that has been deactivated.
	 * @param index - The index of the deactivated row.
	 * @param source
	 *    a pointer to the source MibTable of the event, or 0 if the
	 *    event is local.
	 */
	virtual void	row_deactivated(MibTableRow*, const Oidx&, 
					MibTable* t=0) { (void)t; } 

	/**
	 * Return the next available index value for the receiver table,
	 * that can be used be a manager to create a new row.
	 * @return The next available index value.
	 * @note Works best if the table�s index is a single scalar 
	 *       sub-identifier.
	 */ 
	virtual Oidx	get_next_avail_index() const;

	/**
	 * Find the row within the receiver table with a given index.
	 * 
	 * @param ind - The index to search for.
	 * @return A pointer to the found row, or 0 if a row with the 
	 *         given index does not exists.
	 */ 
	MibTableRow*   	find_index(const Oidx&) const;

	/**
	 * Returns the index part of a given oid (relative to the receiver).
	 * 
	 * @param entry_oid - The oid of an instance in the receiver table.
	 * @return The index part of the given oid.
	 */
	Oidx	       	index(const Oidx&) const;

	/**
	 * Check whether the given index is a valid index for the 
	 * receiver table.
	 * 
	 * @param index
	 *    an Oidx instance.
	 * @return 
	 *    TRUE if the index is valid, FALSE otherwise.
	 */
	virtual bool is_index_valid(const Oidx&) const;

	/**
	 * Returns the base of an oid.
	 * (the entry oid without index and item index) 
	 * @param oid - The oid that should be scoped to this tables base
	 * @return The base of the given oid.
	 */
	Oidx	       	base(const Oidx&);

	/**
	 * Return all (active) rows as a two dimensional array of Vbx
	 * objects. If the receiver table does not have any snmpRowStatus 
	 * column all rows are returned.
	 *
	 * This method is synchronized on its receiver.
	 *
	 * @param contents 
	 *    a two dimensional array of Vbx objects returned. Memory is 
	 *    allocated if the pointer is NULL. Otherwise the rows and cols
	 *    parameter must provide the size of the given array.
	 * @param rows
	 *    the number of rows returned or the max rows capacitiy of the
	 *    given array, respectively.
	 * @param cols
	 *    the number of cols returned or the max cols capacitiy of the
	 *    given array, respectively. 
	 * @param discriminator
	 *    if the receiver table has a snmpRowStatus, the discriminator
	 *    selects the rows to be returned. Default is rowActive, which
	 *    means all active rows are returned. If the discriminator is
	 *    rowEnmpty(0), all rows are returned.
	 */
	virtual void	get_contents(Vbx**&, int&, int&, int = rowActive);

	/**
	 * Return a pointer to the internal list of rows. This list 
	 * operates directly of the tables data. Do not delete the
	 * returned pointer, as this would corrupt the table.
	 *
	 * @return
	 *    a pointer to an OidList instance.
	 */
        OidList<MibTableRow>*	rows() { return &content; }

	/**
	 * Return all (active) rows as a list of pointers to the
	 * corresponding MibTableRow instances. If the receiver table 
	 * does not have any snmpRowStatus column all rows are returned.
	 *
	 * @note Don't use this method for tables where rows are deleted,
	 *       because you only get references, that then may point to 
	 *       nowhere.
	 * @note Call List::clear() method before its destructor to avoid
	 *       deletion of the rows of this table.
	 *
	 * @param discriminator
	 *    if the receiver table has a snmpRowStatus, the discriminator
	 *    selects the rows to be returned. Default is rowActive, which
	 *    means all active rows are returned. If the discriminator is
	 *    rowEmpty(0), all rows are returned.
	 */
	virtual List<MibTableRow>* get_rows(int = rowActive);

	/**
	 * Return all (active) rows as a list of pointers to the
	 * corresponding MibTableRow instances. If the receiver table 
	 * does not have any snmpRowStatus column all rows are returned.
	 *
	 * This method is synchronized on its receiver.
	 *
	 * @note This method is escpecially useful for tables where rows 
	 *       are deleted, because you get copies of the rows rather
	 *       than references.
	 * 
	 * @param discriminator
	 *    if the receiver table has a snmpRowStatus, the discriminator
	 *    selects the rows to be returned. Default is rowActive, which
	 *    means all active rows are returned. If the discriminator is
	 *    rowEmpty(0), all rows are returned.
	 * @return 
	 *    a pointer to a cloned list of the rows in the receiver.
	 */
	virtual List<MibTableRow>* get_rows_cloned(int = rowActive);

	/**
	 * Return those (active) rows as a list of pointers to the
	 * corresponding MibTableRow instances, of which the index starts
	 * with the given prefix. 
	 *
	 * This method is synchronized on its receiver.
	 *
	 * @note This method is escpecially useful for tables where rows 
	 *       are deleted, because you get copies of the rows rather
	 *       than references.
	 * 
	 * @param prefix
	 *    a pointer to an oid. If the pointer is 0 this method 
	 *    behaves like MibTable::get_rows_cloned(bool).
	 * @param discriminator
	 *    if the receiver table has a snmpRowStatus, the discriminator
	 *    selects the rows to be returned. Default is rowActive, which
	 *    means all active rows are returned. If the discriminator is
	 *    rowEmpty(0), all rows are returned.
	 * @return 
	 *    a pointer to a cloned list of the rows in the receiver.
	 */
	virtual List<MibTableRow>* get_rows_cloned(const Oidx*, int = rowActive);

	/**
	 * Return the size of the table meassured in rows.
	 *
	 * @return 
	 *    the size of the receiver.
	 */
	virtual int		size();

	/**
	 * Add a MibTableVoter instance that listens for row status
	 * transition events and votes for or against commiting such
	 * a transition.
	 *
	 * @param listener
	 *   a pointer to a MibTableVoter instance.
	 */
	virtual void		add_voter(MibTableVoter*);

	/**
	 * Remove a MibTableVoter listener.
	 *
	 * @param listener
	 *   a pointer to a MibTableVoter instance.
	 */
	virtual void		remove_voter(MibTableVoter*);

	/**
	 * Add a listener that will be informed about row
	 * additions or deletions. 
	 * Note: Since v3.5.8 listeners are no longer locked when
	 * fire_row_changed is executed. Instead, all listeners are
	 * locked when this table gets locked by Request::set_locked! 
	 *
	 * @param listener
	 *    a pointer to another instance of MibTable whose row_added,
	 *    row_delete, row_activated, and row_deactivated methods
	 *    will be called whenever the corresponding action is performed
	 *    on this table. 
	 */
	virtual void		add_listener(MibTable*);

	/**
	 * Remove a listener that no longer wants to be informed about row
	 * additions or deletions.
	 *
	 * @param listener
	 *    a pointer to another instance of MibTable
	 */
	virtual void		remove_listener(MibTable*);

	/**
	 * Fire the row changed event to the receiver and all its
	 * listeners.
	 * Note: Since v3.5.8 this method no longer locks the listeners
	 * when calling their methods. See also Request::set_locked.
	 *
	 * @param event
	 *    describes the event that occured:
	 *      rowCreateAndGo, rowCreateAndWait for row_added,
	 *      rowActive for row_activated,
	 *      rowNotInService for row_deactivated, and
	 *      rowDestroy fro row_delete.
	 * @param row
	 *    a pointer to the MibTableRow instance to be changed.
	 * @param index
	 *    the index of the above row.
	 */
	void			fire_row_changed(int, MibTableRow*, 
						 const Oidx&);

	/**
	 * Gets a cursor on the listeners for this table's row events.
	 * This method is not synchronized.
	 *
	 * @return
	 *	a pointer to a ListCursor. 
	 */
	ListCursor<MibTable>*   get_listeners() 
		{ return new ListCursor<MibTable>(&listeners); }

	/**
	 * Checks whether this table has any listeners for row events.
	 *
	 * @return
	 *	TRUE if there is at least one listener attached to this
	 *      table via ::add_listener, FALSE otherwise.
	 */
	bool			has_listeners() const 
		{ return (listeners.size() > 0); }

	/**
	 * Removes all rows from this table.
	 */
	virtual void		clear();
	
	/**
	 * Resets the content of the table to its state right after
	 * construction. By default this method calls MibTable::clear()
	 * to remove all rows.
	 */
	virtual void		reset() { clear(); }
	
	/**
	 * Gets the columns (a.k.a. generator row) definitions for
	 * this table.
	 * @return
	 *    a pointer to a MibTableRow instance that is used by this
	 *    table to generate rows from by cloning its elements.
	 */
	virtual MibTableRow*	get_columns();

	 /**
	  * Find a MibLeaf object of the receiver with a given oid. 
	  *
	  * @param o - The oid of the object to find.
	  * @return A pointer to the found object, 0 otherwise.
	  */  
	MibLeaf*       	find     (const Oidx&) const;

	/**
	 * Find the lexicographical successor MibLeaf object to a given oid.
	 *
	 * @param o - An oid.
	 * @return A pointer to the successor object, 0 otherwise.
	 */  
	virtual MibLeaf*       	find_next(const Oidx&);

	/**
	 * Find the lexicographical predessor MibLeaf object to a given oid.
	 *
	 * @param o - An oid.
	 * @return A pointer to the predessor object, 0 otherwise.
	 */  
	MibLeaf*       	find_prev(const Oidx&);

	/**
	 * Return the MibLeaf object of the receiver at a given position.
	 * 
	 * @param n - The column (counted from 0).
	 * @param m - The row (counted from 0).
	 * @return A pointer to the MibLeaf object at position (n,m), 0
	 *         if n or m are out of range.
	 */
	MibLeaf*       	get(int, int);

	/**
	 * Reinitialize the table. By default, this method does nothing.
	 * The method is called when a Mib instance needs to reinitialize
	 * its objects, for example, if a subagent needs to reconnect to
	 * its master agent.
	 *
	 * @since 3.5.22
	 */
	virtual void   	reinit() { }

protected:

	/**
	 * Initialize the receiver.
	 *
	 * @param o - The initialize object identifer of the receiver.
	 * @param ilen - The fixed index length, or VARIABLE_INDEX_LENGTH 
	 *               if the index length is variable.
	 * @param a - If TRUE the automatic index object initialization is
	 *            activated.
	 */                
	void	       	init(const Oidx&, const index_info*, unsigned int);

	/**
	 * Check if an object of a given oid could be created.
	 *
	 * @param o
	 *    the oid to be checked.
	 * @param result
	 *    returns the SNMP error code, which explains why creation
	 *    could not be performed. Possible values are:
	 *    SNMP_ERROR_NO_CREATION and SNMP_ERROR_NOT_WRITEABLE
	 * @return TRUE if an object of the given oid could be created.
	 */ 
	virtual bool	could_ever_be_managed(const Oidx&, int&);

	/**
	 * Remove all rows that are not in the given list.
	 * 
	 * @param confirmed_rows - A list of row indecies of rows which 
	 *                         should not be removed from the receiver.
	 */
	void	       	remove_obsolete_rows(OrderedList<Oidx>& confirmed);

	/**
	 * Return the successor of a given object identifier within the 
	 * receiver's scope and the context of a given Request.
	 * SYNCHRONIZED
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
	virtual Oidx   	find_succ(const Oidx&, Request* req = 0);

	/**
	 * Return a pointer to the generator object of a given column.
	 * (The generator object of a column is the not accessible object
	 * a the top of each row - the master copy)
	 *
	 * @param o - The oid of an MibLeaf instance of the receiver.
	 * @return A pointer to the generator object of the specified column.
	 */ 
	MibLeaf*       	get_generator(const Oidx&);

	/**
	 * Test whether a row can be created succesfully using the
	 * RowStatus mechanism.
	 * 
	 * @param req - A pointer to the whole SNMP SET request.
	 * @param ind - The index of the subrequest to be processed.
	 * @return SNMP_ERROR_SUCCESS on success and 
	 *         SNMP_ERROR_WRONG_VALUE, SNMP_ERROR_INCONSIST_VAL, or
	 *         SNMP_ERROR_WRONG_TYPE on failure.
	 * @note In case of an error: ind is set to the vb index in the 
	 *       processed request req where the error actually occured.
	 */
	virtual int    	check_creation(Request*, int&);

	/**
	 * Check whether the specified row is ready to set in service.
	 * - check if all required (non default) values are given.
	 * - check if given values are ok (by asking MibLeaf::value_ok).
	 * - ask ready_for_service whether row can be set active.
	 *
	 * @param pvbs 
	 *    a pointer to an array of Vbx objects containing the
	 *    (possible new) values and oids of the columns of a row to check.
	 * @param sz
	 *    the size of the array.
	 * @param row 
	 *    a pointer to the original row. If this is a new row,
	 *    it is pointing to a MibTableRow that is filled with the
	 *    new values, but is not inserted into the table.
	 * @return TRUE if the specified row is ready to set for service.
	 */
	virtual bool	ready(Vbx* pvbs, int sz, MibTableRow* row);

	/**
	 * Determine the required columns of a row.
	 * 
	 * @param required
	 *    an array of at least the size of each receiver's row.
	 * @param vbs  
	 *    an array of variable bindings of at least the size of a row.
	 *    It returns the default values of that row. If vbs is 0
	 *    no default values are returned.
	 */
	virtual void   	get_required_columns(bool*, Vbx* vbs = 0);


	/**
	 * Get the value of the snmpRowStatus object of a given row.
	 * 
	 * @param row - A row of the receiver.
	 * @return The value of the snmpRowStatus object of the given row.
	 *         If the row has no snmpRowStatus object return rowDestroy.
	 */ 
	int         	get_row_status(MibTableRow*);

	/**
	 * Set the value of the snmpRowStatus object of a given row.
	 * 
	 * @param row - A row of the receiver.
	 * @param status - The new value for the snmpRowStatus object.
	 * @return If the row has no snmpRowStatus object return
	 *         SNMP_ERROR_RESOURCE_UNAVAIL, on success SNMP_ERROR_SUCCESS. 
	 */ 
	int            	set_row_status(MibTableRow*, int);


	/**
	 * Periodically remove all rows that are notReady for more
	 * than a given timeout (default is 5 minutes).
	 * For tables that have no row status, this can also be used
	 * for deferred row deletion: simply add the row to delete to
	 * the notread_rows list and let Mib::cleanup delete the rows
	 * by calling this method. 
	 *
	 * Note: a row cannot be set from notInService or active to notReady
	 */
	void	        remove_unused_rows();

	/**
	 * Check an index for subidentifiers greater than 255.
	 *
	 * @param index
	 * @param begin
	 * @param end
	 * @return 
	 *    TRUE if no such subid exists.
	 */
	bool		check_index(Oidx&, unsigned long, unsigned long) const;

	/**
	 * Check whether the given row should be serialized or not.
	 *
	 * @param row
	 *    a MibTableRow instance.
	 * @return
	 *    TRUE if the row should be stored persistently, FALSE otherwise
	 */
	virtual bool		is_persistent(MibTableRow*) { return TRUE; }

	/**
	 * Let all voters vote for or against a row status transition.
	 *
	 * @param row
	 *    a pointer to the MibTableRow to be changed.
	 * @param currentStatus
	 *    the row's current status.
	 * @param requestedStatus
	 *    the row's requested new status.
	 * @return
	 *    a SNMP error status or SNMP_ERROR_SUCCESS if the transition
	 *    is acknowledged by all voters.
	 */
	int			perform_voting(MibTableRow*, int, int);


	// the upper bound of the table returned by max_key()
	Oidx			upper;

	MibTableRow	       	generator;
	OidList<MibTableRow>    content;
	unsigned int	      	index_len;
	index_info*	       	index_struc;

	// if this table has a variable size, 
	// row_status is used to create / delete
	// conceptual rows
	snmpRowStatus*		row_status;
	Timer	       		row_timeout;
	OrderedList<MibTableRow> notready_rows;
	OrderedList<MibTableRow> delete_rows;

	List<MibTable>		listeners;
	List<MibTableVoter>	voters;
};

inline Oidx MibLeaf::get_oid() const
{
  if ((!my_table) || (!my_row)) {
    return oid;
  }
  else {
    Oidx o(*my_table->key());
    o += oid;
    o += my_row->get_index();
    return o;
  }
}

/*--------------------- class NotificationSender --------------------*/

/**
 * The abstract class NotificationSender defines the interface for
 * all classes capable of sending notifications or inform requests.
 * Known implementations of this interface are NotificationOriginator
 * and SubAgentXMib. 
 * 
 * @author Frank Fock
 * @version 3.5.10
 * @since 3.5.10
 */

class AGENTPP_DECL NotificationSender 
{
 public:
	/**
	 * Default constructor.
	 */
	NotificationSender() {}

	/**
	 * Destructor.
	 */
	virtual ~NotificationSender() {}

	/**
	 * Send a notification.
	 *
	 * @param context
	 *    the context originating the notification ("" for the default 
	 *    context).
	 * @param trapoid
	 *    the oid of the notification.
	 * @param vbs
	 *    an array of variable bindings.
	 * @param size
	 *    the size of the above variable binding array.
	 * @param timestamp
	 *    an optional timestamp.
	 * @return 
	 *    SNMP_ERROR_SUCCESS if the notification could be sent
	 *    successfully, otherwise an appropriate SNMP error is
	 *    returned.
	 */
	virtual int		notify(const NS_SNMP OctetStr&, const Oidx&,
				       Vbx*, int, unsigned int=0) = 0; 
};

/*--------------------- class NotificationType --------------------*/

/**
 * The abstract class NotificationType defines the interface for
 * all classes implementing a NOTIFICATION-TYPE or TRAP-TYPE SMI
 * construct. Typically a NotificationType uses a NotificationSender
 * to send a notification, trap, or inform request.
 * 
 * @author Frank Fock
 * @version 3.5.10
 * @since 3.5.10
 */

class AGENTPP_DECL NotificationType 
{
 public:
	/**
	 * Default constructor.
	 */
	NotificationType() {}

	/**
	 * Destructor.
	 */
	virtual ~NotificationType() {}

	/**
	 * Send a notification.
	 *
	 * @param context
	 *    the context originating the notification ("" for the default 
	 *    context).
	 * @param vbs
	 *    an array of variable bindings that have to contain at least
	 *    instances for those objects specified in the 
	 *    NOTIFICATION-TYPE's OBJECTS clause.  
	 * @param size
	 *    the size of the above variable binding array.
	 * @return 
	 *    SNMP_ERROR_SUCCESS if the notification could be sent
	 *    successfully, otherwise an appropriate SNMP error is
	 *    returned.
	 */
	virtual int		notify(const NS_SNMP OctetStr&, Vbx*, int) = 0; 
};

/*---------------------- class MibConfigFormat ----------------------*/

/**
 * The MibConfigFormat is a pure virtual class that defines the interface
 * for configuration formats that can be used to load and store 
 * persistent Mib data.
 *
 * @version 3.5.22c
 * @since 3.5.22
 */
class AGENTPP_DECL MibConfigFormat {

 public:

    virtual ~MibConfigFormat() {};

    /**
     * Stores the persistent data in the supplied MibContext to disk.
     * @param context
     *    a pointer to the MibContext to store.
     * @param path
     *    the storage path to use.
     * @return
     *    TRUE if the contents could be saved successfully, FALSE otherwise.
     */
    virtual bool	save(MibContext*, const NS_SNMP OctetStr&) = 0;

    /**
     * Loads the persistent data in the supplied MibContext from disk.
     * @param context
     *    a pointer to the MibContext to load.
     * @param path
     *    the storage path to use.
     * @return
     *    TRUE if the contents could be saved successfully, FALSE otherwise.
     */
    virtual bool	load(MibContext*, const NS_SNMP OctetStr&) = 0;

    /**
     * Clone this format (needed by ArrayList template).
     */
    virtual MibConfigFormat*	clone() = 0;
};

/*----------------------- class MibConfigBER ------------------------*/

/**
 * The MibConfigBER implements the default persistent data configuration
 * format that is using BER encoding for platform independent storage.
 *
 * @version 3.5.22c
 * @since 3.5.22
 */
class AGENTPP_DECL MibConfigBER: public MibConfigFormat {

 public:

    MibConfigBER() { }

    /**
     * Stores the persistent data in the supplied MibContext to disk.
     * @param context
     *    a pointer to the MibContext to store.
     * @param path
     *    the storage path to use.
     * @return
     *    TRUE if the contents could be saved successfully, FALSE otherwise.
     */
    virtual bool	save(MibContext*, const NS_SNMP OctetStr&);

    /**
     * Loads the persistent data in the supplied MibContext from disk.
     * @param context
     *    a pointer to the MibContext to load.
     * @param path
     *    the storage path to use.
     * @return
     *    TRUE if the contents could be saved successfully, FALSE otherwise.
     */
    virtual bool	load(MibContext*, const NS_SNMP OctetStr&);

    virtual MibConfigFormat*	clone() { return new MibConfigBER(); } 

};


/*--------------------------- class Mib -----------------------------*/

/**
 * The AGENT++ Mib class represents a central part of any agent - the
 * Management Information Base (MIB). The MIB of a SNMP agent is
 * a conceptual database rather than a real database. An Agent has
 * exactly one MIB, thus the Mib class is a singleton. Mib
 * has three main functional areas:
 *
 * (a) Registration of MIB objects
 *
 * Use the add member functions to add a MIB object (any C++ object
 * derived from MibEntry) to the agent�s MIB. Use the remove
 * member functions to remove a MIB object from the agent�s MIB. Both
 * functions can be used while the agent is running.
 *
 * (b) Receive and Process SNMP Requests
 *
 * Incoming SNMP requests are accepted by using the receive member
 * function. The receive function waits for such a request until a
 * given timeout is reached. The timeout is given in seconds and if 
 * it is zero receive looks for a pending request and returns it
 * immediately or if is not such a request receive returns the null
 * pointer. 
 *
 * A request is processed by the agent by calling the 
 * process_request member function of the agent's MIB. Depending of the
 * request type the get_request, get_next_request, or 
 * prepare_set_request, commit_set_request, 
 * undo_set_request, and cleanup_set_request of each target
 * MIB object is called. Because the targets of SNMP requests are managed
 * objects, but the MIB contains only MIB objects the mib object 
 * determines which MIB object manages which managed object. So,
 * process_request calls the appropriate of the above mentioned 
 * functions for each target managed object.
 *
 * When the agent ist multi-threaded the method routines called by a
 * request are executed within the same thread. This thread is then
 * different from the master thread accepting new request.
 *
 * (c) Sending notifications
 * 
 * To send a notification (trap) or inform request, use the notify method.
 * By default Mib uses a NotificationOriginator instance to send 
 * notifications. You can override this behavior by assigning a different
 * NotificationSender with set_notification_sender. 
 *
 * Proxy Forwared Applications
 * ---------------------------
 * Since version 3.4.2 of AGENT++ proxy forwarder applications (including
 * trap forwarding) are supported through the ProxyForwarder class.
 * A proxy forwarder application is registered by calling the register_proxy
 * method. It then can be unregistered with unregister_proxy. Please
 * refer to RFC2573 for more information about proxy forwarding.
 * 
 * @author Frank Fock
 * @version 3.5.22
 */

#if !defined (AGENTPP_DECL_TEMPL_ARRAY_MIBCONFIGFORMAT)
#define AGENTPP_DECL_TEMPL_ARRAY_MIBCONFIGFORMAT
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL Array<MibConfigFormat>;
#endif

#if !defined (AGENTPP_DECL_TEMPL_OIDLIST_MIBCONTEXT)
#define AGENTPP_DECL_TEMPL_OIDLIST_MIBCONTEXT
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL OidList<MibContext>;
#endif
#ifdef _SNMPv3
#ifdef _PROXY_FORWARDER
#if !defined (AGENTPP_DECL_TEMPL_OIDLIST_PROXYFORWARDER)
#define AGENTPP_DECL_TEMPL_OIDLIST_PROXYFORWARDER
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL OidList<ProxyForwarder>;
#endif
#endif
#endif

class AGENTPP_DECL Mib {
#if defined(_USE_PROXY) && !defined(_PROXY_FORWARDER)
friend class MibProxy;
#endif

public:
	/**
	 * Default Constructor
	 */
	Mib();
	/**
	 * Constructor with path where persistent MIB objects should be
	 * stored to and loaded from.
	 * @param path - The directory to store persistent objects.
	 */
	Mib(const NS_SNMP OctetStr&);  

	/**
	 * Destructor
	 */
	virtual ~Mib();

	/**
	 * Set the RequestList to be used for answering request.
	 * Note: The request list is NOT deleted by the receiver's destructor.
	 *       This should be done outside class Mib.
	 *
	 * @param requestList
	 *    a RequestList instance.
	 */
#ifndef STATIC_REQUEST_LIST
	void			set_request_list(RequestList*);
#else
	static void		set_request_list(RequestList*);
#endif
	/**
	 * Gets the request list associated with all Mib instances.
	 * 
	 * @return 
	 *    a RequestList instance.
	 */
#ifndef STATIC_REQUEST_LIST
	RequestList*		get_request_list() { return requestList; }
#else
	static RequestList*	get_request_list() { return requestList; }
#endif
	/**
	  * Process a request. If multi-threading is activated, start a
	  * thread to actually process the request. 
	  * @param req - A request.
	  */
	virtual void		process_request(Request*);      

	/**
	 * Send a notification.
	 *
	 * @param context
	 *    the context originating the notification ("" for the default 
	 *    context).
	 * @param trapoid
	 *    the oid of the notification.
	 * @param vbs
	 *    an array of variable bindings.
	 * @param size
	 *    the size of the above variable binding array.
	 * @param timestamp
	 *    an optional timestamp.
	 * @return 
	 *    SNMP_ERROR_SUCCESS if the notification could be sent
	 *    successfully, otherwise an appropriate SNMP error is
	 *    returned.
	 */
	virtual int		notify(const NS_SNMP OctetStr&, const Oidx&,
				       Vbx*, int, unsigned int=0); 

	/**
	 * Sets the notification sender to be used by this Mib. 
	 * By default a NotificationOriginator is used, which is created
	 * the first time the notify method is called. By setting a different
	 * notification sender before notify is called for the first time,
	 * the set NotificationSender is used instead. 
	 * The set NotificationSender is deleted by the destructor of Mib
	 * or when this method is called again.
	 * 
	 * @param notificationSender
	 *    a pointer to a NotificationSender instance. A previously set
	 *    notificationSender will be deleted. If notificationSender is
	 *    set to 0, any previously set instance will be deleted and 
	 *    the default sender will be used when notify is called again.
	 */
	void	       	set_notification_sender(NotificationSender*);

        /**
	 * Gets the currently set NotificationSender.
	 *
	 * @return
	 *    a pointer to a NotificationSender instance or 0 if it has
	 *    not been set yet.
	 */
	NotificationSender*	get_notification_sender() const 
	    { return notificationSender; } 

	/**
	  * Clean up MIB. Currently, the only clean up procedure is to
	  * call the remove_unused_rows() method of each MibTable object
	  * in the MIB.
	  */
	virtual void		cleanup();      

	/**
	 * Register a MIB object for the default context.
	 *
	 * Note: This method is synchronized using ::lock_mib()
	 *
	 * @param item
	 *    a pointer to a MibEntry object (i.e., MibLeaf,
	 *    MibTable, MibGroup, ...).
	 * @return 
	 *    the input pointer if the MibEntry could be registered, or
	 *    0 otherwise.
	 */
	virtual MibEntry*    	add(MibEntry*);

	/**
	 * Register a MIB object within a given context. If the given 
	 * context is a new context, it will be added to the list of
	 * known contexts.
	 *
	 * Note: This method is synchronized using ::lock_mib()
	 *
	 * @param context
	 *    an OctetStr instance specifiying the target context.
	 * @param item
	 *    a pointer to a MibEntry object (i.e., MibLeaf,
	 *    MibTable, MibGroup, ...).
	 * @return 
	 *    the input pointer if the MibEntry could be registered, or
	 *    0 otherwise.
	 */
	virtual MibEntry*    	add(const NS_SNMP OctetStr&, MibEntry*);

	/**
	 * Add a new context with a given name to the receiver
	 * 
	 * Note: This method is synchronized using ::lock_mib(),
	 * thus you may add a context in within the agent`s mainloop
	 * in a multi-threaded enivornment, but do not use the return
	 * pointer then. The pointer to the MibContext created SHOULD
	 * ONLY be used before you enter the main loop.
	 *
	 * @param contextName
	 *    the name of the new context.
	 * @return
	 *    a pointer to the created and empty MibContext instance.
	 */
	virtual MibContext*     add_context(const NS_SNMP OctetStr&);

	/**
	 * Remove a given context from the receiver.
	 * 
	 * Note: This method is synchronized using ::lock_mib().
	 *
	 * @param contextName
	 *    the name of the new context.
	 */
	virtual void		remove_context(const NS_SNMP OctetStr&);

	/**
	 * Add Agent Capabilities to a context.
	 *
	 * @param context
	 *    a context. An empty string denotes the default context.
	 * @param sysORID
	 *    an authoritative identification of a capabilities statement
         *    with respect to various MIB modules supported by the local
         *    SNMPv2 entity acting in an agent role.
	 * @param sysORDescr
	 *    a textual description of the capabilities identified by the
         *    corresponding instance of sysORID.
	 * @return 
	 *    TRUE if the context could be found and it contains the
	 *    sysOR MIB objects, FALSE otherwise.
	 */
	virtual bool		add_agent_caps(const NS_SNMP OctetStr&, 
					       const Oidx&,
					       const NS_SNMP OctetStr&);

	/**
	 * Remove Agent Capabilities from a context.
	 *
	 * @param context
	 *    a context. An empty string denotes the default context.
	 * @param sysORID
	 *    an authoritative identification of a capabilities statement
         *    with respect to various MIB modules supported by the local
         *    SNMPv2 entity acting in an agent role.
	 */
	virtual void		remove_agent_caps(const NS_SNMP OctetStr&, 
						  const Oidx&);

	/** 
	 * Unregister a MIB object from the agent�s MIB.
	 * This method now (>3.3) works for MibGroups, too.
	 *
	 * Note: This method is synchronized. It first acquires 
	 * ::mib_lock(), then searches for the target MIB object and
	 * then acquires target::start_synch().
	 * Then it RELEASES the MIB object lock by calldelete_requesting 
	 * target::end_synch(). The target object is deleted and the
	 * MIB lock is released.
	 *
	 * Why is this save?
	 * - we hold the lock for the MIB (lock_mib())
	 * - before a request can lock a MibEntry object it
         *   must get the MIB lock
	 * - so if we get the MibEntry lock here no other request
         *   can be currently using this object 
         * - even if we release the lock, because we hold the
         *   MIB lock
	 *
	 * @param oid
	 *    the Oidx instance pointing to the key of the MibEntry
	 *    to be removed.
	 * @return 
	 *    TRUE if the entry could be removed, FALSE otherwise
	 */
	virtual bool		remove(const Oidx&);

	/** 
	 * Unregister a MIB object from the agent�s MIB.
	 * This method now (>3.3) works for MibGroups, too.
	 *
	 * Note: This method is synchronized. It first acquires 
	 * ::mib_lock(), then searches for the target MIB object and
	 * then acquires target::start_synch().
	 * Then it RELEASES the MIB object lock by calling 
	 * target::end_synch(). The target object is deleted and the
	 * MIB lock is released.
	 *
	 * Why is this save?
	 * - we hold the lock for the MIB (lock_mib())
	 * - before a request can lock a MibEntry object it
         *   must get the MIB lock
	 * - so if we get the MibEntry lock here no other request
         *   can be currently using this object 
         * - even if we release the lock, because we hold the
         *   MIB lock
	 *
	 * @param context
	 *    an OctetStr instance specifying the target context.
	 * @param oid
	 *    the Oidx instance pointing to the key of the MibEntry
	 *    to be removed.
	 * @return 
	 *    TRUE if the entry could be removed, FALSE otherwise
	 */
	virtual bool		remove(const NS_SNMP OctetStr&, const Oidx&);

	/**
	 * Initialize all contexts. The main purpose of this method
	 * is to trigger all MIB objects to load their data from 
	 * persistent storage.
     * NOTE: If no thread pool has been assigned until calling this
     * method a default ThreadPool will be created with the default
     * stack size. The number of threads and the thread stack size
     * might not match your requirements. To use your own thread pool,
     * simply call {@link #set_thread_pool()} before calling this method.
	 *
	 * @return
	 *    TRUE if initialization was successful, FALSE otherwise.
	 */
	virtual bool		init(); 

	/**
	 * Save all persistent MIB objects to disk.
	 */
	virtual void		save_all();

	/**
	 * Save all persistent MIB objects in the supplied format to the
	 * supplied path.
	 * @param format
	 *    the format of the persistent data.
	 * @param path
	 *    the path where the data should be stored.
	 * @return
	 *    TRUE if data has been saved successfully, FALSE otherwise.
	 */
	virtual bool		save(unsigned int, const NS_SNMP OctetStr&);

	/**
	 * Load all persistent MIB objects in the supplied format from the
	 * supplied path. All presistent objects will be resetted.
	 * @param format
	 *    the format of the persistent data.
	 * @param path
	 *    the path where the data should be stored.
	 * @return
	 *    TRUE if data has been saved successfully, FALSE otherwise.
	 */
	virtual bool		load(unsigned int, const NS_SNMP OctetStr&);

	/**
	 * Get the MIB object with a given object id.
	 *
	 * @param key
	 *    an object identifier
	 * @return
	 *    a pointer to a MibEntry instance if an object with the 
	 *    given key could be found or 0 otherwise.
	 */
	virtual MibEntryPtr    	get(const Oidx&);

	/**
	 * Get the MIB object with a given object id.
	 *
	 * @param context
	 *    an OctetStr instance specifiying the target context.
	 * @param key
	 *    an object identifier
	 * @return
	 *    a pointer to a MibEntry instance if an object with the 
	 *    given key could be found or 0 otherwise.
	 */
	virtual MibEntryPtr    	get(const NS_SNMP OctetStr&, const Oidx&);

	/**
	 * Find the MibGroup that contains a MibEntry with a given
	 * object ID.
	 * (NOT SYNCHRONIZED)
	 *
	 * @param context
	 *    a context identifier.
	 * @param oid
	 *    a object identifier.
	 * @return 
	 *    a pointer to the MibGroup instance that contains an 
	 *    instance with the given oid, or 0 otherwise.
	 */
	virtual MibGroup*	find_group_of(const NS_SNMP OctetStr&, 
					      const Oidx&);

	/**
	 * Get path where persistent MIB objects are stored.
	 *
	 * @return The path string.
	 */
	virtual NS_SNMP OctetStr       	get_persistent_objects_path() const;

	/**
	 * Set the path where persistent MIB objects are stored.
         *
         * In order to disable persistent objects, call with NULL.
	 *
	 * @param str
	 *    pointer to the the path (this function calls clone() to get
         *    a clean new pointer.
	 */
	virtual void set_persistent_objects_path(const NS_SNMP OctetStr* str);

	/**
	 * Checks whether a persistent objects path has been set.
	 * Since get_persistent_objects_path always returns a path,
	 * calling this method is the only way to determine whether
	 * a path has been explicitly unset, to disable persistent
	 * storage at all.
	 * @return
	 *    TRUE if persistent storage is activated.
	 */
	bool			is_persistency_activated() const
	    { return (persistent_objects_path != 0); }

	/**
	 * Check whether a node (a MIB object) is complex, i.e.
	 * whether the node manages more than one leaf object.
	 *
	 * @param A pointer to a node (MIB object).
	 */
	virtual bool		is_complex_node(const MibEntryPtr&);

	/**
	 * Check whether a node (a MIB object) is a table.
	 *
	 * @param entry
	 *    a pointer to a node (MIB object).
	 */
	bool			is_table_node(const MibEntryPtr& e)
				{ return (e->type() == AGENTPP_TABLE); }

	/**
	 * Check whether a node (a MIB object) is a leaf node.
	 *
	 * @param entry
	 *    a pointer to a node (MIB object).
	 */
	bool			is_leaf_node(const MibEntryPtr& e)
				{ return (e->type() == AGENTPP_LEAF); }

	// Pointer to the singleton instance of the Mib class.
	static Mib*  		instance;

	/**
	 * Pointer to the RequestList used for answering requests
	 */
#ifndef STATIC_REQUEST_LIST
	RequestList*		requestList;  
#else
	static RequestList*	requestList;  
#endif

	/**
	 * Lock the receiver's registration. Thus, no adding nor removing
	 * of MIB objects can be done while the lock is active.
	 *
	 * While processing a (sub-)request the locking scheme is as 
	 * follows: 
	 * 1. This MIB lock is acquired.
	 * 2. The MIB object is identified, which is responsible for
	 *    processing the sub-request. 
	 * 3. The MIB objects lock (MibEntry::start_synch()) is acquired.
	 * 4. The MIB lock is released.
	 * 5. The sub-request is propagated to the MIB object.
	 * 6. When finished, the MIB objects lock is released.
	 */ 
	void		       	lock_mib();

	/**
	 * Release the lock on the receiver's registration.
	 *
	 * @see Mib::lock_mib()
	 */
	void		       	unlock_mib();

	/**
	 * Get a context.
	 *
	 * @param context
	 *    a context.
	 * @return
	 *    a pointer to a MibContext instance, or 0 if no such instance
	 *    exists.
	 */
	virtual MibContext*	get_context(const NS_SNMP OctetStr&);

	/**
	 * Get the default context.
	 *
	 * @return
	 *    a pointer to the default context.
	 */
	MibContext*		get_default_context() { return defaultContext;}

	/**
	 * Find the next MIB entry which has an oid greater than the given oid.
	 * 
	 * @param context
	 *    the target context.
	 * @param oid
	 *    the OID for which a successor OID and managing MibEntry is
         *    searched. 
	 * @param entry 
	 *    will be changed to a pointer to the found entry or remain 
	 *    unchanged if the return value is different from 
	 *    SNMP_ERROR_SUCCESS.
	 * @param request
	 *    a pointer to the Request instance for in whose context the
	 *    find_next is issued.
	 * @param subrequest
	 *    the subrequest number being processed (starting from 0)
         * @param nextOid
         *    returns the successor OID (if found) or a zero length OID 
         *    (if not found) for objects where the successor could be 
         *    determined. Otherwise, the oid will be returned unchanged.
	 * @return 
	 *    SNMP_ERROR_SUCCESS if an entry has been found and 
	 *    sNMP_SYNTAX_NOSUCHOBJECT otherwise.
	 */
	virtual int    	find_next(MibContext*, const Oidx& oid, MibEntryPtr&, 
				  Request* req, const int, Oidx& nextOid);

	/**
	 * Find the MIB object (i.e., MibTable, MibLeaf, etc.) that is 
	 * responsible for the managed object identified by a given oid
	 * within a given context.
	 *
	 * @param context
	 *    the target context.
	 * @param oid
	 *    an object identifier.
	 * @param retval
	 *    changed to a pointer to the found entry or unchanged
	 *    if the return value is different from SNMP_ERROR_SUCCESS.
	 * @param request
	 *    a pointer to the request that is searching. 
	 * @return 
	 *    SNMP_ERROR_SUCCESS if an entry has been found and 
	 *    SNMP_ERROR_NO_SUCH_NAME otherwise.
	 */
	virtual int    	find_managing_object(MibContext*, 
					     const Oidx&, 
					     MibEntryPtr&,
					     Request*);

#ifdef _SNMPv3
#ifdef _PROXY_FORWARDER
	/**
	 * Register a proxy forwarder with the Mib.
	 *
	 * @param proxy
	 *    a pointer to a ProxyForwarder instance. Do not delete it
	 *    because Mib will delete the proxy when unregistering.
	 * @return
	 *    TRUE if the proxy could be registered successfully, if
	 *    a proxy with the same contextEngineID and PDU types is
	 *    already registered, FALSE will be returned. 
	 */
	bool		register_proxy(ProxyForwarder*); 

	/**
	 * Unregister and delete a proxy forwarder.
	 *
	 * @param contextEngineID
	 *    the context engine ID of the proxy.
	 * @param pduType
	 *    the ProxyForwarder::pdu_type of the proxy.
	 */
	void		unregister_proxy(const NS_SNMP OctetStr&, 
					 ProxyForwarder::pdu_type);
	/**
	 * Set the local engine ID used by the proxy forwarder application.
	 * The local engine ID is automatically set if the v3MP has been
	 * initialized before this Mib instance was created.
	 *
	 * @param engineID
	 *    an OctetStr instance denoting the local engine ID. 
	 */
	void    	set_local_engine_id(const NS_SNMP OctetStr& s) { myEngineID=s;}
#endif
#endif
#ifdef AGENTPP_USE_THREAD_POOL
	/**
	 * Set the thread pool to be used by this Mib instance.
	 * The default thread pool is deleted by this operation.
	 * Thus, this method must not be called after entering 
	 * the agent's main loop (i.e., after processing the 
	 * first requests).
	 * The ThreadPool will be deleted when the Mib is deleted.
	 *
	 * @param threadPool
	 *    a ThreadPool instance that provides a number of
	 *    threads for execution of requests.
	 */
	void		set_thread_pool(ThreadPool* pool); 

	/**
	 * Delete the thread pool. This should be done before deleting
	 * this Mib instance and RequestList. The RequestList should then
	 * be deleted before the Mib instance to avoid deadlocks from
	 * unfinished requests.
	 */	 
	void		delete_thread_pool() 
	  { if (threadPool) delete threadPool; threadPool = 0; }

	/**
	 * Get a pointer to the thread pool.
	 *
	 * @return
	 *    a pointer to the Mib's thread pool.
	 */
	ThreadPool*	get_thread_pool() { return threadPool; }
#endif

	/**
	 * Internally process a request (typically 
	 * within its own thread).
	 *
	 * @param req 
	 *    a request.
	 */
        virtual void   	do_process_request(Request*);


	/**
	 * Process a set request.
	 * 
	 * @param rep
	 *    the set request to process.
	 */
	virtual void	process_set_request(Request*);

	/**
	 * Process the prepare phase of a set request.
	 * 
	 * @param rep - The request.
	 * @return SNMP_ERROR_SUCCESS on success or any other SNMP error 
	 *         code on failure.
	 */
	virtual int	process_prepare_set_request(Request*);

	/**
	 * Process the commit phase of a set request.
	 * 
	 * @param rep - The request.
	 * @return SNMP_ERROR_SUCCESS on success or any other SNMP error 
	 *         code on failure.
	 */
	virtual int     process_commit_set_request(Request*);

	/**
	 * Process the undo phase of a set request.
	 * 
	 * @param rep - The request.
	 * @return SNMP_ERROR_SUCCESS on success or any other SNMP error 
	 *         code on failure.
	 */
	virtual int     process_undo_set_request(Request*);

	/**
	 * Process the clean up phase of a set request.
	 * 
	 * @param rep - The request.
	 */
	virtual void    process_cleanup_set_request(Request*);

	/**
	 * Process a get bulk request.
	 * 
	 * @param rep - The request.
	 */
	virtual void    process_get_bulk_request(Request*);

#ifdef _SNMPv3
	/**
	 * Gets the SNMPv3 engine boot counter from permanent storage,
	 * which is by default the file "snmpv3_boot_counter" in the
	 * working directory.
	 * 
	 * @param engineID
	 *    the engine ID of the engine whose boot counter should be
	 *    returned.
	 * @param bootCounter
	 *    returns the boot counter if the returned status is SNMPv3_OK.
	 * @returns
	 *    SNMPv3_OK if no error occurred.
	 */
	virtual int     get_boot_counter(const NS_SNMP OctetStr&, unsigned int&);

	/**
	 * Sets the SNMPv3 engine boot counter and stores it into
	 * permanent storage, which is by default the file 
	 * "snmpv3_boot_counter" in the current working directory. 
	 * 
	 * @param engineID
	 *    the engine ID of the engine whose boot counter should be
	 *    saved.
	 * @param bootCounter
	 *    the boot counter of the above engine ID.
	 * @returns
	 *    SNMPv3_OK if no error occurred.
	 */
	virtual int	set_boot_counter(const NS_SNMP OctetStr&, unsigned int);
#endif

	/**
	 * Adds a config format to this Mib instance. If a format with
	 * the specified ID (see AGENTPP-CONFIG-MIB) is already registered,
	 * then it will be replaced and the old one will be returned.
	 * NOTE: This method is not synchronized and should therefore not be
	 * called after agent initialization.
	 * 
	 * @param formatID
	 *    an unsigned integer > 0 that specifies the format as defined
	 *    in the AGENTPP-CONFIG-MIB or any supplemental MIB specification.
	 * @param format
	 *    the MibConfigFormat instance implementing the format.
	 * @return
	 *    the MibConfigFormat instance pointer provided as format parameter.
	 */
	MibConfigFormat* add_config_format(unsigned int format, 
					   MibConfigFormat*);

        /**
	 * Returns the config format for the specified format ID.
	 * @param formatID
	 *    the format ID.
	 * @return
	 *    the MibConfigFormat instance registered for the supplied
	 *    formatID, or NULL if such a format is not registered.
	 */
	MibConfigFormat* get_config_format(unsigned int format) 
	    { return (configFormats.getNth(format-1)); }

protected:

	/**
	 * Set the exception status for the supplied sub-request.
	 * @param req
	 *    a pointer to a Request.
	 * @param reqind
	 *    the index of the sub-request to be modified.
	 * @return
	 *    TRUE if the request can be further processed (thus it is 
	 *    not necessarily finished yet) or FALSE if the it has to
	 *    be finished.
	 */
	static bool	set_exception_vb(Request*, int, int);

	/**
	 * Process a subrequest
	 * @param req A pointer to the request
	 * @return FALSE if an error occured and the whole request finished
	 */
	virtual bool process_request(Request*, int); 

	// (only GET and GETNEXT subrequests can be processed independently)

	/**
	 * Finalize a request. The finalize method is called if all
	 * sub-requests have been successfully processed.
	 *
	 * @param request
	 *    the request to finalize.
	 */
	virtual void	finalize(Request*);

	/**
	 * Deletes the the supplied request. This method can be overwritten
	 * by sub-classes to implement special processing before a request
	 * object is freed. This method is called at the end of the finalize
	 * method.
	 * @param req
	 *    a pointer to the Request instance to delete.
	 * @since 3.5.23
	 */ 
	virtual void    delete_request(Request* req) { delete req; }

#ifdef _SNMPv3
	/**
	 * Check access rights for GETNEXT/GETBULK requests in the SNMPv3
	 * security model.
	 *
	 * @param req - A pointer to the corresponding GETNEXT/BULK request.
	 * @param entry - Returns the entry that is the next accessible in
	 *                the current view.
	 * @param oid - Returns the oid of the object for that access has 
	 *              been denied or granted.
         * @param nextOid - Provides the already determined next OID for complex
         *    mib entries. If nextOid has the zero length, it will be ignored.
	 * @return VACM_accessAllowed if access is granted and any other
	 *         VACM error code if access is denied.
	 */
	virtual int	next_access_control(Request*, 
					    const MibEntryPtr, Oidx&, 
                                            const Oidx&); 

#ifdef _PROXY_FORWARDER
	virtual void	proxy_request(Request*);
#endif
#endif

	OidList<MibContext>		contexts;
	MibContext*			defaultContext;

	NotificationSender*		notificationSender;

        NS_SNMP OctetStr*	       	persistent_objects_path;
#ifdef _THREADS
	ThreadManager			mibLock;
#endif
#ifdef _SNMPv3
	NS_SNMP OctetStr       	       	bootCounterFile;
#ifdef _PROXY_FORWARDER
	NS_SNMP OctetStr	       	myEngineID;
	OidList<ProxyForwarder>		proxies;
#endif
#endif

#ifdef AGENTPP_USE_THREAD_POOL
	ThreadPool*			threadPool;
#endif

	Array<MibConfigFormat>		configFormats;

 private:
	void				construct(const NS_SNMP OctetStr& path);
};

#ifdef _THREADS
#ifndef AGENTPP_USE_THREAD_POOL
#ifdef _WIN32THREADS
void mib_method_routine_caller(void*);
#else
void* mib_method_routine_caller(void*);
#endif
#endif
#endif

inline void Mib::lock_mib() 
{
#ifdef _THREADS
	mibLock.start_synch();
#endif
}

inline void Mib::unlock_mib() 
{
#ifdef _THREADS
	mibLock.end_synch();
#endif
}

#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
