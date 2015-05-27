/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - mib.cpp  
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

#ifdef _AIX
#include <rapi.h>
#endif

#include <agent_pp/mib.h>
#include <agent_pp/system_group.h>
#include <agent_pp/snmp_counters.h>
#include <agent_pp/snmp_group.h>
#include <agent_pp/notification_originator.h>
#include <agent_pp/vacm.h>
#include <snmp_pp/log.h>

#ifdef _USE_PROXY
#include <agent_pp/mib_proxy.h>
#include <agent_pp/proxy_forwarder.h>
#endif

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

static const char *loggerModuleName = "agent++.mib";

/*--------------------------------------------------------------------
 *
 * MibLeaf
 *
 */



MibLeaf::MibLeaf()
{
	init(0, VMODE_NONE);
	validity = 0;
}

MibLeaf::MibLeaf(const Oidx& o,
		 mib_access  a,
		 SmiUINT32   s): MibEntry(o, a)
{
	init(0, VMODE_NONE);
	set_syntax(s);
}

MibLeaf::MibLeaf(const Oidx& o,
		 mib_access  a,
		 SnmpSyntax* s): MibEntry(o, a)
{
	init(s, VMODE_NONE);
}


MibLeaf::MibLeaf(const Oidx& o,
		 mib_access  a, SnmpSyntax* s, int mode): MibEntry(o, a)
{
	init(s, mode);
}

/**
 * Copy Constructor
 */

MibLeaf::MibLeaf(const MibLeaf& other): MibEntry(other.oid, other.access)
{
	ListCursor<MibEntry> cur;
	for (cur.init(&other.notifies); cur.get(); cur.next())
		notifies.add(cur.get());

	if (other.value)
		value  	= other.value->clone();
	else	value	= 0;
	value_mode	= other.value_mode;

	validity	= other.validity;
	// ATTENTION: table and row probably have to be adjusted later!
	my_table	= other.my_table;
	my_row		= other.my_row;
	undo            = 0;
}

/**
 * Destructor
 */

MibLeaf::~MibLeaf()
{
	free_value();
}

void MibLeaf::init(SnmpSyntax* s, int mode)
{
	validity = 0;
	value = s;
	value_mode = mode;
	if (mode & VMODE_DEFAULT)
		validity |= LEAF_VALUE_INITIALIZED;
	my_table = 0;
	my_row   = 0;
	undo	 = 0;
}

/**
 * Return the type of the receiver.
 *
 * @return AGENTPP_LEAF
 */

mib_type MibLeaf::type() const
{
	return AGENTPP_LEAF;
}

/**
 * Clone the receiver object.
 *
 * @return A pointer to the clone.
 */

MibEntryPtr MibLeaf::clone()
{
	return new MibLeaf(*this);
}

/**
 * Get the syntax of the receiver's value.
 *
 * @return The syntax of a MibLeaf object's value or sNMP_SYNTAX_NULL,
 *         if the object has no value.
 */

SmiUINT32 MibLeaf::get_syntax() const
{
	return (value == 0) ? sNMP_SYNTAX_NULL : value->get_syntax();
}

/**
 * Set the syntax of the receiver's value and delete the old value.
 *
 * @param syntax - The syntax of a MibLeaf object's value.
 */

void MibLeaf::set_syntax(SmiUINT32 syntax)
{
        if (value) delete value; // setting to SNMP_SYNTAX_NULL
	value = 0;

        switch (syntax) {
        case sNMP_SYNTAX_INT32:
                value = new SnmpInt32();
                break;
        case sNMP_SYNTAX_TIMETICKS:
                value = new TimeTicks();
                break;
        case sNMP_SYNTAX_CNTR32:
                value = new Counter32();
                break;
        case sNMP_SYNTAX_GAUGE32:
                value = new Gauge32();
                break;
        case sNMP_SYNTAX_CNTR64:
                value = new Counter64();
                break;
        case sNMP_SYNTAX_OCTETS:
        case sNMP_SYNTAX_BITS:
                value = new OctetStr();
                break;
        case sNMP_SYNTAX_OPAQUE:
                value = new OpaqueStr();
                break;
        case sNMP_SYNTAX_IPADDR:
                value = new IpAddress();
                break;
        case sNMP_SYNTAX_OID:
                value = new Oid();
                break;
        case sNMP_SYNTAX_NULL:
        case sNMP_SYNTAX_NOSUCHINSTANCE:
        case sNMP_SYNTAX_NOSUCHOBJECT: case sNMP_SYNTAX_ENDOFMIBVIEW:
                break;
        case sNMP_SYNTAX_SEQUENCE:
                break;
        }
}

/**
 * Set the value of the receiver by value.
 *
 * @param v - The value.
 */

void MibLeaf::set_value(const SnmpSyntax& v)
{
	if (value) delete value;
	value = v.clone();
	validity |= LEAF_VALUE_INITIALIZED;
}

/**
 * Set the value of the receiver by reference.
 *
 * @param v - A pointer to the new value.
 */

void MibLeaf::replace_value(SnmpSyntax* v)
{
	if (value) delete value;
	value = v;
	validity |= LEAF_VALUE_INITIALIZED;
}

/**
 * Set the integer value of the receiver.
 *
 * @param l - The new integer value.
 */
void MibLeaf::set_value(const unsigned long l)
{
	set_syntax(sNMP_SYNTAX_INT32);
	*((SnmpInt32*)value) = l;
	validity |= LEAF_VALUE_INITIALIZED;
}

int MibLeaf::set_value(const Vbx& vb)
{
	if (vb.valid() && (vb.get_oid() == get_oid()))
		if (vb.get_syntax() == get_syntax()) {
			replace_value(vb.clone_value());
			return SNMP_ERROR_SUCCESS;
		}
		else return SNMP_ERROR_WRONG_TYPE;

	else return SNMP_ERROR_BAD_VALUE;
}

Vbx MibLeaf::get_value() const
{
	Vbx vb(get_oid());
	if (value)
		vb.set_value(*value);

	return vb;
}

void MibLeaf::free_value()
{
	if (value)
		delete value;
	value = 0;
	validity &= ~LEAF_VALUE_INITIALIZED;
}

bool MibLeaf::serialize(char*& buf, int& sz)
{
	Vbx vb(get_oid());
	vb.set_value(*value);
	return (Vbx::to_asn1(&vb, 1, (unsigned char*&)buf, sz) ==
		SNMP_CLASS_SUCCESS) ? TRUE : FALSE;
}

bool MibLeaf::deserialize(char* buf, int& sz)
{
	Vbx* vbs;
	int size = 0;
	unsigned char* data = (unsigned char*)buf;
	int status = Vbx::from_asn1(vbs, size, data, sz);
	if (status == SNMP_CLASS_SUCCESS)
	{
	    if (size > 0)
	    {
		free_value();
		init(vbs[0].clone_value(), value_mode);
		delete[] vbs;
		return TRUE;
	    }
	    else
	    {
		delete [] vbs;
		return FALSE;
	    }
	}
	return FALSE;
}

void MibLeaf::get_request(Request* req, int ind)
{
	if (get_access() >= READONLY) {
		if ((my_table) && (valid() == 0)) {
			Vbx vb(req->get_oid(ind));
			vb.set_syntax(sNMP_SYNTAX_NOSUCHINSTANCE);
			req->finish(ind, vb);
		}
		else {
			req->finish(ind, get_value());
		}
	}
	else {
	    req->error(ind, SNMP_ERROR_NO_ACCESS);
	}
}

void MibLeaf::get_next_request(Request* req, int ind)
{
	// get_next_request is almost the same as the get_request
	// because the 'next' functionality has been already performed
	// by the mib class get_next_request
	get_request(req, ind);
}

int MibLeaf::commit_set_request(Request* req, int ind)
{
	// set the value. set is a wrapper for set_value unless
	// set is overwritten by subclasses
	int status = set(req->get_value(ind));
	if (status != SNMP_ERROR_SUCCESS) {
		return SNMP_ERROR_COMITFAIL;
	}
	// do not send answer until cleanup finished
	req->finish(ind, req->get_value(ind));

	// notify other classes of change this value
	notify_change(get_oid(), CHANGE);
	return SNMP_ERROR_SUCCESS;
}

int MibLeaf::prepare_set_request(Request* req, int& ind)
{
	if (get_access() >= READWRITE) {
		if (value->get_syntax() == req->get_value(ind).get_syntax()) {
			if (value_ok(req->get_value(ind))) {
			  // check if column must be locked while rowStatus is
			  // active
				if ((is_locked()) && (my_row) &&
				    (my_row->get_row_status())) {
					if (my_row->get_row_status()->get() !=
					    rowActive)
					  return SNMP_ERROR_SUCCESS;
					else
					  return SNMP_ERROR_INCONSIST_VAL;
				}
				return SNMP_ERROR_SUCCESS;
			}
			else return SNMP_ERROR_WRONG_VALUE;
		}
		else return SNMP_ERROR_WRONG_TYPE;
	}
	return SNMP_ERROR_NOT_WRITEABLE;
}

int MibLeaf::undo_set_request(Request*, int&)
{
	return (unset()==SNMP_ERROR_SUCCESS) ?
	  SNMP_ERROR_SUCCESS : SNMP_ERROR_UNDO_FAIL;
}

int MibLeaf::set(const Vbx& vb)
{
	undo = value->clone();
	return set_value(vb);
}

int MibLeaf::unset()
{
	if (undo) {
		delete value;
		value = undo;
		undo = 0;
	}
	return SNMP_ERROR_SUCCESS;
}

void MibLeaf::cleanup_set_request(Request*, int&)
{
	if (undo) {
		delete undo;
		undo = 0;
	}
}

#if 0
Oidx MibLeaf::get_oid() const
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
#endif

/*--------------------------------------------------------------------
 *
 * snmpRowStatus
 *
 */

snmpRowStatus::snmpRowStatus(const Oidx& o):
  MibLeaf(o, READCREATE, new SnmpInt32(rowEmpty))
{
}

/**
 * Construct a snmpRowStatus instance.
 *
 * @param o - The row status column subidentifier.
 * @param a - The maximum access rights for the managed object.
 */

snmpRowStatus::snmpRowStatus(const Oidx& o, mib_access a):
  MibLeaf(o, a, new SnmpInt32(rowEmpty))
{
}

snmpRowStatus::~snmpRowStatus()
{
}

/**
 * Clone the receiver.
 *
 * @return A pointer to the clone.
 */

MibEntryPtr snmpRowStatus::clone()
{
	snmpRowStatus* ptr = new snmpRowStatus(oid, access);
	ptr->set_reference_to_table(my_table);
	return ptr;
}

int snmpRowStatus::prepare_set_request(Request* req, int& ind)
{
	if (!value_ok(req->get_value(ind)))
		return SNMP_ERROR_INCONSIST_VAL;
	return SNMP_CLASS_SUCCESS;
}


bool snmpRowStatus::value_ok(const Vbx& v)
{
	int l;
	if (v.get_value(l) != SNMP_CLASS_SUCCESS) return FALSE;

	return ((l == rowCreateAndGo) ||
		(l == rowCreateAndWait) ||
		(l == rowDestroy) ||
		(l == rowActive) ||
		(l == rowNotInService));
}

bool snmpRowStatus::transition_ok(const Vbx& v)
{
	int l;
	if (v.get_value(l) != SNMP_CLASS_SUCCESS) return FALSE;

	if (value) {
		switch (get()) {
		case rowEmpty:
		  return ((l == rowCreateAndGo) ||
			  (l == rowCreateAndWait) || (l == rowDestroy));
		case rowNotReady:
		  return ((l == rowDestroy) || (l == rowActive) ||
			  (l == rowNotInService));
		case rowActive:
		  return ((l == rowActive) ||
			  (l == rowNotInService) || (l == rowDestroy));
		case rowNotInService:
		  return ((l == rowNotInService) ||
			  (l == rowActive) || (l == rowDestroy));
		default:
		  return (l == rowDestroy);
		}
	}
	else
		return ((l == rowCreateAndGo) || (l == rowCreateAndWait) ||
			(l == rowDestroy));
}

/**
 * Check whether the state of the receiver's row may be changed.
 *
 * @param v - A variable binding that holds the requested new state.
 * @return TRUE if the requested state can be set, otherwise FALSE.
 */

bool snmpRowStatus::check_state_change(const Vbx& v, Request* req)
{
	int l;
	if (!req || v.get_value(l) != SNMP_CLASS_SUCCESS) return FALSE;

	if (value) {
		switch (get()) {
		case rowNotInService:
		case rowNotReady: {
		  if ((l == rowActive) || (l == rowNotInService)) {

			Vbx* pvbs = new Vbx[my_row->size()];
			my_row->get_vblist(pvbs, my_row->size());
			// if req is given collect all subrequest affecting
			// this row
			for (int i=0; i<req->subrequests(); i++) {
			  Oidx id(req->get_oid(i));
			  if ((my_table->base(id) == *my_table->key()) &&
			      (my_table->index(id) == my_row->get_index())) {
				  int col = my_row->index_of(id);
				  pvbs[col] = req->get_value(i);
			  }
			}
			bool ok = my_table->ready(pvbs, my_row->size(),
						     my_row);
			delete[] pvbs;

			LOG_BEGIN(loggerModuleName, DEBUG_LOG | 4);
			LOG("snmpRowStatus: row is");
			LOG(ok ? "READY" : "NOT READY");
			LOG_END;

			return ok;
		  }
		  else if (l == rowDestroy) {
			if (transition_ok(v)) {
				my_table->delete_rows.add(my_row);
				return TRUE;
			}
			return FALSE;
		  }
		  else
		  return transition_ok(v);
		}
		default:
		  return transition_ok(v);
		}
	}
	else
		return ((l == rowCreateAndGo) ||
			(l == rowCreateAndWait) ||
			(l == rowDestroy));
}

