/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - mib_context.h  
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

#ifndef _mib_context_h_
#define _mib_context_h_

#include <agent_pp/threads.h>
#include <agent_pp/snmp_pp_ext.h>
#include <agent_pp/List.h>
#include <agent_pp/mib_entry.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


/*--------------------------- class MibGroup --------------------------*/

/**
 * The MibGroup class is an encapsulation for a collection of MibEntry 
 * objects. MibGroup can be used to group a collection of
 * MIB objects logically. If such a MibGroup object is added to a
 * Mib instance it is flattened, which means each MibEntry
 * object within that group object will be added to the Mib instance. 
 * The group object itself will be added to a list of groups. 
 * If a MibGroup instance is deleted, the contained MibEntry instances
 * are NOT deleted. Instead, those instances are deleted by the destructor
 * of the Mib instance the group has been added to.
 *
 * NOTE: A MibGroup must not contain other MibGroup instances.
 *
 * @author Frank Fock
 * @version 3.5.15
 */
#if !defined (AGENTPP_DECL_TEMPL_LIST_MIBENTRY)
#define AGENTPP_DECL_TEMPL_LIST_MIBENTRY
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL List<MibEntry>;
#endif

class AGENTPP_DECL MibGroup: public MibEntry {
friend class Mib;  
public:
	/**
	 * Construct a MibGroup with a given OID which has to uniquely
	 * identify the group.
	 * 
	 * In version < 3.5.15 there was a check that ensured that only
	 * objects with an OID in the subtree of the group could have
	 * been added to it. This check has been removed because it
	 * has no real benefit but some drawbacks.
	 *
	 * All objects added to this group will not be persistently
	 * stored or loaded.
	 *
	 * @param o - The oid of the new MibGroup.
	 */ 
	MibGroup(const Oidx&);

	/**
	 * Construct a MibGroup with a given OID which has to uniquely
	 * identify the group.
	 * All objects can be stored/loaded to/from persistent 
	 * storage using the given identification name.
	 *
	 * In version < 3.5.15 there was a check that ensured that only
	 * objects with an OID in the subtree of the group could have
	 * been added to it. This check has been removed because it
	 * has no real benefit but some drawbacks.
	 *
	 * Whether a particular object is stored/loaded to/from
	 * disk depends on the value its is_volatile method returns
	 * false.
	 *
	 * @param o
	 *    the oid of the new MibGroup.
	 * @param persitentName
	 *    determines under which name the group should be stored 
	 *    persistently.
	 */ 
	MibGroup(const Oidx&, const NS_SNMP OctetStr&);

	/**
	 * Destructor (does not delete contained MibEntry instances) 
	 */
	virtual ~MibGroup();

	 /**
	  * Return the type of the receiver.
	  *
	  * @return GROUP
	  */
	mib_type		type() const;

	/**
	 * Add a MibEntry object to the receiver group.
	 *
	 * @param item
	 *    a pointer to the MibEntry instance to be added.
	 */
	MibEntryPtr		add(MibEntryPtr item);	

	/**
	 * Remove a MibEntry object from the receiver group and delete it.
	 *
	 * @note Do not call this function after the MibGroup was added to
	 *       a Mib.
	 *
	 * @param key
	 *    the oid (key) of the MibEntry instance to be removed.
	 */
	void			remove(const Oidx&);
	
	
	/**
	 * Get content.
	 *
	 * @return
	 *    a ListCursor at the first element of the receiver.
	 */
	ListCursor<MibEntry>	get_content();

	/**
	 * Free content. Deletes all contained MIB objects and removes
	 * them from this group.
	 *
	 * @note Do not call this function after the MibGroup was added to
	 *       a Mib. If the MibGroup object was not added to a Mib,
	 *       this function must be called before deleting the object.
	 */
	void			clearAll();

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
	 * Return whether objects in this group are persistent or not.
	 *
	 * @return
	 *    TRUE if objects of this group should be stored persistently,
	 *    FALSE otherwise.
	 */
	bool			is_persistent() { return (persistencyName!=0);}

	NS_SNMP OctetStr       	get_persistency_name()
					{ return *persistencyName; }

	/**
	 * Set the timeout value characteristics for this group
	 * of MIB objects. Within AGENT++ this value is currently
	 * not used, but AgentX++ is using this value to set
	 * the region timeout when registering a subagent. Future
	 * implementations may also use this value for setting
	 * region (or subtree) timeouts.
	 *
	 * @param timeout
	 *    a timeout value in seconds. 0 sets the systems default
	 *    timeout.
	 * @since 3.5
	 */
	void			set_timeout(unsigned int t) { timeout = t; }