/**
 * Set the receiver's value and backup its old value for a later undo.
 *
 * @param vb - The variable binding that holds the new value.
 * @return SNMP_ERROR_SUCCESS if the new value has been set,
 *         SNMP_ERROR_WRONG_TYPE or SNMP_ERROR_BAD_VALUE otherwise.
 */

int snmpRowStatus::set(const Vbx& vb)
{
	if (undo) delete undo;  // paranoia? just to be sure ;-)
	undo = value->clone();
	int rs;
	if (vb.get_value(rs) != SNMP_CLASS_SUCCESS)
	    return SNMP_ERROR_WRONG_TYPE;
	switch (rs) {
	case rowNotInService: {
	  set_value(rowNotInService);
	  my_table->fire_row_changed(rs, my_row, my_row->get_index());
	  break;
	}
	case rowActive:
	case rowCreateAndGo:
	  set_value(rowActive);
	  my_table->fire_row_changed(rowActive, my_row, my_row->get_index());
	  break;
	case rowCreateAndWait:
	  set_value(rowNotReady);
	  break;
	case rowDestroy:
	  set_value(rs);
	  delete undo; // No undo of row destroy!
	  undo = 0;
	  break;
	default:
	  set_value(rs);
	}
	return SNMP_ERROR_SUCCESS;
}

/**
 * Undo a previous set.
 *
 * @return SNMP_ERROR_SUCCESS on success and SNMP_ERROR_UNDO_FAIL on failure.
 */

int snmpRowStatus::unset()
{
  if (undo)
  {
	int rs;
	rs = *(SnmpInt32*)undo;

	switch (rs) {
	case rowEmpty:
		if ((get() == rowActive) || (get() == rowCreateAndGo))
		  my_table->fire_row_changed(rowDestroy,
					     my_row, my_row->get_index());
		if (value) delete value;
		value = undo;
		undo = 0;
		break;
	case rowActive:
		my_table->fire_row_changed(rowActive,
					   my_row, my_row->get_index());
		if (value) delete value;
		value = undo;
		undo = 0;
		break;
	case rowNotInService:
	case rowNotReady:
		if (get() == rowActive)
		  my_table->fire_row_changed(rowNotInService,
					     my_row, my_row->get_index());
		if (value) delete value;
		value = undo;
		undo = 0;
		break;
	case rowCreateAndGo: {
		set_value(rowNotReady);
		delete undo;
		undo = 0;
		Vbx vbx;
		vbx.set_value(rowActive);
		if (check_state_change(vbx))
		  my_table->fire_row_changed(rs, my_row, my_row->get_index());
		else
		  my_table->fire_row_changed(rowDestroy,
					     my_row, my_row->get_index());
		break;
	}
	case rowCreateAndWait:
		if (get() == rowActive)
		  my_table->fire_row_changed(rowDestroy,
					     my_row, my_row->get_index());
		set_value(rowNotReady);
		delete undo;
		undo = 0;
		break;
	default:
		if (value) delete value;
		value = undo;
		undo = 0;
	}
  }
  return SNMP_ERROR_SUCCESS;
}

/**
 * Get the receiver's value.
 *
 * @return The row status of the receiver's row.
 */

long snmpRowStatus::get()
{
	return ((int)*((SnmpInt32*)value));
}




/*--------------------------------------------------------------------
 *
 * MibTableRow
 *
 */

/**
 * Default constructor
 */

MibTableRow::MibTableRow()
{
	base  = "";
	index = "";
	row_status = 0;
}

/**
 * Construct a row with specified base.
 *
 * @param b - A base object identifier (the object identifier of the table's
 *            entry object type).
 */

MibTableRow::MibTableRow(const Oidx& b)
{
	base = b;
	index = "";
	row_status = 0;
}

/**
 * Copy Constructor
 *
 * @param other - Another MibTableRow instance.
 */

MibTableRow::MibTableRow(const MibTableRow& other)
{
	row_status = 0;
#ifdef USE_ARRAY_TEMPLATE
	ArrayCursor<MibLeaf> cur;
#else
	OrderedListCursor<MibLeaf> cur;
#endif
	for(cur.init(&other.row); cur.get(); cur.next()) {
	  //		if (cur.get()->get_access() != NOACCESS)
			// Attention! Cast to MibLeaf* avoids special
			// handling of snmpRowStatus objects. So we must
			// check it manually.
			if ((other.row_status) &&
			    (cur.get() == other.row_status)) {
				row_status = add((snmpRowStatus*)
						 cur.get()->clone());
			}
			else {
				MibLeaf* leaf = (MibLeaf*)cur.get()->clone();
				// a cloned leaf is not initialized by default
				if ((leaf->get_access() == READCREATE) &&
				    (!leaf->has_default())) {
				    leaf->set_validity(0);
				}
				add(leaf);
			}
	}
	base = other.base;
	// row_status MUST NOT be set around here
	set_index(other.index);
}

/**
 * Destructor - destroys the row and all the MibLeaf objects it contains.
 */

MibTableRow::~MibTableRow()
{
}

MibTableRow* MibTableRow::clone()
{
	return new MibTableRow(*this);
}

/**
 * Append a MibLeaf instance to the end of the receiver row.
 *
 * @param l - A pointer to the MibLeaf object to append.
 * @return The pointer to the added MibLeaf object (always the same as the
 *         input pointer).
 */

MibLeaf* MibTableRow::add(MibLeaf* l)
{
	row.add(l);
	l->set_reference_to_row(this);
	return l;
}

/**
 * Append a snmpRowStatus instance to the end of the receiver row.
 * Each row can only contain zero or one snmpRowStatus object, but
 * that's not checked here.
 *
 * @param l - A pointer to the snmpRowStatus object to append.
 * @return The pointer to the added snmpRowStatus object
 *         (always the same as the input pointer).
 */

snmpRowStatus* MibTableRow::add(snmpRowStatus* l)
{
	row_status = l;
	row.add(l);
	l->set_reference_to_row(this);
	return l;
}

/**
 * Remove the object at a specified column from the receiver's row.
 *
 * @param i - The index (starting from 0) of the column to be removed.
 * @return TRUE if a such a column existed and has been removed,
 *         FALSE otherwise.
 */

bool MibTableRow::remove(int i)
{
	MibLeaf* ptr = row.getNth(i);
	if (!ptr) return FALSE;
	delete row.remove(ptr);
	return TRUE;
}

void MibTableRow::replace_element(unsigned int i, MibLeaf* l)
{
	row.overwriteNth(i, l);
}

/**
 * Operator <
 *
 * @param other - The MibTableRow the receiver is compared with.
 * @return TRUE if the receiver is less than the comparate, FALSE otherwise.
 */

int MibTableRow::operator<(const MibTableRow& other)
{
	return (index < other.index);
}

/**
 * Operator >
 *
 * @param other - The MibTableRow the receiver is compared with.
 * @return TRUE if the receiver is greater than the comparate, FALSE otherwise.
 */

int MibTableRow::operator>(const MibTableRow& other)
{
	return (index > other.index);
}

/**
 * Operator ==
 *
 * @param other - The MibTableRow the receiver is compared with.
 * @return TRUE if the receiver is equals the comparate, FALSE otherwise.
 */

int MibTableRow::operator==(const MibTableRow& other)
{
	return (index == other.index);
}

/**
 * Operator =
 *
 * @param other - The MibTableRow the receiver is assigned to.
 * @return A reference to the receiver itself.
 */

MibTableRow& MibTableRow::operator=(const MibTableRow& other)
{
	if (this == &other) return *this;

	index = other.index;
#ifndef USE_ARRAY_TEMPLATE
	row.clearAll();
	OrderedListCursor<MibLeaf> cur;
	for(cur.init(&other.row); cur.get(); cur.next())
		row.add((MibLeaf*)cur.get()->clone());
#else
	row = other.row;
#endif
	set_reference_to_row();
	return (*this);
}

void MibTableRow::set_index(const Oidx& ind)
{
	index = ind;
}


/**
 * Return the index (counted from 0) of the rows object whose index
 * equals the given oid.
 *
 * @param oid - The object id to search for.
 * @return The index (counted from 0) of the found column or -1 if
 *         such a column cannot be found.
 */
int MibTableRow::index_of(const Oidx& oid) const
{
	if ((row.first()->get_oid() <= oid) &&
	    (row.last()->get_oid() >= oid)) {
#ifndef USE_ARRAY_TEMPLATE
		OrderedListCursor<MibLeaf> cur;
		int i=0;
		for (cur.init(&row); cur.get(); cur.next(), i++)
			if (cur.get()->get_oid() == oid) return i;
#else
		for (int i=0; i<row.size(); i++) {
			if (row[i].get_oid() == oid) return i;
		}
#endif
	}
	return -1;
}


int MibTableRow::index_of_upper(const Oidx& oid) const
{
	if ((row.first()->get_oid() <= oid) &&
	    (row.last()->get_oid() >= oid)) {
#ifndef USE_ARRAY_TEMPLATE
		OrderedListCursor<MibLeaf> cur;
		int i=0;
		for (cur.init(&row); cur.get(); cur.next(), i++)
			if (cur.get()->get_oid() >= oid) return i;
#else
		for (int i=0; i<row.size(); i++) {
			if (row[i].get_oid() >= oid) return i;
		}
#endif
	}
	return -1;
}

int MibTableRow::index_of_lower(const Oidx& oid) const
{
	if ((row.first()->get_oid() <= oid) &&
	    (row.last()->get_oid() >= oid)) {
#ifndef USE_ARRAY_TEMPLATE
		OrderedListCursor<MibLeaf> cur;
		int i = row.size()-1;
		for (cur.initLast(&row); cur.get(); cur.prev(), i--)
			if (cur.get()->get_oid() <= oid) return i;
#else
		for (int i=row.size()-1; i>=0; i--) {
			if (row[i].get_oid() <= oid) return i;
		}
#endif
	}
	return -1;
}

MibLeaf* MibTableRow::get_element(const Oidx& oid)
{
	if ((row.first()->get_oid() <= oid) &&
	    (row.last()->get_oid() >= oid)) {
#ifndef USE_ARRAY_TEMPLATE
		OrderedListCursor<MibLeaf> cur;
		for (cur.init(&row); cur.get(); cur.next())
			if (cur.get()->get_oid() == oid) return cur.get();
#else
		for (int i=0; i<row.size(); i++) {
			if (row[i].get_oid() == oid) return row.getNth(i);
		}
#endif
	}
	return 0;
}

/**
 * Return whether the receiver row contains an object with a given oid.
 *
 * @param oid - The object identfier to search for.
 * @return TRUE if the receiver row contains an object with the
 *         given oid, FALSE otherwise.
 */
bool MibTableRow::contains(const Oidx& oid) const
{
	return (index_of(oid) >= 0);
}

void MibTableRow::get_vblist(Vbx* vbs, int sz, bool returnVolatileAsNull)
{
#ifndef USE_ARRAY_TEMPLATE
	OrderedListCursor<MibLeaf> cur;
	int i=0;
	for (cur.init(&row); ((cur.get()) && (i<sz)); cur.next(), i++) {
	       	vbs[i] = cur.get()->get_value();
		if ((returnVolatileAsNull) && (cur.get()->is_volatile())) {
#else
	for (int i=0; ((i<row.size()) && (i<sz)); i++) {
		vbs[i] = row[i].get_value();
		if ((returnVolatileAsNull) && (row[i].is_volatile())) {
#endif
			vbs[i].set_null();
		}
	}
}

/**
 * Set a reference to the receivers table in all of its columns.
 *
 * @param t - The MibTable object the reciever belongs to.
 */
void MibTableRow::set_reference_to_table(MibTable* t)
{
#ifndef USE_ARRAY_TEMPLATE
	OrderedListCursor<MibLeaf> cur;
	for (cur.init(&row); cur.get(); cur.next())
		cur.get()->set_reference_to_table(t);
#else
	for (int i=0; i<row.size(); i++) {
		row[i].set_reference_to_table(t);
	}
#endif
}

void MibTableRow::set_reference_to_row()
{
#ifndef USE_ARRAY_TEMPLATE
	OrderedListCursor<MibLeaf> cur;
	for (cur.init(&row); cur.get(); cur.next())
		cur.get()->set_reference_to_row(this);
#else
	for (int i=0; i<row.size(); i++) {
		row[i].set_reference_to_row(this);
	}
#endif
}

void MibTableRow::set_access(mib_access a)
{
#ifndef USE_ARRAY_TEMPLATE
	OrderedListCursor<MibLeaf> cur;
	for (cur.init(&row); cur.get(); cur.next())
		cur.get()->set_access(a);
#else
	for (int i=0; i<row.size(); i++) {
		row[i].set_access(a);
	}
#endif
}

void MibTableRow::read_only()
{
#ifndef USE_ARRAY_TEMPLATE
	OrderedListCursor<MibLeaf> cur;
	for (cur.init(&row); cur.get(); cur.next())
		cur.get()->set_access(READONLY);
#else
	for (int i=0; i<row.size(); i++) {
		row[i].set_access(READONLY);
	}
#endif
}



MibTable::MibTable(const MibTable& other): MibEntry(other.oid, other.access)
{
	generator = other.generator;

	// content should be copied too...
	content.clearAll();
	MibTableRow* r;
	OidListCursor<MibTableRow> cur;
	for (cur.init(&content); cur.get(); cur.next()) {
		r = content.add(cur.get()->clone());
		r->set_reference_to_table(this);
		r->set_reference_to_row();
	}

	index_len		   = other.index_len;
	index_struc		   = new index_info[other.index_len];
	memcpy(index_struc, other.index_struc,
	       sizeof(index_info)*other.index_len);
	row_status		   = other.row_status;
	row_timeout		   = other.row_timeout;
}

/**
 * Constructor with object identifier and variable index length.
 *
 * @param o - The object identifier of the table, which has to be
 *            the oid of the the SMI table entry object (table.1).
 */
MibTable::MibTable(const Oidx& o): MibEntry(o, NOACCESS)
{
	index_info* istruc = new index_info[1];
	istruc[0].min = 0;
	istruc[0].max = 127;
	istruc[0].implied = TRUE;
	init(o, istruc, 1);
	delete[] istruc;
}

/**
 * Constructor with object identifier and given fixed index length.
 *
 * @param o - The object identifier of the table, which has to be
 *            the oid of the the SMI table entry object (table.1).
 * @param ilen - The length of the index meassured in subidentifiers.
 */
MibTable::MibTable(const Oidx& o, int ilen): MibEntry(o, NOACCESS)
{
	index_info* istruc = new index_info[1];
	istruc[0].type = sNMP_SYNTAX_OID;
	if (ilen > 0) {
		istruc[0].min = istruc[0].max = ilen;
		istruc[0].implied = FALSE;
	}
	else if (ilen == 0) {
		istruc[0].min = 0;
		istruc[0].max = 127;
		istruc[0].implied = FALSE;
	}
	else {
		istruc[0].min = 0;
		istruc[0].max = 127;
		istruc[0].implied = TRUE;
	}
	init(o, istruc, 1);
	delete[] istruc;
}

/**
 * Constructor with object identifier and given fixed index length.
 * Additionally the automatic index object initialization can be
 * activated.
 *
 * @param o - The object identifier of the table, which has to be
 *            the oid of the the SMI table entry object (table.1).
 * @param ilen - The length of the index measured in subidentifiers.
 * @param a - If TRUE the automatic index object initialization is
 *            activated.
 */
MibTable::MibTable(const Oidx& o, int ilen, bool a):
  MibEntry(o, NOACCESS)
{
	index_info* istruc = new index_info[1];
	istruc[0].type = sNMP_SYNTAX_OID;
	if (ilen > 0) {
		istruc[0].min = istruc[0].max = ilen;
		istruc[0].implied = FALSE;
	}
	else if (ilen == 0) {
		istruc[0].min = 0;
		istruc[0].max = 127;
		istruc[0].implied = FALSE;
	}
	else {
		istruc[0].min = 0;
		istruc[0].max = 127;
		istruc[0].implied = TRUE;
	}
	init(o, istruc, 1);
	delete[] istruc;
}

MibTable::MibTable(const Oidx& o, const index_info* istruc,
		   unsigned int ilen):
  MibEntry(o, NOACCESS)
{
	init(o, istruc, ilen);
}


MibTable::~MibTable()
{
	if (index_struc) delete[] index_struc;
	// listeners are just pointers, so do not delete them here
	listeners.clear();
	voters.clear();
}

void MibTable::init(const Oidx& o, const index_info* istruc,
		    unsigned int ilen)
{
	generator.set_base(o);
	row_status = 0;
	row_timeout.set_life(DEFAULT_ROW_CREATION_TIMEOUT);
	index_len = ilen;
	index_struc = new index_info[ilen];
	memcpy(index_struc, istruc, sizeof(index_info)*ilen);
	upper = oid;
    if (upper.len() > 0) {
        upper[upper.len()-1] += 1;
    }
}


/*
  * Return the oid of the last accessible MibLeaf object within the
  * receiver table.
  *
  * @return A pointer to the maximum object identifier.
  */
Oidx* MibTable::max_key()
{
	return &upper;
}

int MibTable::size()
{
	return content.size();
}

void MibTable::clear()
{
	content.clearAll();
}

MibTableRow* MibTable::get_columns()
{
	return &generator;
}

bool MibTable::serialize(char*& buf, int& sz)
{
	OctetStr stream; // reserve two bytes for the sequence header
	OidListCursor<MibTableRow> cur;
	for (cur.init(&content); cur.get(); cur.next()) {
		// check if row should be made persistent
		if (!is_persistent(cur.get())) continue;

		int vbsz = cur.get()->size();
		Vbx* vbs = new Vbx[vbsz];
		cur.get()->get_vblist(vbs, vbsz);

		// set volatile values to Null

		unsigned char* b = 0;
		int buflen = 0;
		int status = Vbx::to_asn1(vbs, vbsz, b, buflen);
		delete[] vbs;
		if (b) {
			OctetStr add(b, buflen);
			delete[] b;
			stream += add;
		}
		if (status != SNMP_CLASS_SUCCESS) return FALSE;
	}
	int size = stream.len();
	buf = new char[size+10];
	int len = size+10;
	unsigned char* cp =
	  asn_build_header((unsigned char*)buf,
			   &len,
			   (unsigned char)(ASN_SEQUENCE | ASN_CONSTRUCTOR),
			   size);
	memcpy(cp, stream.data(), size);
	sz = ((size+10)-len)+stream.len();
	return TRUE;
}


bool MibTable::deserialize(char* buf, int& sz)
{
	unsigned char type = 0;
	int size = sz;
	buf = (char*) asn_parse_header((unsigned char*)buf, &size, &type);
	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 4);
	LOG("MibTable: deserialize: reading table (table)(size)");
	LOG(key()->get_printable());
	LOG(size);
	LOG_END;
	if (!buf) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 4);
		LOG("MibTable: deserialize: failed reading table header (table)");
		LOG(key()->get_printable());
		LOG_END;
		sz = 0;
		return FALSE;
	}
	if (type != (unsigned char)(ASN_SEQUENCE | ASN_CONSTRUCTOR)) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("MibTable: deserialize: wrong header - no sequence (table)");
		LOG(key()->get_printable());
		LOG_END;
		sz = 0;
		return FALSE;
	}
	while(size > 0) {
		unsigned char *data = (unsigned char *)buf;
		Vbx* vbs = 0;
		int vbsz = 0;
		int status = Vbx::from_asn1(vbs, vbsz, data, size);
                buf = (char *)data;
		if ((status != SNMP_CLASS_SUCCESS) ||
		    (vbsz == 0) ||
		    (vbsz != generator.size())) {

			LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			LOG("MibTable: deserialize: decoding error (table)(col count)(size)(status)");
			LOG(key()->get_printable());
			LOG(vbsz);
			LOG(generator.size());
			LOG(status);
			LOG_END;
			sz = 0;
			if (vbs) delete[] vbs;
			return FALSE;
		}

		Oidx ind(index(vbs[0].get_oid()));

		LOG_BEGIN(loggerModuleName, EVENT_LOG | 4);
		LOG("MibTable: deserialize: loading row (table)(index)(bytes remaining)");
		LOG(key()->get_printable());
		LOG(ind.get_printable());
		LOG(size);
		LOG_END;

		// preserve existing rows
		if (!find_index(ind)) {
			init_row(ind, vbs);
		}
		else {
			LOG_BEGIN(loggerModuleName, INFO_LOG | 3);
			LOG("MibTable: deserialize: row exists -> not loaded (index)");
			LOG(ind.get_printable());
			LOG_END;
		}
		delete[] vbs;
	}
	sz -= sz-size;
	return TRUE;
}


int MibTable::set_value(Request* req, int reqind)
{
	int status = SNMP_ERROR_SUCCESS;
	Oidx tmpoid(req->get_oid(reqind));
	MibLeaf* o;

	if ((o = find(tmpoid)) != 0) {

		if ((row_status) && (get_generator(tmpoid) == row_status)) {

			Vbx vb(req->get_value(reqind));

			int new_value = 1;
			int rs;

			if (vb.get_value(rs) != SNMP_CLASS_SUCCESS)
			    return SNMP_ERROR_WRONG_TYPE;
			switch (rs) {
			case rowCreateAndGo:
			  new_value = rowActive;
			  break;
			case rowCreateAndWait:
			  new_value = rowNotReady;
			  break;
			default:
			  new_value = rs;
			}

			if (new_value == rowDestroy) {

				Oidx ind(index(tmpoid));
				// call RowStatus set method to trigger
			        // userdefined actions
				status = set_row_status(find_index(ind),
							rowDestroy);
				// delete the whole row
				remove_row(ind);

				// notify other classes of this change
				notify_change(ind, REMOVE);
				return status;
			}
		}
		if ((status = o->commit_set_request(req, reqind)) !=
		    SNMP_ERROR_SUCCESS)
			return status;

		// check whether row can now be set active
		if (row_status) {
			Oidx ind(index(tmpoid));
			MibTableRow* row = find_index(ind);
			// check for status upgrade only if it is nessesary
			if (get_row_status(row) == rowNotReady) {

			  Vbx* pvbs = new Vbx[row->size()];
			  // row must exists, because we found the object
			  row->get_vblist(pvbs, row->size());
			  if (ready(pvbs, row->size(), row))
			    status = set_row_status(row,rowNotInService);
			  delete[] pvbs;
			}
		}

		// notify other classes
		notify_change(index(tmpoid), CHANGE);
	}
	else {
		Oidx ind(index(tmpoid));
		MibTableRow* r = add_row(ind);
		int rowStatusReq = -1;
		// for new rows collect all sets for the row and commit
		// them before the row_status is set
		bool ok = TRUE;
		for (int i=0; i<req->subrequests(); i++) {

			Oidx id(req->get_oid(i));
			if ((base(id) == oid) && (index(id) == ind)) {
				MibLeaf* gen = get_generator(id);
				if (row_status == gen) {
					rowStatusReq = i;
					continue;
				}

				status = r->get_nth(r->index_of(id))->
				  commit_set_request(req, i);
				if (status != SNMP_ERROR_SUCCESS) {
					ok = FALSE;
					break;
				}
			}
		}
		if ((ok) && (rowStatusReq >=0)) {
			status = r->get_nth(r->
			      index_of(req->get_oid(rowStatusReq)))->
				   commit_set_request(req, rowStatusReq);
		}
		else if (!ok) {
			// silently remove row
			content.remove(&ind);
			return status;
		}
		// default values are assigned by cloning from the
		// generator row
		// the index ist set by MibTable automatically
		notify_change(ind, CREATE);
		// check whether row can now be set active
		if (row_status) {
			Oidx ind(index(tmpoid));
			MibTableRow* row = find_index(ind);
			// check for status upgrade only if it is nessesary
			if (get_row_status(row) == rowNotReady) {

			  Vbx* pvbs = new Vbx[row->size()];
			  // row must exists, because we found the object
			  row->get_vblist(pvbs, row->size());
			  if (ready(pvbs, row->size(), row))
			    status = set_row_status(row,rowNotInService);
			  delete[] pvbs;
			}
		}
	}
	return status;
}



// if this table has a row status, this method returns the row status
// of the given row r
int MibTable::get_row_status(MibTableRow* row)
{
	if ((!row_status) || (!row)) return rowDestroy;
	else {
		int l = rowDestroy;
		row->get_row_status()->get_value(l);
		return l;
	}
}

/**
 * Set the value of the snmpRowStatus object of a given row.
 *
 * @param row - A row of the receiver.
 * @param status - The new value for the snmpRowStatus object.
 * @return If the row has no snmpRowStatus object return
 *         SNMP_ERROR_RESOURCE_UNAVAIL, on success SNMP_ERROR_SUCCESS.
 */
int MibTable::set_row_status(MibTableRow* row, int status)
{
	if ((!row_status) || (!row)) return SNMP_ERROR_RESOURCE_UNAVAIL;
	else {
		// this conversion is done because we want to call
	        // the (probably) userdefined RowStatus set method
		Vbx vb;
		vb.set_oid(row->get_row_status()->get_oid());
		vb.set_value(status);
		return row->get_row_status()->set(vb);
	}
}


/**
 * Returns the index part of a given oid (relative to the receiver).
 *
 * @param entry_oid - The oid of an instance in the receiver table.
 * @return The index part of the given oid.
 */
Oidx MibTable::index(const Oidx& entry_oid) const
{
	Oidx retval;
	retval = entry_oid.cut_left(oid.len() + 1);

	return retval;
}

/**
 * Returns the base of an oid.
 * (the entry oid without index and item index)
 * @param oid - The oid that should be scoped to this tables base
 * @return The base of the given oid.
 */
Oidx MibTable::base(const Oidx& entry_oid)
{
	Oidx retval;
	retval = entry_oid.cut_right(entry_oid.len() - oid.len());

	return retval;
}

int MibTable::perform_voting(MibTableRow* row, int curState, int reqState)
{
	ListCursor<MibTableVoter> cur;
	Oidx ind(row->get_index());
	int vote = is_transition_ok(0, row, ind, curState, reqState);
	for (cur.init(&voters); ((cur.get()) && (vote == SNMP_ERROR_SUCCESS));
	     cur.next()) {
	  vote = cur.get()->is_transition_ok(this, row, ind,
					     curState, reqState);
	}
	return vote;
}

void MibTable::fire_row_changed(int event, MibTableRow* row, const Oidx& ind)
{
	switch (event) {
	case rowCreateAndWait: {
		row_init(row, ind);
		ListCursor<MibTable> cur;
		for (cur.init(&listeners); cur.get(); cur.next()) {
			cur.get()->row_init(row, ind, this);
		}
		break;
	}
	case rowCreateAndGo: {
		row_added(row, ind);
		ListCursor<MibTable> cur;
		for (cur.init(&listeners); cur.get(); cur.next()) {
			cur.get()->row_added(row, ind, this);
		}
		break;
	}
	case rowNotInService: {
		row_deactivated(row, ind);
		ListCursor<MibTable> cur;
		for (cur.init(&listeners); cur.get(); cur.next()) {
			cur.get()->row_deactivated(row, ind, this);
		}
		break;
	}
	case rowActive: {
		row_activated(row, ind);
		ListCursor<MibTable> cur;
		for (cur.init(&listeners); cur.get(); cur.next()) {
			cur.get()->row_activated(row, ind, this);
		}
		break;
	}
	case rowDestroy: {
		row_delete(row, ind);
		ListCursor<MibTable> cur;
		for (cur.init(&listeners); cur.get(); cur.next()) {
			cur.get()->row_delete(row, ind, this);
		}
		break;
	}
	}
}


/**
 * Add a row with the given index to the table.
 * by cloning the generator row and setting the oids accordingly.
 * Then call row_added.
 *
 * @note If a row with this index already exists, a memory
 *       leak occurs.
 *
 * @param ind The index of the new row.
 * @return A pointer to the added row.
 */