	/**
	 * Get the timeout (in seconds) for this MIB group.
	 *
	 * @return
	 *    a timeout value in seconds or 0 if default timeout should
	 *    be used.
	 * @since 3.5
	 */ 
	unsigned int		get_timeout() { return timeout; }

protected:

	List<MibEntry>	content;
	NS_SNMP OctetStr*      	persistencyName;
	unsigned int	timeout;
};


/*--------------------------- class MibContext --------------------------*/

/**
 * The MibContext class is a collection class of MibEntry instances, that
 * reside in the same (named) context. The default context is identified by
 * an empty string. 
 * 
 * The MibEntry instances within an context can be loaded/stored from/to 
 * persitent storage. Whenever a context is loaded from disk by calling
 * the init_from method, the context's content is stored to disk to the
 * same location when the context is deleted.
 *
 * @author Frank Fock
 * @version 3.5.9
 */
#if !defined (AGENTPP_DECL_TEMPL_OIDLIST_MIBENTRY)
#define AGENTPP_DECL_TEMPL_OIDLIST_MIBENTRY
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL OidList<MibEntry>;
#endif
#if !defined (AGENTPP_DECL_TEMPL_OIDLIST_MIBGROUP)
#define AGENTPP_DECL_TEMPL_OIDLIST_MIBGROUP
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL OidList<MibGroup>;
#endif

class AGENTPP_DECL MibContext: public ThreadManager
{
 public:
	/**
	 * Default constructor. Creates a default context.
	 */
	MibContext();

	/**
	 * Create a given context.
	 *
	 * @param context
	 *    an OctetStr instance identifying the context to be created.
	 */
	MibContext(const NS_SNMP OctetStr&);

	/**
	 * Destructor.
	 */
	virtual ~MibContext();

	/**
	 * Initialize context, thus load initialize groups with data
	 * from persistent storage.
	 *
	 * @param path
	 *    where data should be loaded from.
	 * @return
	 *    TRUE if initialization was successful, FALSE otherwise.
	 */
	virtual bool		init_from(const NS_SNMP OctetStr&);

	/**
	 * Loads persistent data from the supplied path. Any previously
	 * existing data will be deleted before loading.
	 *
	 * @param path
	 *    where data should be loaded from.
	 * @return
	 *    TRUE if initialization was successful, FALSE otherwise.
	 */
	virtual bool		load_from(const NS_SNMP OctetStr&);

	/**
	 * Save management data to persistent storage. 
	 * NOTE: This method is called by the receiver's destructor 
	 * with the path used by init_from. 
	 *
	 * @param path
	 *    where data should be written to.
	 * @return
	 *    TRUE if data could be saved successfully, FALSE otherwise.
	 */
	virtual bool		save_to(const NS_SNMP OctetStr&);

	/**
	 * Return a key value for the receiver context.
	 *
	 * @return
	 *    a pointer to an Oidx instance.
	 */
	OidxPtr			key();

	/**
	 * Register a MIB object for the context.
	 * (SYNCHRONIZED)
	 *
	 * @param item
	 *    a pointer to a MibEntry object (i.e., MibLeaf,
	 *    MibTable, MibGroup, ...).
	 * @return 
	 *    the input pointer, or 0 if the object could not be 
	 *    registered.
	 */
	virtual MibEntry*    	add(MibEntry*);

	/** 
	 * Unregister a MIB object from the context.
	 * This method does not work for MibGroups, instead you have
	 * to remove each MibGroup's MIB object seperately.
	 * (SYNCHRONIZED)
	 *
	 * @param oid 
	 *    the oid of the object to be removed.
	 * @return 
	 *    a pointer to the removed object, or 0 if such an object
	 *    could not be found.
	 */
	virtual MibEntry*    	remove(const Oidx&);

	/**
	 * Get the MIB object with a given object id.
	 *
	 * @param key
	 *    an object identifier
	 * @return
	 *    a pointer to a MibEntry instance if an object with the 
	 *    given key could be found or 0 otherwise.
	 */
	virtual MibEntry*    	get(const Oidx&);

	/**
	 * Get the MIB entry with a given object id if such an entry
	 * exists or otherwise an object near to the given oid.
	 *
	 * @param key
	 *    an object identifier
	 * @return
	 *    a pointer to a MibEntry instance if an object could be 
	 *    found or 0 otherwise.
	 */
	virtual MibEntry*    	seek(const Oidx&);