MibTableRow* MibTable::add_row(const Oidx& ind)
{
	MibTableRow* new_row = new MibTableRow(generator);
	new_row->set_index(ind);
	fire_row_changed(rowCreateAndGo, new_row, ind);
	return content.add(new_row);
}

MibTableRow* MibTable::init_row(const Oidx& ind, Vbx* vbs)
{
	MibTableRow* row = new MibTableRow(generator);
	row->set_index(ind);
	int i=0;
#ifdef USE_ARRAY_TEMPLATE
	ArrayCursor<MibLeaf> cur;
#else
	ListCursor<MibLeaf> cur;
#endif
	for (cur.init(&row->row); cur.get(); cur.next(), i++) {
	    if (!cur.get()->is_volatile()) {
		cur.get()->set_value(vbs[i]);
	    }
	}
	fire_row_changed(rowCreateAndWait, row, ind);
	return content.add(row);
}


void MibTable::remove_row(const Oidx& ind)
{
	Oidx o(ind);
	MibTableRow* r = content.find(&o);
	if (!r) return;
	fire_row_changed(rowDestroy, r, ind);

	if (r) notready_rows.remove(r);
	content.remove(&o);
}


MibTableRow* MibTable::find_index(const Oidx& ind) const
{
	Oidx o(ind);
	return content.find(&o);
}

MibLeaf* MibTable::find(const Oidx& o) const
{
	Oidx ind = index(o);
	MibTableRow* row = content.find(&ind);
	if (row) {
		MibLeaf* leaf;
		if ((leaf = row->get_element(o)) != 0)
			return leaf;
	}
	return 0;
}


/**
  * Find the lexicographical successor MibLeaf object to a given oid.
  *
  * @param o - An oid.
  * @return A pointer to the successor object, 0 otherwise.
  */
MibLeaf* MibTable::find_next(const Oidx& o)
{
	if (content.empty()) return 0;

	// shortcut
	if (content.first()->first()->get_oid() > o) {
		return content.first()->first();
	}
	int col = 0;
#ifdef USE_ARRAY_TEMPLATE
	ArrayCursor<MibLeaf> cur;
#else
	OrderedListCursor<MibLeaf> cur;
#endif
	for (cur.init(&content.last()->row); cur.get(); cur.next(), col++) {

		if (cur.get()->get_oid() > o)
			break;
	}
	if (!cur.get()) return 0;
	Oidx ind = index(o);
	OidListCursor<MibTableRow> row;
	row.init(&content);
	// try to position the row
	if ((ind.len() > 0) && (o > row.get()->get_nth(col)->get_oid()))
		row.lookup(&ind);
	for (; row.get(); row.next()) {
		// we assume here that tables are arrays, that is every row has
		// the same size
		if (row.get()->get_nth(col)->get_oid() > o)
			return row.get()->get_nth(col);
	}
	return 0;
}


Oidx MibTable::find_succ(const Oidx& o, Request*)
{
        ThreadSynchronize s(*this); {
		MibLeaf* l = find_next(o);
		while ((l) &&
		       ((l->get_access() == NOACCESS) ||
			(!l->valid()))) {
		    l = find_next(l->get_oid());
		}
		if (l) {
		    return l->get_oid();
		}
		return Oidx();
	}
}


/**
 * Find the lexicographical predessor MibLeaf object to a given oid.
 *
 * @param o - An oid.
 * @return A pointer to the predessor object, 0 otherwise.
 */
MibLeaf* MibTable::find_prev(const Oidx& o)
{
	if (content.empty()) return 0;

	// shortcut
	if (content.last()->last()->get_oid() < o) {
		return content.last()->last();
	}
	int col = content.first()->size();
#ifdef USE_ARRAY_TEMPLATE
	ArrayCursor<MibLeaf> cur;
#else
	OrderedListCursor<MibLeaf> cur;
#endif
	for (cur.initLast(&content.first()->row); cur.get();
	     cur.prev(), col--) {

		if (cur.get()->get_oid() < o)
			break;
	}
	if (!cur.get()) return 0;
	OidListCursor<MibTableRow> row;
	for (row.init(&content); row.get(); row.next()) {
		// we assume here that tables are arrays, that is every row has
		// the same size
		if (row.get()->get_nth(col)->get_oid() < o)
			return row.get()->get_nth(col);
	}
	return 0;
}


/**
 * Return the MibLeaf object of the receiver at a given position.
 *
 * @param n - The row (counted from 0).
 * @param m - The column (counted from 0).
 * @return A pointer to the MibLeaf object at position (n,m), 0
 *         if n or m are out of range.
 */
MibLeaf* MibTable::get(int n, int m)
{
	MibTableRow* ptr = content.getNth(m);
	if (ptr)
		return ptr->get_nth(n);
	return 0;
}


/**
 * Return a pointer to the generator object of a given column.
 * (The generator object of a column is the not accessible object
 * a the top of each row - the master copy)
 *
 * @param o - The oid of an MibLeaf instance of the receiver.
 * @return A pointer to the generator object of the specified column.
 */
MibLeaf* MibTable::get_generator(const Oidx& o)
{
	// does oid belong to this table?
	if ((o.len() <= oid.len()) ||
	    (o.cut_right(o.len() - oid.len()) != oid)) return 0; // no

	Oidx genOid(oid);
	genOid += o[oid.len()];

	return generator.get_element(genOid);
}

void MibTable::add_col(MibLeaf* l)
{
	generator.add(l);
	l->set_reference_to_table(this);
}

void MibTable::replace_col(unsigned int i, MibLeaf* l)
{
	generator.replace_element(i, l);
	l->set_reference_to_table(this);
}

void MibTable::add_col(snmpRowStatus* rs)
{
	generator.add(rs);
	rs->set_reference_to_table(this);
	row_status = rs;
}

bool MibTable::is_index_valid(const Oidx& ind) const
{
	Oidx o(ind);
	unsigned long l = 0;
	unsigned int i;
	for (i=0; ((i<index_len) && (l < o.len())); i++) {
		if (index_struc[i].implied) {
			if (i+1 != index_len)
				return FALSE;
			if (l < o.len()) {
			  if ((index_struc[i].type == sNMP_SYNTAX_OCTETS) ||
			      (index_struc[i].type == sNMP_SYNTAX_IPADDR)) {
			      if (!check_index(o, l, o.len())) return FALSE;
			  }
			  return TRUE;
			}
			return FALSE;
		}
		else if ((!index_struc[i].implied) &&
			 (index_struc[i].min != index_struc[i].max)) {
			if (o.len() < o[l]+1) return FALSE;
			if ((o[l] < index_struc[i].min) ||
			    (o[l] > index_struc[i].max)) return FALSE;
			if ((index_struc[i].type == sNMP_SYNTAX_OCTETS) ||
			    (index_struc[i].type == sNMP_SYNTAX_IPADDR)) {
			  if (!check_index(o, l, l+o[l]+1))
			    return FALSE;
			}
			l += o[l]+1;
		}
		else {
			if ((index_struc[i].type == sNMP_SYNTAX_OCTETS) ||
			    (index_struc[i].type == sNMP_SYNTAX_IPADDR)) {
			  if (!check_index(o, l, l+index_struc[i].max))
			    return FALSE;
			}
			// min == max
			l += index_struc[i].max;
		}
	}
	return ((o.len() == l) && (i >= index_len));
}

bool  MibTable::check_index(Oidx& o, unsigned long b, unsigned long e) const
{
	for (unsigned long j=b; ((j<o.len()) && (j<e)); j++)
		if (o[j] > 255) return FALSE;
	return TRUE;
}

/**
 * Check if an object of a given oid could be created.
 *
 * @param o - The oid to be checked.
 * @return TRUE if an object of the given oid could be created.
 */
bool MibTable::could_ever_be_managed(const Oidx& o, int& result)
{
	MibLeaf* gen = get_generator(o);
	if ((!gen) || (gen->get_access() < READCREATE) ||
	    (!is_index_valid(index(o)))) {
		if ((gen) && (gen->get_access() < READWRITE))
			result = SNMP_ERROR_NOT_WRITEABLE;
		else
			result = SNMP_ERROR_NO_CREATION;
		return FALSE;
	}
	return TRUE;
}

/**
 * Check whether the specified row is ready to set in service.
 * This method is left empty for override by the API user.
 *
 * @param pvbs - A pointer to an array of Vbx objects containing
 *               the values and oids of the columns of a row to check.
 * @param sz - The size of the array.
 * @return TRUE if the specified row is ready to set for service.
 */
bool	MibTable::ready_for_service(Vbx* pvbs, int sz)
{
	return TRUE;
}

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
bool MibTable::ready(Vbx* pvbs, int sz, MibTableRow* row)
{
	// sz must be size of a complete row!
	bool* required = new bool[sz];
	get_required_columns(required, 0);

	for (int i=0; i<sz; i++) {
		// check cols (except row_status)
		if (generator.get_nth(i) != row_status) {
		  // check if value_ok()
		  MibLeaf* c = row->get_nth(i);
		  if ((c->get_access() > READONLY) &&
		      (pvbs[i].valid()) &&        // check if value is set
		      (!c->value_ok(pvbs[i]))) {  // check if set value is ok
		    delete[] required;

		    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 3);
		    LOG("MibTable: ready_for_service: failed on (col)(value)");
		    LOG(c->get_oid().get_printable());
		    LOG(pvbs[i].get_printable_value());
		    LOG_END;

		    return FALSE;
		  }
		}
		// check if value was given for non-default col
		if (required[i]) {
			if (!pvbs[i].valid()) {
				delete[] required;
				return FALSE;
			}
			// check for changed value
			/* this check is redundant and error
			   prone:
			MibLeaf* l = generator.get_nth(i);
			if ((!l->valid()) &&
			    (l->get_value() == pvbs[i])) {
				delete[] required;
				return FALSE;
			}
			*/
		}
	}
	delete[] required;
	// let the API user have the last word
	return ready_for_service(pvbs, sz);
}


// request processing methods

// MibTable::get_request searches the desired value within the table
// and returns the value as a SNMP pdu via the notify queue.
// Per definition the returned pdu contains exactly one
// variable binding (vb). The errorindex points to the index of the
// corresponding vb in the original pdu.

void MibTable::get_request(Request* req, int ind)
{
	MibLeaf* o;
	if ((o = find(req->get_oid(ind))) != 0) {

		if (o->get_access() >= READONLY) {

				// call get_request-method of
				// the MibLeaf Object
				o->get_request(req, ind);
				// response has been sent by the above call,
				// so we could leave here
				return;
		}
		else {
			Vbx vb(req->get_oid(ind));
			vb.set_syntax(sNMP_SYNTAX_NOSUCHOBJECT);
			req->finish(ind, vb);
		}
	}
	else {
		Vbx vb(req->get_oid(ind));
		vb.set_syntax(sNMP_SYNTAX_NOSUCHOBJECT);
#ifdef USE_ARRAY_TEMPLATE
		ArrayCursor<MibLeaf> cur;
#else
		OrderedListCursor<MibLeaf> cur;
#endif
		for (cur.init(&generator.row); cur.get(); cur.next()) {
			Oidx id(*key());
			id += *cur.get()->key();
			if (id.is_root_of(req->get_oid(ind))) {
				vb.set_syntax(sNMP_SYNTAX_NOSUCHINSTANCE);
				break;
			}
		}
		req->finish(ind, vb);
	}
}

void MibTable::get_next_request(Request* req, int ind)
{
	MibLeaf* o;
	if ((o = find(req->get_oid(ind))) != 0) {
		o->get_request(req, ind);
	}
	else {
		Vbx vb(req->get_oid(ind));
		vb.set_syntax(sNMP_SYNTAX_ENDOFMIBVIEW);
		req->finish(ind, vb);
	}
}


int MibTable::commit_set_request(Request* req, int ind)
{
	if ((set_value(req, ind)) != SNMP_ERROR_SUCCESS) {
		// do not answer request, wait for undo
	    req->error(ind, SNMP_ERROR_COMITFAIL);
	    return SNMP_ERROR_COMITFAIL;
	}
	return SNMP_ERROR_SUCCESS;
}


void MibTable::get_required_columns(bool* required, Vbx* pvbs)
{
#ifndef USE_ARRAY_TEMPLATE
	OrderedListCursor<MibLeaf> cur;
	int i;
	for (cur.init(&generator.row), i=0; cur.get(); cur.next(), i++) {
		if ((cur.get()->get_access() == READCREATE) &&
		    (!cur.get()->has_default()))
			required[i] = TRUE;
		else {
			required[i] = FALSE;
			if (pvbs)
				pvbs[i] = cur.get()->get_value();
		}
	}
#else
	for (int i=0; i<generator.row.size(); i++) {
		if ((generator.row[i].get_access() == READCREATE) &&
		    (!generator.row[i].has_default()))
			required[i] = TRUE;
		else {
			required[i] = FALSE;
			if (pvbs)
				pvbs[i] = generator.row[i].get_value();
		}
	}
#endif
}