	/**
	 * Find a MIB entry with a given oid.
	 *
	 * @param oid
	 *    an object identifier.
	 * @param entry
	 *    changed to a pointer to the found entry or unchanged
	 *    if the return value is different from SNMP_ERROR_SUCCESS.
	 * @return 
	 *    SNMP_ERROR_SUCCESS if an entry has been found and 
	 *    SNMP_ERROR_NO_SUCH_NAME otherwise.
	 */
	virtual int		find(const Oidx&, MibEntryPtr&);

	/**
	 * Find a MIB entry with an oid less or equal to a given oid.
	 * 
	 * @param oid
	 *    an object identifier.
	 * @param entry
	 *    changed to a pointer to the found entry or unchanged
	 *    if the return value is different from SNMP_ERROR_SUCCESS.
	 * @return 
	 *    SNMP_ERROR_SUCCESS if an entry has been found and 
	 *    SNMP_ERROR_NO_SUCH_NAME otherwise.
	 */
	virtual int		find_lower(const Oidx&, MibEntryPtr&);

	/**
	 * Find a MIB entry with an oid greater or equal to a given oid.
	 * 
	 * @param oid
	 *    an object identifier.
	 * @param entry
	 *    changed to a pointer to the found entry or unchanged
	 *    if the return value is different from SNMP_ERROR_SUCCESS.
	 * @return 
	 *    SNMP_ERROR_SUCCESS if an entry has been found and 
	 *    SNMP_ERROR_NO_SUCH_NAME otherwise.
	 */
	virtual int		find_upper(const Oidx&, MibEntryPtr&);

	/**
	 * Find the successor of a given MibEntry.
	 * 
	 * @param oid
	 *    an object identifier of an existing MibEntry. If the
	 *    receiver doesnot contain such an object, 0 will be returned.
	 * @return 
	 *    a pointer to the successor MibEntry if the specified object 
	 *    could be found, 0 otherwise.
	 */
	virtual MibEntry*    	find_next(const Oidx&);

	/**
	 * Get a OidListCursor at the first element of the receiver.
	 *
	 * @return
	 *    an OidListCursor instance.
	 */
	OidListCursor<MibEntry>		get_content();

	/**
	 * Get a Cursor at the first element of registered groups.
	 *
	 * @return
	 *    a ListCursor instance.
	 */
	OidListCursor<MibGroup>		get_groups();

	/**
	 * Find a group with a given OID.
	 * (NOT SYNCHRONIZED)
	 *
	 * @param oid
	 *    a group's oid.
	 * @return
	 *    a pointer a MibGroup instance, or 0 if no such group exists.
	 */
	virtual MibGroup*		find_group(const Oidx&);

	/**
	 * Remove a group and all its entries from the context.
	 * (SYNCHRONIZED)
	 *
	 * @param oid
	 *    a group's oid.
	 * @return
	 *    TRUE if such a group could be found (and removed),
	 *    FALSE otherwise.
	 */
	virtual bool			remove_group(const Oidx&);

	/**
	 * Find the MibGroup that contains a MibEntry with a given
	 * object ID.
	 *
	 * @param oid
	 *    a object identifier.
	 * @return 
	 *    a pointer to the MibGroup instance that contains an 
	 *    instance with the given oid, or 0 otherwise.
	 */
	virtual MibGroup*	find_group_of(const Oidx&);

	/**
	 * Get the name of the context.
	 *
	 * @return 
	 *    an OctetStr instance.
	 */
	NS_SNMP OctetStr		get_name() { return context; }

	/**
	 * Get the number of MibGroup instances in this context.
	 *
	 * @return
	 *    the number of MibGroups in this context.
	 */
	unsigned int		get_num_groups() const
					{ return groups.size(); }
	
	/**
	 * Get the nth group of the MibGroup instances in this
	 * context.
	 *
	 * @param n
	 *    a 0-based index to the MibGroup to return.
	 * @return
	 *    a pointer to the n-th MibGroup of this context or
	 *    0 if n is out of range.
	 */
	MibGroup*		get_group(int n)
	    { return ((n>=0) && (n<groups.size())) ? groups.getNth(n) : 0; } 

	/**
	 * Get the number of entries in this context.
	 *
	 * @return
	 *    the number of entries in this context.
	 */
	unsigned int		get_num_entries() const
					{ return content.size(); }

 protected:
	OidList<MibEntry>		content;
	OidList<MibGroup>      		groups;	
	Oidx				contextKey;
	NS_SNMP OctetStr		context;
	NS_SNMP OctetStr*		persistencyPath;
};

#ifdef AGENTPP_NAMESPACE
}
#endif


#endif