int MibTable::check_creation(Request* req, int& ind)
{
	bool ok = FALSE;
	bool wait = FALSE;
	bool ignore = FALSE;

	int rowsize = generator.size();

	bool* fulfilled = new bool[rowsize];
	bool* required  = new bool[rowsize];
	Vbx* pvbs = new Vbx[rowsize];

	memset(fulfilled, FALSE, sizeof(bool)*rowsize);
	memset(required,  FALSE, sizeof(bool)*rowsize);

	get_required_columns(required, pvbs);

	Oidx new_index = index(req->get_oid(ind));
	int i;
	int rsIndex = 0;

       	for (i=0; i<req->subrequests(); i++) {

	       	if (row_status && (get_generator(req->get_oid(i)) == row_status) &&
		        (new_index == index(req->get_oid(i)))) {

	       		if (req->get_value(i).get_syntax() !=
	       		    row_status->get_syntax()) {

				delete[] fulfilled;
				delete[] required;
				delete[] pvbs;
				return SNMP_ERROR_WRONG_TYPE;
			}

			rsIndex = i;
			int new_row_status = 0;
			req->get_value(i).get_value(new_row_status);

			switch (new_row_status) {
			case rowActive:
			case rowNotInService:
			  ind = i;

			  delete[] fulfilled;
			  delete[] required;
			  delete[] pvbs;
			  return SNMP_ERROR_INCONSIST_VAL;
			case rowCreateAndGo:
			  ok = TRUE;
			  break;
			case rowCreateAndWait:
			  ok   = TRUE;
			  wait = TRUE;
			  break;
			case rowDestroy:
			  ok = TRUE;
			  ignore = TRUE;
			  break;
			default:
			  delete[] fulfilled;
			  delete[] required;
			  delete[] pvbs;
			  return SNMP_ERROR_WRONG_VALUE;
			}
			break;
		}
	}
	// no rowStatus has been set but row does not exist ->
	// inconsistentName
	if (!ok) {
		delete[] fulfilled;
		delete[] required;
		delete[] pvbs;
		return SNMP_ERROR_INCONSIS_NAME;
	}
	// collect all set requests for the row to be created
	Pdux pdu;
	int col;

       	for (i=0; i<req->subrequests(); i++) {

		if ((base(req->get_oid(i)) == oid) &&
		    (index(req->get_oid(i)) == new_index)) {

			MibLeaf* gen = get_generator(req->get_oid(i));
			if ((!gen) ||
			    (req->get_value(i).get_syntax() !=
			     gen->get_syntax()) ||
			    (!gen->value_ok(req->get_value(i)))) {
			  // TODO: more precise error status
				ind = i;
				delete[] fulfilled;
				delete[] required;
				delete[] pvbs;
				return SNMP_ERROR_INCONSIST_VAL;
			}
			if (gen->get_access() < READWRITE) {

				ind = i;
				delete[] fulfilled;
				delete[] required;
				delete[] pvbs;
				return SNMP_ERROR_NOT_WRITEABLE;
			}
			req->set_ready(i);

			if (ignore) // ignore destroying of non existent row
				req->finish(i);
			col = generator.index_of(gen);
			fulfilled[col] = TRUE;
			pvbs[col] = req->get_value(i);
		}
	}
	// test values through prepare
	MibTableRow* new_row = new MibTableRow(generator);
	new_row->set_index(new_index);
       	for (i=0; i<req->subrequests(); i++) {
		MibLeaf* gen = get_generator(req->get_oid(i));
		if (!gen) continue; // does this really happen?
		col = generator.index_of(gen);
		if (fulfilled[col]) {
			MibLeaf* l = new_row->get_element(req->get_oid(i));
			if (!l) continue; // does this really happen?
			int result = l->prepare_set_request(req, i);
			if (result != SNMP_ERROR_SUCCESS) {
				// objects < i are already cleaned up
				l->cleanup_set_request(req, i);
				// force setting error index right later
				ind = i;
				delete[] fulfilled;
				delete[] required;
				delete[] pvbs;
				delete new_row;
				return result;
			}
			else {
				// new row must be cleaned up because
			        // it will be deleted later in any case
				l->cleanup_set_request(req, i);
			}
		}
	}
	int voting_result = SNMP_ERROR_SUCCESS;
	if ((!ignore) && (!wait)) {
		// if createAndGo: check if row is complete

		for (i=0; i<rowsize; i++)

			if ((required[i]) && (!fulfilled[i])) {
				delete[] fulfilled;
				delete[] required;
				delete[] pvbs;
				delete new_row;
				ind = rsIndex;
				return SNMP_ERROR_INCONSIST_VAL;
			}
		// check if row can be set active
		// set the row values
		for (i=0; i<rowsize; i++) {
			MibLeaf* col = new_row->get_nth(i);
			if (fulfilled[i]) {
				col->replace_value(pvbs[i].clone_value());
			}
		}
		if (!ready(pvbs, rowsize, new_row)) {
			delete[] fulfilled;
			delete[] required;
			delete[] pvbs;
			delete new_row;
			ind = rsIndex;
			return SNMP_ERROR_INCONSIST_VAL;
		}
		// let registered voters vote for the state transition
		voting_result = perform_voting(new_row, rowEmpty, rowActive);
	}
	else if (!ignore)
		voting_result = perform_voting(new_row,
					       rowEmpty, rowNotInService);
	else
		voting_result = perform_voting(new_row,
					       rowEmpty, rowDestroy);
	delete new_row;

	// Unset the ready flag of the current column.
	// via this one all the other columns are set by commit_set_request
	req->unset_ready(ind);
	delete[] fulfilled;
	delete[] required;
	delete[] pvbs;
	return voting_result;
}


int MibTable::prepare_set_request(Request* req, int& ind)
{
	int result = SNMP_ERROR_NO_CREATION;
	MibLeaf* o;
	if ((o = find(req->get_oid(ind))) != 0) {
	    if (delete_rows.index(o->my_row) < 0) {

		if (o->get_access() >= READWRITE) {
		  if (o->get_syntax() ==
		      req->get_value(ind).get_syntax()) {

		    if ((row_status) &&
			(get_generator(req->get_oid(ind)) ==
			 row_status)) {
		      Vbx vb(req->get_value(ind));
		      if (!(((snmpRowStatus*)o)->value_ok(vb)))
			return SNMP_ERROR_WRONG_VALUE;
		      if ((((snmpRowStatus*)o)->
			   transition_ok(vb)) &&
			  (((snmpRowStatus*)o)->
			   check_state_change(vb, req))) {
			result = o->prepare_set_request(req, ind);
			if (result == SNMP_ERROR_SUCCESS) {
			  int nrs = -1;
			  vb.get_value(nrs);
			  return perform_voting(o->my_row,
					       (int)((snmpRowStatus*)o)->get(),
						nrs);
			}
			else return result;
		      }
		      else
			return SNMP_ERROR_INCONSIST_VAL;
		    }
		    else return o->prepare_set_request(req, ind);
		  }
		  else return SNMP_ERROR_WRONG_TYPE;
		}
		else return SNMP_ERROR_NOT_WRITEABLE;
	    }
	    else return SNMP_ERROR_INCONSIST_VAL;
	}
	else if ((row_status) &&
		 (could_ever_be_managed(req->get_oid(ind), result))) {
		// will also perform a voting on the row status change
		return check_creation(req, ind);
	}
	else if (could_ever_be_managed(req->get_oid(ind), result)) {

		if (req->get_value(ind).get_syntax() ==
		    get_generator(req->get_oid(ind))->get_syntax())
			return SNMP_ERROR_SUCCESS;
		else	return SNMP_ERROR_WRONG_TYPE;

	}
	else
		return result;
}

int MibTable::undo_set_request(Request* req, int& ind)
{
	MibLeaf* o;
	delete_rows.clear();
	int result = SNMP_ERROR_SUCCESS;
	for (int i=0; i < req->subrequests(); i++) {
		if ((o = find(req->get_oid(i))) != 0) {
			int status = o->undo_set_request(req, i);
			if ((!result) && (status != SNMP_ERROR_SUCCESS))
			    result = status;
		}
	}
        // nothing to undo!
	return result;
}

void MibTable::cleanup_set_request(Request* req, int& ind)
{
	// this method is only called once per MibTable
	// involved in a set request, so we need to
	// cleanup all sub-request for this table here.
	MibLeaf* o;
	for (int i=0; i < req->subrequests(); i++) {
		if ((o = find(req->get_oid(i))) != 0) {
			o->cleanup_set_request(req, i);
		}
	}
	delete_rows.clear();
}

/**
 * Return the next available index value for the receiver table,
 * that can be used be a manager to create a new row.
 * @return The next available index value.
 * @note Works best if the tables index is a single scalar
 *       sub-identifier.
 */
Oidx MibTable::get_next_avail_index() const
{
	Oidx retval;
	if (content.empty()) {
		for (unsigned int i=0; i<index_len; i++) {
			if (index_struc[i].implied) retval += 1;
			else {
			  if (!index_struc[i].implied)
			    retval+=index_struc[i].min;
			  for (unsigned int j=1; j<index_struc[i].max; j++)
			    retval += 1;
			}
		}
		return retval;
	}
	retval = content.last()->get_index();
	retval[retval.len()-1] = retval[retval.len()-1]+1;

	return retval;
}

/**
 * Remove all rows that are not in the given list.
 *
 * @param confirmed_rows - A list of row indecies of rows which
 *                         should not be removed from the receiver.
 */
void MibTable::remove_obsolete_rows(OrderedList<Oidx>& confirmed_rows)
{
	OidListCursor<MibTableRow> cur;
	OrderedListCursor<Oidx> con;
	for (cur.init(&content); cur.get(); ) {
		for (con.init(&confirmed_rows);
		     ((con.get()) && (*con.get() < cur.get()->get_index()));
		     con.next()) { }

		// not confirmed -> delete row
		if ((!con.get()) || (*con.get() > cur.get()->get_index())) {

			LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
			LOG("MibTable: update: removing row");
			LOG(cur.get()->get_index().get_printable());
			LOG_END;

			MibTableRow* victim = cur.get();
			cur.next();
			delete content.remove(victim);
			continue;
		}
		cur.next();
	}
	// reset list
	confirmed_rows.clearAll();
}

/**
 * Periodically remove all rows that are notReady for more
 * than a given timeout (default is 5 minutes)
 *
 * Note: a row cannot be set from notInService or active to notReady
 */
void MibTable::remove_unused_rows()
{
	start_synch();
	if ((row_status) && (!(row_timeout.in_time()))) {

		OrderedListCursor<MibTableRow> cur;
		for (cur.init(&notready_rows); cur.get(); ) {
			snmpRowStatus* status = cur.get()->get_row_status();
			if (status->get() == rowNotReady) {
				LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
				LOG("MibTable: removing row due to timeout");
				LOG(cur.get()->index.get_printable());
				LOG_END;
				fire_row_changed(rowDestroy, cur.get(),
						 cur.get()->get_index());
				ListItem<MibTableRow>* victim =
				  cur.get_cursor();
				MibTableRow* del = cur.get();
				cur.next();
				content.remove(del);
				delete notready_rows.remove(victim);
				continue;
			}
			cur.next();
		}
		notready_rows.clear();
		OidListCursor<MibTableRow> c;
		for (c.init(&content); c.get(); c.next()) {
			if (c.get()->get_row_status()->get() ==
			    rowNotReady)
				notready_rows.add(c.get());
		}
		row_timeout.set_timestamp();
	}
	else {
		// remove rows for tables without RowStatus
		// (used for deferred row deletion)
		OrderedListCursor<MibTableRow> cur;
		for (cur.init(&notready_rows); cur.get(); ) {
			LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
			LOG("MibTable: removing row (index)");
			LOG(cur.get()->index.get_printable());
			LOG_END;
			fire_row_changed(rowDestroy, cur.get(),
					 cur.get()->get_index());
			ListItem<MibTableRow>* victim =
			    cur.get_cursor();
			MibTableRow* del = cur.get();
			cur.next();
			content.remove(del);
			delete notready_rows.remove(victim);
			continue;
		}
		notready_rows.clear();
	}
	end_synch();
}


/**
 * Return all (active) rows as a two dimensional array of Vbx
 * objects. If the receiver table does not have any snmpRowStatus
 * column all rows are returned.
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
 *    0 all rows are returned.
 */
void MibTable::get_contents(Vbx**& contents, int& rows, int& cols,
							int discriminator)
{
	start_synch();
	if (!contents) {
		rows = content.size();
		cols = generator.size();
		contents = new Vbx*[rows];
		for (int i=0; i<rows; i++) {
			contents[i] = new Vbx[cols];
		}
	}
	int n = 0;
	OidListCursor<MibTableRow> cur;
	for (cur.init(&content); cur.get(); cur.next()) {
		snmpRowStatus* status = cur.get()->get_row_status();
		if ((!status) || (discriminator == 0) ||
		    ((status) && (status->get() == discriminator))) {
			cur.get()->get_vblist(contents[n++], cols);
		}
	}
	end_synch();
}

/**
 * Return all (active) rows as a list of pointers to the
 * corresponding MibTableRow instances. If the receiver table
 * does not have any snmpRowStatus column all rows are returned.
 *
 * @note Don't use this method for tables where rows are deleted,
 *       because you only get references, that then may point to
 *       nowhere.
 *
 *
 * @param discriminator
 *    if the receiver table has a snmpRowStatus, the discriminator
 *    selects the rows to be returned. Default is rowActive, which
 *    means all active rows are returned. If the discriminator is
 *    rowEnmpty(0), all rows are returned.
 */
List<MibTableRow>* MibTable::get_rows(int discriminator)
{
	OidListCursor<MibTableRow> cur;
	List<MibTableRow>* list = new List<MibTableRow>;
	for (cur.init(&content); cur.get(); cur.next()) {
		snmpRowStatus* status = cur.get()->get_row_status();
		if ((!status) || (discriminator == 0) ||
		    ((status) && (status->get() == discriminator))) {
			list->add(cur.get());
		}
	}
	return list;
}

/**
 * Return all (active) rows as a list of pointers to the
 * corresponding MibTableRow instances. If the receiver table
 * does not have any snmpRowStatus column all rows are returned.
 *
 * @note This method is escpecially useful for tables where rows
 *       are deleted, because you get copies of the rows rather
 *       than references.
 *
 * @param discriminator
 *    if the receiver table has a snmpRowStatus, the discriminator
 *    selects the rows to be returned. Default is rowActive, which
 *    means all active rows are returned. If the discriminator is
 *    rowEnmpty(0), all rows are returned.
 * @return
 *    a pointer to a cloned list of the rows in the receiver.
 */
List<MibTableRow>* MibTable::get_rows_cloned(int discriminator)
{
	return get_rows_cloned(0, discriminator);
}

List<MibTableRow>* MibTable::get_rows_cloned(const Oidx* prefix, int discriminator)
{
	start_synch();
	OidListCursor<MibTableRow> cur;
	List<MibTableRow>* list = new List<MibTableRow>();
	for (cur.init(&content); cur.get(); cur.next()) {
		snmpRowStatus* status = cur.get()->get_row_status();
		if (((!status) || (discriminator == 0) ||
		     ((status) && (status->get() == discriminator))) &&
		    ((!prefix) ||
		     (cur.get()->get_index().in_subtree_of(*prefix)))) {
			list->add(new MibTableRow(*cur.get()));
		}
	}
	end_synch();
	return list;
}


void MibTable::add_listener(MibTable* other)
{
	listeners.add(other);
}

void MibTable::remove_listener(MibTable* other)
{
	listeners.remove(other);
}

void MibTable::add_voter(MibTableVoter* v)
{
	voters.add(v);
}

void MibTable::remove_voter(MibTableVoter* v)
{
	voters.remove(v);
}



/*----------------------- class MibConfigBER ------------------------*/

bool MibConfigBER::save(MibContext* context, const NS_SNMP OctetStr& path)
{
	LOG_BEGIN(loggerModuleName, INFO_LOG | 1);
	LOG("Saving MIB context contents BER encoded (context)(path)");
	LOG(context->get_name().get_printable());
	LOG(path.get_printable());
	LOG_END;
	context->save_to(path);
	return TRUE;
}

bool MibConfigBER::load(MibContext* context, const NS_SNMP OctetStr& path)
{
	OctetStr pathPrefix(path);
	LOG_BEGIN(loggerModuleName, INFO_LOG | 1);
	LOG("Loading MIB context contents BER encoded (context)(path)");
	LOG(context->get_name().get_printable());
	LOG(path.get_printable());
	LOG_END;
	context->load_from(path);
	return TRUE;
}

/*--------------------------- class Mib -----------------------------*/


Mib* Mib::instance = 0;
#ifdef STATIC_REQUEST_LIST
RequestList* Mib::requestList = 0;
#endif

/**
 * Default Constructor
 */
Mib::Mib()
{
    construct(DEFAULT_PATH_PERSISTENT_MO);
}

/**
 * Constructor with path where persistent MIB objects should be
 * stored to and loaded from.
 * @param path - The directory to store persistent objects.
 */
Mib::Mib(const OctetStr& path)
{
	construct(path);
}

/**
 * Destructor
 */
Mib::~Mib()
{
#ifdef AGENTPP_USE_THREAD_POOL
	if (threadPool)
	{
          threadPool->terminate();  
	  int loops = 0;
	  while (!threadPool->is_idle())
	  {
	    Thread::sleep(500);
	    loops++;
	    if (loops > 10)
	    {
	      LOG_BEGIN(loggerModuleName, INFO_LOG | 1);
	      LOG("Mib: Still waiting for active requests to finish");
	      LOG_END;
	      loops = 0;
	    }
	  }
	  delete threadPool;
	}
#endif
	lock_mib();
	contexts.clearAll();
	unlock_mib();
	if (notificationSender)
	    delete notificationSender;
	if (persistent_objects_path) {
	    delete persistent_objects_path;
	    persistent_objects_path = 0;
	}
	// entries.clearAll() is called within ~List().
	instance = 0;
}

void Mib::construct(const OctetStr& path)
{
	instance = this;
	persistent_objects_path = new OctetStr(path);

	notificationSender = 0;
	// add default context
	defaultContext = new MibContext();
	contexts.add(defaultContext);
#ifndef STATIC_REQUEST_LIST
	requestList = 0;
#endif
#ifdef _SNMPv3
	bootCounterFile = DEFAULT_ENGINE_BOOTS_FILE;
#ifdef _PROXY_FORWARDER
	if ((requestList) && (requestList->get_v3mp()))
	    requestList->get_v3mp()->get_local_engine_id(myEngineID);
#endif
#endif
#ifdef AGENTPP_USE_THREAD_POOL
	threadPool = 0;
#endif
	add_config_format(1, new MibConfigBER());
}

#ifdef _SNMPv3
int Mib::get_boot_counter(const OctetStr& engineID, unsigned int& engineBoots)
{
	return getBootCounter(bootCounterFile.get_printable(),
			      engineID, engineBoots);
}

int Mib::set_boot_counter(const OctetStr& engineID, unsigned int engineBoots)
{
	return saveBootCounter(bootCounterFile.get_printable(),
			       engineID, engineBoots);
}
#endif

#ifdef AGENTPP_USE_THREAD_POOL
void Mib::set_thread_pool(ThreadPool* tp)
{
	if (threadPool)
		delete threadPool;
	threadPool = tp;
}
#endif

void Mib::set_request_list(RequestList* reqList)
{
	requestList = reqList;
}

void Mib::set_notification_sender(NotificationSender* s)
{
	if (notificationSender) {
		delete notificationSender;
	}
	notificationSender = s;
}

int Mib::notify(const OctetStr& context,
		const Oidx& oid,
		Vbx* vbs, int sz,
		unsigned int timestamp)
{
	if (!notificationSender) {
		notificationSender = new NotificationOriginator();
	}
	return notificationSender->notify(context, oid, vbs, sz, timestamp);
}

MibEntry* Mib::add(MibEntry* item)
{
	lock_mib();
	MibEntry* e = defaultContext->add(item);
	unlock_mib();
	return e;
}

MibEntry* Mib::add(const OctetStr& context, MibEntry* item)
{
	Oidx contextKey(Oidx::from_string(context));
	lock_mib();
	MibContext* c = contexts.find(&contextKey);
	if (!c) {
		c = contexts.add(new MibContext(context));
	}
	MibEntry* e = c->add(item);
	unlock_mib();
	return e;
}

MibContext* Mib::add_context(const OctetStr& context)
{
	Oidx contextKey(Oidx::from_string(context));
	lock_mib();
	MibContext* c = contexts.find(&contextKey);
	if (!c) {
		c = contexts.add(new MibContext(context));
	}
	unlock_mib();
	return c;
}

void Mib::remove_context(const OctetStr& context)
{
	Oidx contextKey(Oidx::from_string(context));
	lock_mib();
	contexts.remove(&contextKey);
	unlock_mib();
}

bool Mib::remove(const Oidx& oid)
{
	bool removed = TRUE;
	lock_mib();
	// first look for a group
	if (!defaultContext->remove_group(oid)) {
		MibEntryPtr entry = NULL;
		if ((defaultContext->find(oid, entry) == SNMP_ERROR_SUCCESS) && (entry)) {
		  entry->start_synch();
		  /* ok, this works because:
		     - we hold the lock for the MIB (lock_mib())
		     - before a request can lock a MibEntry object it
		     must get the MIB lock
		     - so if we get the MibEntry lock here no other request
		     can be currently using this object
		     - even if we release the lock, because we hold the
		     MIB lock
		  */
		  MibEntry* victim = defaultContext->remove(oid);
		  entry->end_synch(); // necessary? wanted?
		  if (victim) delete victim;
		  else removed = FALSE;
		}
	}
	else removed = TRUE;
	unlock_mib();
	return removed;
}

bool Mib::remove(const OctetStr& context, const Oidx& oid)
{
	bool removed = TRUE;
	Oidx contextKey(Oidx::from_string(context));
	lock_mib();
	MibContext* c = contexts.find(&contextKey);
	if (!c)
	{
	  unlock_mib();
	  return FALSE;
	}
	if (!c->remove_group(oid)) {
		MibEntryPtr entry = NULL;
		if ((c->find(oid, entry) == SNMP_ERROR_SUCCESS) && (entry)) {
		  entry->start_synch();
		  /* ok, this works because:
		     - we hold the lock for the MIB (lock_mib())
		     - before a request can lock a MibEntry object it
                     must get the MIB lock
		     - so if we get the MibEntry lock here no other request
                     can be currently using this object
		     - even if we release the lock, because we hold the
                     MIB lock
		  */
		  MibEntry* victim = c->remove(oid);
		  entry->end_synch(); // necessary? wanted?
		  if (victim) delete victim;
		  else removed = FALSE;
		}
	}
	else removed = TRUE;
	unlock_mib();
	return removed;
}

OctetStr Mib::get_persistent_objects_path() const
{
	if (persistent_objects_path)
		return *persistent_objects_path;
	return DEFAULT_PATH_PERSISTENT_MO;
}

void Mib::set_persistent_objects_path(const OctetStr* str)
{
    if (persistent_objects_path)
    {
	delete persistent_objects_path;
	persistent_objects_path = 0;
    }

    if (str)
	persistent_objects_path = (OctetStr*)str->clone();
}

MibGroup* Mib::find_group_of(const OctetStr& context, const Oidx& oid)
{
	MibContext* c = get_context(context);
	return c->find_group_of(oid);
}

bool Mib::init()
{
#ifdef AGENTPP_USE_THREAD_POOL
	if (!threadPool) {
		threadPool = new ThreadPool();
	}
#endif
	if (is_persistency_activated()) {
		OidListCursor<MibContext> cur;
		lock_mib();
		for (cur.init(&contexts); cur.get(); cur.next()) {
			cur.get()->init_from(get_persistent_objects_path());
		}
		unlock_mib();
	}
	return TRUE;
}

void Mib::save_all()
{
	if (is_persistency_activated()) {
		OidListCursor<MibContext> cur;
		lock_mib();
		for (cur.init(&contexts); cur.get(); cur.next()) {
			cur.get()->save_to(get_persistent_objects_path());
		}
		unlock_mib();
	}
}

bool Mib::save(unsigned int format, const OctetStr& path)
{
	MibConfigFormat* f = configFormats.getNth(format-1);
	if (f) {
		bool ok = TRUE;
		OidListCursor<MibContext> cur;
		lock_mib();
		for (cur.init(&contexts); cur.get(); cur.next()) {
		    ok = f->save(cur.get(), path) && ok;
		}
		unlock_mib();
		return ok;
	}
	return FALSE;
}

MibConfigFormat* Mib::add_config_format(unsigned int formatID,
					MibConfigFormat* format)
{
	while ((unsigned int)configFormats.size() < formatID) {
	    configFormats.add(0);
	}
	return configFormats.overwriteNth(formatID-1, format);
}

bool Mib::load(unsigned int format, const NS_SNMP OctetStr& path)
{
	MibConfigFormat* f = configFormats.getNth(format-1);
	if (f) {
		bool ok = TRUE;
		OidListCursor<MibContext> cur;
		lock_mib();
		for (cur.init(&contexts); cur.get(); cur.next()) {
		    ok = f->load(cur.get(), path) && ok;
		}
		unlock_mib();
		return ok;
	}
	return FALSE;
}

bool Mib::add_agent_caps(const OctetStr& context,
			    const Oidx& sysORID,
			    const OctetStr& sysORDescr)
{
	MibContext* c = get_context(context);
	if (!c) return FALSE;
	sysOREntry* e = (sysOREntry*)c->get(oidSysOREntry);
	if ((!e) || (e->type() != AGENTPP_TABLE)) return FALSE;
	MibTableRow* r = e->find(sysORID);
	if (!r)
		r = e->add_row(e->get_next_avail_index());
	e->set_row(r, sysORID, sysORDescr, sysUpTime::get());
	return TRUE;
}

void Mib::remove_agent_caps(const OctetStr& context,
			    const Oidx& sysORID)
{
	MibContext* c = get_context(context);
	if (!c) return;
	sysOREntry* e = (sysOREntry*)c->get(oidSysOREntry);
	if (!e) return;
	MibTableRow* r = e->find(sysORID);
	if (r)
		e->remove_row(r->get_index());
}

MibEntryPtr Mib::get(const Oidx& key)
{
	return defaultContext->get(key);
}

MibEntryPtr Mib::get(const OctetStr& context, const Oidx& key)
{
	Oidx contextKey(Oidx::from_string(context));
	MibContext* c = contexts.find(&contextKey);
	if (!c) return 0;
	return c->get(key);
}


bool Mib::is_complex_node(const MibEntryPtr& entry)
{
	return ((entry->type() == AGENTPP_TABLE) ||
		(entry->type() == AGENTPP_PROXY) ||
		(entry->type() == AGENTX_NODE) ||
		(entry->type() == AGENTPP_COMPLEX));
}

MibContext* Mib::get_context(const OctetStr& context)
{
	Oidx contextKey(Oidx::from_string(context));
	return contexts.find(&contextKey);
}

int Mib::find_managing_object(MibContext* context,
			      const Oidx& oid,
			      MibEntryPtr& retval,
			      Request* req)
{
	if (!context || oid.len() == 0) return sNMP_SYNTAX_NOSUCHOBJECT;
	// no match?
	if (context->find_lower(oid, retval) != SNMP_ERROR_SUCCESS)
		return sNMP_SYNTAX_NOSUCHOBJECT;
	// update table
	if (is_table_node(retval))
		((MibTable*)retval)->update(req);
	// exact match?
	if (*retval->key() == oid)
		return SNMP_ERROR_SUCCESS;
	// target object could be managed by table or proxy object
	if ((is_complex_node(retval)) &&
	    ((retval->key()->is_root_of(oid)) ||
	     ((oid >= *retval->key()) && (oid < *retval->max_key())))) {
		return SNMP_ERROR_SUCCESS;
	}
	if ((is_leaf_node(retval)) && // for leafs we have to match without .0
	    (oid.in_subtree_of(retval->key()->cut_right(1)))) {
		return sNMP_SYNTAX_NOSUCHINSTANCE;
	}
	return sNMP_SYNTAX_NOSUCHOBJECT;
}

int Mib::find_next(MibContext* context, const Oidx& oid, MibEntryPtr& entry,
		   Request* req, const int, Oidx& nextOid)
{
	if (!context) return sNMP_SYNTAX_NOSUCHOBJECT;
	int err = find_managing_object(context, oid, entry, req);

	if ((err == sNMP_SYNTAX_NOSUCHOBJECT) ||
	    (err == sNMP_SYNTAX_NOSUCHINSTANCE)) {
		if ((err = context->find_upper(oid, entry)) !=
		    SNMP_ERROR_SUCCESS)
			return err;
		else {
			if (is_table_node(entry))
				((MibTable*)entry)->update(req);
			while ((is_complex_node(entry)) &&
			       (entry->is_empty())) {
				MibEntry* e =context->find_next(*entry->key());
				if (!e)
					return sNMP_SYNTAX_NOSUCHOBJECT;
				entry = e;
				if (is_table_node(entry))
					((MibTable*)entry)->update(req);
			}
			return SNMP_ERROR_SUCCESS;
		}
	}
	if ((is_complex_node(entry)) &&
	    ((nextOid = entry->find_succ(oid, req)).len() > 0))
		return SNMP_ERROR_SUCCESS;
	do {
		MibEntry* e = context->find_next(*entry->key());
		if (!e)
			return sNMP_SYNTAX_NOSUCHOBJECT;
		entry = e;
		if (is_table_node(entry))
			((MibTable*)entry)->update(req);
	}
	while ((is_complex_node(entry)) && (entry->is_empty()));
	return SNMP_ERROR_SUCCESS;
}

#ifdef _SNMPv3
/**
 * Check access rights for GETNEXT/GETBULK requests in the SNMPv3
 * security model.
 *
 * @param req - A pointer to the corresponding GETNEXT/BULK request.
 * @param entry - Returns the entry that is the next accessible in
 *                the current view.
 * @param oid - Returnsnms the oid of the object for that access has
 *              been denied or granted. If oid is not empty (size != 0), it
 *              is used to point to the target OID within a table or
 *              complex entry. Otherwise, the successor OID will be searched
 *              and returned.
 * @param nextOid
 *    Provides the already determined next OID for complex
 *    mib entries. If nextOid has the zero length, it will be ignored.
 * @return VACM_accessAllowed if access is granted and any other
 *         VACM error code if access is denied.
 */
int Mib::next_access_control(Request* req, const MibEntryPtr entry, Oidx& oid,
                             const Oidx& nextOid)
{
	int vacmErrorCode = VACM_otherError;
	switch (entry->type()) {
	case AGENTX_LEAF:
	case AGENTPP_LEAF: {
		// Assign the oid of the next value here because we have
		// to check the target object!
		oid = *entry->key();
		vacmErrorCode =
		  requestList->get_vacm()->isAccessAllowed(req->viewName, oid);
		break;
	}
	case AGENTX_NODE: {
		// do not change oid here
		vacmErrorCode = VACM_accessAllowed;
		// we cannot determine next value here, so we assume access
		// is granted - will be checked later again
		break;
	}
	case AGENTPP_TABLE:
	case AGENTPP_COMPLEX:
	case AGENTPP_PROXY: {
                // reuse provided OID for the first iteration (if not empty)
                if (nextOid.len() > 0) {
                    oid = nextOid;
                }
                else {
                    oid = entry->find_succ(oid, req);
                }
		do {
		  if (oid.len() <= 0) {
			oid = *entry->max_key();
			return VACM_notInView;
		  }
		  vacmErrorCode =
		    requestList->get_vacm()->
		    isAccessAllowed(req->viewName, oid);
                  if (vacmErrorCode != VACM_accessAllowed) {
                    oid = entry->find_succ(oid, req);                      
                  }
		} while (vacmErrorCode != VACM_accessAllowed);
		break;
	}
	default: {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("Mib::get_next_request: not implemented (entry->type)");
		LOG(entry->type());
		LOG_END;
	}
	}
	return vacmErrorCode;
}
#endif

bool Mib::set_exception_vb(Request* req, int reqind, int err)
{
	Vbx vb(req->get_oid(reqind));
	vb.set_syntax(err);
	// error status (v1) will be set by RequestList
	req->finish(reqind, vb);
	if (!req->finished()) {
		if (req->version == version1) {
		    return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

bool Mib::process_request(Request* req, int reqind)
{
	// only GET and GETNEXT subrequest can be performed independently
	switch (req->get_type()) {
	case (sNMP_PDU_GET): {

	       	LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
	       	LOG("Mib: process subrequest: get request, oid");
	       	LOG(req->get_transaction_id());
		LOG(req->get_oid(reqind).get_printable());
	       	LOG_END;

		MibEntryPtr entry;
		Oidx tmpoid(req->get_oid(reqind));
		int err;

		lock_mib();
		// entry not available
#ifdef _SNMPv3
		if ((err = find_managing_object(get_context(req->get_context()),
					  tmpoid, entry, req)) != SNMP_ERROR_SUCCESS)
#else
		if ((err = find_managing_object(defaultContext,
						tmpoid, entry, req)) != SNMP_ERROR_SUCCESS)
#endif
		{
			unlock_mib();
			return set_exception_vb(req, reqind, err);
		}
#ifdef _SNMPv3
                // access control
                int vacmErrorCode =
                  requestList->get_vacm()->
		  isAccessAllowed(req->viewName, tmpoid);
		if (vacmErrorCode == VACM_notInView) {
		    unlock_mib();
		    return set_exception_vb(req, reqind,
					    sNMP_SYNTAX_NOSUCHOBJECT);
		}
		else if (vacmErrorCode != VACM_accessAllowed) {
		  unlock_mib();
                  req->vacmError(reqind, vacmErrorCode);
                  return FALSE;
                }
#endif
		entry->start_synch();
		unlock_mib();
		entry->get_request(req, reqind);
		entry->end_synch();
		break;
	}
	case (sNMP_PDU_GETNEXT): {

	       	LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
	       	LOG("Mib: process subrequest: getnext request, oid");
	       	LOG(req->get_transaction_id());
		LOG(req->get_oid(reqind).get_printable());
	       	LOG_END;

		MibEntryPtr entry;
		Oidx tmpoid(req->get_oid(reqind));
                Oidx nextOid;
		lock_mib();
#ifdef _SNMPv3
		int vacmErrorCode = VACM_otherError;
		do {                   
                  nextOid.clear();
		  if (find_next(get_context(req->get_context()), tmpoid, entry,
				req, reqind, nextOid) != SNMP_ERROR_SUCCESS) {
#else
	reprocess:
                // this goto label is used for complex (i.g., proxy) mib
                // entries that to not exactly know their last member
		  if (find_next(defaultContext, tmpoid, entry,
				req, reqind, nextOid) != SNMP_ERROR_SUCCESS) {
#endif
			unlock_mib();
			return set_exception_vb(req, reqind,
						sNMP_SYNTAX_ENDOFMIBVIEW);
		  }
#ifdef _SNMPv3
		} while ((vacmErrorCode = 
                          next_access_control(req, entry, tmpoid, nextOid)) ==
			  VACM_notInView);
                if (vacmErrorCode != VACM_accessAllowed) {
		  unlock_mib();
                  req->vacmError(reqind, vacmErrorCode);
                  return FALSE;
                }
#else
		switch (entry->type()) {
		case AGENTPP_TABLE: {
			tmpoid = ((MibTable*)entry)->find_succ(tmpoid, req);
			break;
		}
		case AGENTX_NODE:
		case AGENTX_LEAF:
		case AGENTPP_LEAF: { break; }
		case AGENTPP_COMPLEX: {
			Oidx nextoid;
			nextoid = entry->find_succ(tmpoid, req);
			if (!nextoid.valid()) {
				goto reprocess;
			}
			else {
				tmpoid = nextoid;
			}
			break;
		}
		case AGENTPP_PROXY: {
			Oidx nextoid;
			nextoid = entry->find_succ(tmpoid, req);
			if (!nextoid.valid()) {
				goto reprocess;
			}
			break;
		}
		default: {
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			LOG("Mib::get_next_request: not implemented (entry->type)");
			LOG(entry->type());
			LOG_END;
		}
		}
#endif
		// set oid of request to found object
		// this can be done because at this point we are sure
		// that we can answer the request
		req->set_oid(tmpoid, reqind);
		entry->start_synch();
		unlock_mib();
		entry->get_next_request(req, reqind);
		entry->end_synch();
		break;
	}
	}
	return TRUE;
}



/**
 * Process a request. If multi-threading is activated, start a
 * thread to actually process the request.
 * @param req - A request.
 */
void Mib::process_request(Request* req)
{
#ifdef _THREADS
	MibMethodCall* call = new MibMethodCall(this,
					  &Mib::do_process_request, req);
#ifdef AGENTPP_USE_THREAD_POOL
	MibTask* mt = new MibTask(call);
	threadPool->execute(mt);
#else
#ifdef _WIN32THREADS
	_beginthread(mib_method_routine_caller, 0, call);
#else
	static pthread_attr_t* attr = 0;
	pthread_t thread;
	if (!attr) {
		attr = new pthread_attr_t;
		pthread_attr_init(attr);
		pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED);
	}
	pthread_create(&thread, attr,
		       &mib_method_routine_caller,
		       (void*) call);
#endif
#endif /*AGENTPP_USE_THREAD_POOL*/
#else
	do_process_request(req);
	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 12);
	LOG("Agent: ready to receive request");
	LOG_END;
#endif
}

#ifdef _SNMPv3
#ifdef _PROXY_FORWARDER

bool	Mib::register_proxy(ProxyForwarder* proxy)
{
	if (!proxies.find(proxy->key())) {
		proxies.add(proxy);
		return TRUE;
	}
	return FALSE;
}

void Mib::unregister_proxy(const OctetStr& contextEngineID,
			   ProxyForwarder::pdu_type type)
{
	Oidx k(Oidx::from_string(contextEngineID));
	k += type;
	proxies.remove(&k);
}

void Mib::proxy_request(Request* req)
{
	Oidx key = Oidx::from_string(req->get_pdu()->get_context_engine_id());
	ProxyForwarder::pdu_type t = ProxyForwarder::ALL;
	key += t;

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	LOG("Agent: Proxy request (contextID)");
	LOG(key.get_printable());
	LOG_END;

	ProxyForwarder* proxy = proxies.find(&key);
	if (!proxy) {
		switch (req->get_pdu()->get_type()) {
		case sNMP_PDU_GET:
		case sNMP_PDU_GETBULK:
		case sNMP_PDU_GETNEXT: {
		  t = ProxyForwarder::READ;
		  break;
		}
		case sNMP_PDU_SET: {
		  t = ProxyForwarder::WRITE;
		  break;
		}
		case sNMP_PDU_V1TRAP:
		case sNMP_PDU_TRAP: {
		  t = ProxyForwarder::NOTIFY;
		  break;
		}
		case sNMP_PDU_INFORM: {
		  t = ProxyForwarder::INFORM;
		  break;
		}
		}
		key.trim(); // cut off type
		key += t;
		proxy = proxies.find(&key);
		if (!proxy) { // fall back
			key = "0.0";
			proxy = proxies.find(&key);
		}

	}
	if ((!proxy) || ((proxy) && (!proxy->process_request(req)))) {
		MibIIsnmpCounters::incProxyDrops();
		Vbx vb(oidSnmpProxyDrops);
		vb.set_value(MibIIsnmpCounters::proxyDrops());
		req->get_pdu()->set_vblist(&vb, 1);
		requestList->report(req);
	}
	else {
		requestList->answer(req);
	}
    delete_request(req);
}
#endif
#endif

/**
 * Internally process a request (within its own thread)
 * @param req - A request.
 */
void Mib::do_process_request(Request* req)
{
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	LOG("Agent: starting thread execution (pduType)(subrequests)");
	LOG(req->get_type());
	LOG(req->subrequests());
	LOG_END;
#ifdef _SNMPv3
#ifdef _PROXY_FORWARDER
	// init myEngineID if not yet initialized
	if ((requestList) && (myEngineID.len() == 0) &&
	    (requestList->get_v3mp()))
	    requestList->get_v3mp()->get_local_engine_id(myEngineID);
	// check for proxy request
	if ((req->get_pdu()->get_context_engine_id().len() > 0) &&
	    (myEngineID != req->get_pdu()->get_context_engine_id())) {
		// use requestList directly to avoid processing by sub classes
		LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
		LOG("Mib: processing proxy request (contextEngineID)");
		LOG(req->get_pdu()->get_context_engine_id().get_printable_hex());
		LOG_END;
	        proxy_request(req);
		LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
		LOG("Agent: finished thread execution");
		LOG_END;
		return;
	}
#endif
#endif
	int n = req->subrequests();
	if (n > 0) {
		int i;
		switch (req->get_type()) {
		case (sNMP_PDU_GET): {

		  LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
		  LOG("Mib: process request: get request, oid");
		  LOG(req->get_transaction_id());
		  for (i=0; i<n; i++)
			LOG(req->get_oid(i).get_printable());
		  LOG_END;

		  for (i=0; i<n; i++) {
			if (!req->is_done(i))
				if (!process_request(req, i)) break;
		  }
		  break;
		}
		case (sNMP_PDU_GETNEXT): {

		  LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
		  LOG("Mib: process request: getnext request, oid");
		  LOG(req->get_transaction_id());
		  for (i=0; i<n; i++)
			LOG(req->get_oid(i).get_printable());
		  LOG_END;

		  for (i=0; i<n; i++) {
			if (!req->is_done(i))
				if (!process_request(req, i)) break;
		  }
		  break;
		}
		case (sNMP_PDU_GETBULK): {
		  process_get_bulk_request(req);
		  break;
		}
		case (sNMP_PDU_SET): {
		  process_set_request(req);
		  break;
		}
		}
	}
	// answer the request
	finalize(req);

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	LOG("Agent: finished thread execution");
	LOG_END;
}

void Mib::process_set_request(Request* req)
{
	int n = req->subrequests();

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	LOG("Mib: process request: set request (tid)(oid)");
	LOG(req->get_transaction_id());

	for (int j=0; j<n; j++)
		LOG(req->get_oid(j).get_printable());
	LOG_END;
	req->phase++; // indicate PHASE_PREPARE
	if (process_prepare_set_request(req) == SNMP_ERROR_SUCCESS) {
		req->phase++; // indicate PHASE_COMMIT
		if (process_commit_set_request(req) !=
		    SNMP_ERROR_SUCCESS) {

			req->phase++;

			LOG_BEGIN(loggerModuleName, WARNING_LOG | 2);
			LOG("Mib: commit failed (tid)");
			LOG(req->get_transaction_id());
			LOG_END;

			process_undo_set_request(req);
			return;
		}
	}
	req->phase = PHASE_CLEANUP;
	process_cleanup_set_request(req);
}

int Mib::process_prepare_set_request(Request* req)
{
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
	LOG("Agent: preparing set request");
	LOG(req->get_transaction_id());
	LOG_END;

	int err;
	MibEntryPtr entry = 0;
	// before processing the SET lock all affected MIB objects
	lock_mib();
	for (int i=0; i<req->subrequests(); i++) {

		if (!req->is_ready(i)) {

			Oidx tmpoid(req->get_oid(i));
			// entry not available
#ifdef _SNMPv3
			if ((err = find_managing_object(get_context(req->
							get_context()),
							tmpoid, entry, req)) !=
#else
			if ((err = find_managing_object(defaultContext,
							tmpoid, entry, req)) !=
#endif
			    SNMP_ERROR_SUCCESS) {
				unlock_mib();
				req->error(i, SNMP_ERROR_NO_CREATION);
				// error status (v1) will be set by RequestList
				//requestList->answer(req);
				return err;
			}
#ifdef _SNMPv3
			// access control
			int vacmErrorCode =
			  requestList->get_vacm()->
			  isAccessAllowed(req->viewName, tmpoid);
			if (vacmErrorCode != VACM_accessAllowed) {
			  unlock_mib();
			  req->vacmError(i, vacmErrorCode);
			  return SNMP_ERROR_NO_ACCESS;
			}
#endif
			// entry->start_synch();
			req->set_locked(i, entry);
			if ((err = entry->prepare_set_request(req, i))
			    != SNMP_ERROR_SUCCESS) {
				unlock_mib();
				req->error(i, err);
				return err;
			}
			// We do not unlock the object here instead
			// it will be unlocked in cleanup_set_request
			// and undo_set_request respectively
			// entry->end_synch();
		}
	}
	unlock_mib();
	return SNMP_ERROR_SUCCESS;
}


int Mib::process_commit_set_request(Request* req)
{
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
	LOG("Agent: committing set request");
	LOG(req->get_transaction_id());
	LOG_END;

	MibEntryPtr entry = 0;

	for (int i=0; i<req->subrequests(); i++) {
		// conceptual row creation via createAndGo
		// requires that a table handles all set operations
		// belonging to that row at one time. So the
		// first set operation to such a row will also handle
		// the other ones.
		if (!req->is_done(i)) {

			entry = req->get_locked(i);
			// entry should be available, but we have to be sure:
			if (!entry) {
				req->error(i, SNMP_ERROR_COMITFAIL);
				return SNMP_ERROR_COMITFAIL;
			}
			// Entry has been locked already by
			// prepare_set_request
			// entry->start_synch();
			int status = SNMP_ERROR_SUCCESS;
			if ((status = entry->commit_set_request(req, i)) !=
			    SNMP_ERROR_SUCCESS) {
				// Make sure error status is set
                                // according to returned status.
                                // Should be SNMP_ERROR_COMITFAIL in most
                                // cases.
				req->error(i, status);
				return status;
			}
		}
	}
	return SNMP_ERROR_SUCCESS;
}

int Mib::process_undo_set_request(Request* req)
{
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
	LOG("Agent: undoing set request");
	LOG(req->get_transaction_id());
	LOG_END;

	MibEntryPtr entry = 0;

	int status = SNMP_ERROR_SUCCESS;
	for (int i=0; i<req->subrequests(); i++) {

		entry = req->get_locked(i);
		// entry must be available:
		if (!entry) {
		        // if there is no entry then the request
                        // is already undone (i.e., a create row
                        // operation)
			continue;
		}
		// Entry has been locked by prepare_set_request
		if (entry->undo_set_request(req, i) !=
		    SNMP_ERROR_SUCCESS) {
			req->set_unlocked(i);
			//entry->end_synch();
			req->error(i, SNMP_ERROR_UNDO_FAIL);
			status = SNMP_ERROR_UNDO_FAIL;
		}
		req->set_unlocked(i);
		//entry->end_synch();
	}
	return status;
}

void Mib::process_cleanup_set_request(Request* req)
{
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
	LOG("Agent: cleaning up set request");
	LOG(req->get_transaction_id());
	LOG_END;

	MibEntryPtr entry = 0;

	for (int i=0; i<req->subrequests(); i++) {

		entry = req->get_locked(i);
		// entry should be available (but anyhow we want to be sure
		if (!entry) {
			return;
		}
		// Entry has been locked by prepare_set_request
		entry->cleanup_set_request(req, i);
		req->set_unlocked(i);
		//entry->end_synch();
	}
}


void Mib::process_get_bulk_request(Request* req)
{
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	LOG("Mib: process request: getbulk request, oid");
	LOG(req->get_transaction_id());
	for (int i=0; i<req->subrequests(); i++)
		LOG(req->get_oid(i).get_printable());
	LOG_END;

	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
	LOG("Mib: getbulk: processing (non repeaters)(max rep)");
	LOG(req->get_non_rep());
	LOG(req->get_max_rep());
	LOG_END;

	int id;
	int subreq = req->subrequests();
	int nonrep = req->get_non_rep();
	int maxrep = req->get_max_rep();
    
    if ((AGENTPP_MAX_GETBULK_REPETITIONS > 0) && 
        (maxrep > AGENTPP_MAX_GETBULK_REPETITIONS)) {

        LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
        LOG("Mib: getbulk: limited max rep (orig maxrep)(limited maxrep)");
        LOG(maxrep);
        LOG((long)AGENTPP_MAX_GETBULK_REPETITIONS);
        LOG_END;
        maxrep = AGENTPP_MAX_GETBULK_REPETITIONS;        
    }

	// do not finish until max_rep is reached - wait for answer at end
	req->inc_outstanding();

	for (id=0; (id<subreq) &&
	           (id<nonrep); id++) {

		if (req->is_done(id)) continue;

		Oidx tmpoid(req->get_oid(id));
		MibEntryPtr entry;
		lock_mib();
                Oidx nextOid;
#ifdef _SNMPv3
		int vacmErrorCode = VACM_otherError;
		do {
                  nextOid.clear();
		  if (find_next(get_context(req->get_context()),
				tmpoid, entry, req, id, nextOid) != SNMP_ERROR_SUCCESS){
#else
	reprocess:
		// this goto label is used for complex (i.g., proxy) mib
		// entries that to not exactly know their last member
		  if (find_next(defaultContext, tmpoid, entry,
				req, id, nextOid) != SNMP_ERROR_SUCCESS) {
#endif
			unlock_mib();
			Vbx vb(req->get_oid(id));
			vb.set_syntax(sNMP_SYNTAX_ENDOFMIBVIEW);
			req->finish(id, vb);
			return;
		  }
#ifdef _SNMPv3
		} while ((vacmErrorCode = 
                          next_access_control(req, entry, tmpoid, nextOid)) ==
			  VACM_notInView);

                if (vacmErrorCode != VACM_accessAllowed) {
			unlock_mib();
			req->vacmError(id, vacmErrorCode);
			return;
                }
#else
		  switch (entry->type()) {
		  case AGENTPP_TABLE: {
			tmpoid = ((MibTable*)entry)->find_succ(tmpoid, req);
			break;
		  }
		  case AGENTX_NODE:
		  case AGENTX_LEAF:
		  case AGENTPP_LEAF: { break; }
		  case AGENTPP_COMPLEX: {
			Oidx nextoid;
			nextoid = entry->find_succ(tmpoid, req);
			if (!nextoid.valid()) {
				goto reprocess;
			}
			else {
				tmpoid = nextoid;
			}
			break;
		  }
		  case AGENTPP_PROXY: {
			Oidx nextoid;
			nextoid = entry->find_succ(tmpoid, req);
			if (!nextoid.valid()) {
				goto reprocess;
			}
			break;
		  }
		  default: {
		    LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		    LOG("Mib::get_next_request: not implemented (entry->type)");
		    LOG(entry->type());
		    LOG_END;
		  }
		  }
#endif
		// set oid of request to found object
		// this can be done because at this point we are sure
		// that we can answer the request
		req->set_oid(tmpoid, id);
		entry->start_synch();
		unlock_mib();
		entry->get_next_request(req, id);
		entry->end_synch();
	}

	// If no repetitions, then do not wait for them
	if (maxrep == 0) req->dec_outstanding();

	for (int j=0; j<maxrep; j++) {

		id = nonrep + req->get_rep()*j;

		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
		LOG("Mib: getbulk: processing repeaters");
		LOG(j);
		LOG_END;

		// finish at last repetition
		if (j == maxrep-1) req->dec_outstanding();

		bool all_endofview = TRUE;

		int endofNextRow = nonrep + req->get_rep()*(j+1);
		for (; (id < req->subrequests()) &&
		       (id < endofNextRow); id++) {

		  if (!req->is_done(id)) {

			Oidx tmpoid(req->get_oid(id));
			MibEntryPtr entry;

			LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
			LOG("Mib: getbulk: processing (id)(until)(oid)");
			LOG(id);
			LOG(endofNextRow);
			LOG(tmpoid.get_printable());
			LOG(req->outstanding);
			LOG_END;

			lock_mib();
                        Oidx nextOid;
#ifdef _SNMPv3
			bool contin = FALSE;
			int vacmErrorCode = VACM_otherError;
			do {
                          nextOid.clear();
			  if (find_next(get_context(req->get_context()),
					tmpoid, entry, req, id, nextOid) !=
#else
       repeating:
			if (find_next(defaultContext,
					tmpoid, entry, req, id, nextOid) !=
#endif
			      SNMP_ERROR_SUCCESS) {

				LOG_BEGIN(loggerModuleName, DEBUG_LOG | 6);
				LOG("Mib: getbulk: end of mib view (id)(left)");
				LOG(id);
				LOG(req->outstanding);
				LOG_END;

				Vbx vb(req->get_oid(id));
				vb.set_syntax(sNMP_SYNTAX_ENDOFMIBVIEW);
				req->finish(id, vb);
				//req->dec_outstanding();
				if (req->finished()) {
					unlock_mib();
					return;
				}
#ifdef _SNMPv3
				contin = TRUE;
				break;
#else
				else {
					unlock_mib();
					continue;
				}
#endif
			  }
#ifdef _SNMPv3
			} while ((vacmErrorCode =
				  next_access_control(req, entry, 
                                                      tmpoid, nextOid)) ==
				 VACM_notInView);

			if (contin) { unlock_mib(); continue; }

			if (vacmErrorCode != VACM_accessAllowed) {
				unlock_mib();
				req->vacmError(id, vacmErrorCode);
				return;
			}
#else
			switch (entry->type()) {
			case AGENTPP_TABLE: {
			  tmpoid = ((MibTable*)entry)->find_succ(tmpoid);
			  break;
			}
			case AGENTX_NODE:
			case AGENTX_LEAF:
			case AGENTPP_LEAF: { break; }
			case AGENTPP_COMPLEX: {
				Oidx nextoid;
				nextoid = entry->find_succ(tmpoid);
				if (nextoid.valid()) {
					tmpoid = nextoid;
				}
				else {
					goto repeating;
				}
				break;
			}
			case AGENTPP_PROXY: {
				Oidx nextoid;
				nextoid = entry->find_succ(tmpoid);
				if (!nextoid.valid()) {
					goto repeating;
				}
				break;
			}
			default: {
			  LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			  LOG("Mib::get_next_request: not implemented (entry->type)");
			  LOG(entry->type());
			  LOG_END;
			}
			}
#endif
			all_endofview = FALSE;
			// set oid of request to found object
			// this can be done because at this point we are sure
			// that we can answer the request
			req->set_oid(tmpoid, id);

			entry->start_synch();
			unlock_mib();
			entry->get_next_request(req, id);
			entry->end_synch();
		    }
		    else {
		        Vbx vb(req->get_value(id));
			if (vb.get_exception_status() !=
			    sNMP_SYNTAX_ENDOFMIBVIEW)
				all_endofview = FALSE;
		    }

		    if (id+1 == endofNextRow) {
			// add one repetition
		        if (all_endofview) {
				req->no_outstanding();
				req->trim_request(id+1);
				return;
			}
			if (!req->init_rep_row(j+1))
				if ((j+1 < maxrep) &&
				    (!req->add_rep_row())) {
					req->dec_outstanding();
					return;
				}
			break; // not really needed
		    }
		}
	        if (all_endofview) {
			req->no_outstanding();
			req->trim_request(id+1);
			return;
		}
	}
}

void Mib::cleanup()
{
	OidListCursor<MibContext> cur;
	lock_mib();
	for (cur.init(&contexts); cur.get(); cur.next()) {
		cur.get()->start_synch();
		OidListCursor<MibEntry> c(cur.get()->get_content());
		for (; c.get(); c.next()) {
			if (c.get()->type() == AGENTPP_TABLE) {
				// synchronized
				((MibTable*)c.get())->remove_unused_rows();
			}
		}
		cur.get()->end_synch();
	}
	unlock_mib();
}


void Mib::finalize(Request* req)
{
	// If responding to a BULK request, trim response to N+M*R variables
	// and make sure we are using right OIDs for ENDOFMIBVIEW vbs.
	req->trim_bulk_response();
    if (requestList) {
        requestList->answer(req);
        delete_request(req);
    }
    else {
        delete req;
        req = 0;
    }
}

#ifdef _THREADS
#ifndef AGENTPP_USE_THREAD_POOL
#ifdef _WIN32THREADS
void mib_method_routine_caller(void* ptr)
#else
void* mib_method_routine_caller(void* ptr)
#endif
{
	MibMethodCall* call = (MibMethodCall*)ptr;
	(call->called_class->*call->method)(call->req);
	delete call;
#ifndef _WIN32THREADS
	return 0;
#endif
}
#endif
#endif

#ifdef AGENTPP_NAMESPACE
}
#endif


