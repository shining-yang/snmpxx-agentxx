/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - vacm.cpp  
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

#include <agent_pp/tools.h>
#include <agent_pp/vacm.h>
#include <agent_pp/snmp_textual_conventions.h>
#include <snmp_pp/log.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

static const char *loggerModuleName = "agent++.vacm";

const index_info       	iVacmSecurityToGroupTable[2]	=
{ { sNMP_SYNTAX_INT, FALSE, 1, 1 }, { sNMP_SYNTAX_OCTETS, FALSE, 1, 32 } };
const unsigned int	lVacmSecurityToGroupTable	= 2;

const index_info       	iVacmContextTable[1]		=
{ { sNMP_SYNTAX_OCTETS, FALSE, 0, 32 } };
const unsigned int	lVacmContextTable		= 1;

const index_info       	iVacmAccessTable[4]		=
{ { sNMP_SYNTAX_OCTETS, FALSE, 1, 32 }, { sNMP_SYNTAX_OCTETS, FALSE, 0, 32 },
  { sNMP_SYNTAX_INT, FALSE, 1, 1 }, { sNMP_SYNTAX_INT, FALSE, 1, 1 } };
const unsigned int	lVacmAccessTable		= 4;

const index_info       	iVacmViewTreeFamilyTable[2]	=
{ { sNMP_SYNTAX_OCTETS, FALSE, 1, 32 }, { sNMP_SYNTAX_OID, FALSE, 0, 95 } };
const unsigned int	lVacmViewTreeFamilyTable	= 2;


SnmpUnavailableContexts::SnmpUnavailableContexts():
  MibLeaf(oidSnmpUnavailableContexts, READONLY, new Counter32(0))
{}

void SnmpUnavailableContexts::incValue()
{
  *((SnmpInt32*)value) = (long)*((SnmpInt32*)value) + 1;
}

SnmpInt32 SnmpUnavailableContexts::getValue()
{
  return *((SnmpInt32*)value);
}

SnmpUnknownContexts::SnmpUnknownContexts():
  MibLeaf(oidSnmpUnknownContexts, READONLY, new Counter32(0))
{}

void SnmpUnknownContexts::incValue()
{
  *((SnmpInt32*)value) = (long)*((SnmpInt32*)value) + 1;
}

SnmpInt32 SnmpUnknownContexts::getValue()
{
  return *((SnmpInt32*)value);
}




/*********************************************************************

               VacmContextTable

 ********************************************************************/
VacmContextTable::VacmContextTable(): MibTable(oidVacmContextEntry,
					       iVacmContextTable,
					       lVacmContextTable)
{
  // vacmContextName
  add_col(new SnmpAdminString("1", READONLY, new OctetStr(""),
			      VMODE_DEFAULT, 0, 32));

  add_row("0"); // add default context ""

}

VacmContextTable::~VacmContextTable()
{

}

bool VacmContextTable::addNewRow(const OctetStr& context)
{
  Oidx newIndex = Oidx::from_string(context, TRUE);

  if (find_index(newIndex))
    return FALSE;
  else {
    MibTableRow *mtr = add_row(newIndex);
    mtr->get_nth(0)->replace_value(new OctetStr(context));
    return TRUE;
  }
}

void VacmContextTable::deleteRow(const OctetStr& context)
{
  remove_row(Oidx::from_string(context, TRUE));
}

bool VacmContextTable::isContextSupported(const OctetStr& context)
{
  OidListCursor<MibTableRow> cur;
  for (cur.init(&content); cur.get(); cur.next()) {
    Vbx v=cur.get()->get_nth(0)->get_value();
    OctetStr os;
    v.get_value(os);
    if ((os.len() == context.len()) && (os == context))
      return TRUE;
  }
  return FALSE;

}
/*********************************************************************

               VacmSecurityToGroupTable

 ********************************************************************/

VacmSecurityToGroupTable::VacmSecurityToGroupTable():
  StorageTable(oidVacmSecurityToGroupEntry, iVacmSecurityToGroupTable,
	       lVacmSecurityToGroupTable)
{
  Oidx tmpoid = Oidx(oidVacmSecurityToGroupEntry);

  // vacmSecurityModel
  add_col(new SnmpInt32MinMax("1", NOACCESS, 0, VMODE_NONE, 1, 3));
  // vacmSecurityName
  add_col(new SnmpAdminString("2", NOACCESS, new OctetStr(""),
			      VMODE_NONE, 1, 32));
  // vacmGroupName
  add_col(new SnmpAdminString("3", READCREATE, new OctetStr(""),
			      VMODE_NONE, 1, 32));
  // vacmSecurityToGroupStorageType
  add_storage_col(new StorageType("4", 3));
  // vacmSecurityToGroupStatus
  add_col(new snmpRowStatus("5"));

}

VacmSecurityToGroupTable::~VacmSecurityToGroupTable()
{
}

bool VacmSecurityToGroupTable::ready_for_service(Vbx* pvbs, int sz)
{
  // check if GroupName is set

  if (!pvbs[2].valid())
    return FALSE;
  if (!(generator.get_nth(2)->value_ok(pvbs[2])))
    return FALSE;

  return TRUE;
}

void VacmSecurityToGroupTable::row_added(MibTableRow* new_row,
					 const Oidx& ind, MibTable*)
{
  Oidx o = Oidx(ind);
  if (o.len() == 0) {
    return;
  }
  MibLeaf* ml;
  ml = new_row->get_nth(0);
  ml->set_value(o[0]);

  ml = new_row->get_nth(1);
  o = o.cut_left(2);
  ml->set_value(o.as_string());
}

bool VacmSecurityToGroupTable::could_ever_be_managed(const Oidx& o,
							int& result)

{
  if (!MibTable::could_ever_be_managed(o, result)) return FALSE;
  Oidx tmpoid(o);

  // check oid through value_ok() of the INDEX-objects
  Vbx v;
  v.set_value(SnmpInt32(o[oid.len()+1]));
  if (!(generator.get_nth(0)->value_ok(v)))
    return FALSE;
  v.set_value(o.cut_left(oid.len()+3).as_string());

  if (!(generator.get_nth(1)->value_ok(v)))
    return FALSE;

  return TRUE;
}

bool VacmSecurityToGroupTable::getGroupName(const int& securityModel, const OctetStr& securityName, OctetStr& groupName)
{

  Oidx o=oid; // base
  OctetStr os = securityName;
  o += 3;       // col GroupName
  o += securityModel;
  o += os.len();
  for (unsigned int i=0; i<os.len(); i++)
    o+=os[i];
  //int n,m;
  LOG_BEGIN(loggerModuleName, DEBUG_LOG | 7);
  LOG("Vacm: getGroupName: (model) (name)");
  LOG(securityModel);
  LOG(OctetStr(securityName).get_printable());
  LOG(o.get_printable());
  LOG_END;

  MibLeaf* leaf;
  if ((leaf = find(o)) == 0)
    return FALSE;
  leaf->get_value().get_value(groupName);
  return TRUE;
}

bool VacmSecurityToGroupTable::isGroupNameOK(const OctetStr& os)
{
  Vbx v("0");
  v.set_value(os);
  if (generator.get_nth(2)->value_ok(v))
    return TRUE;
  return FALSE;
}

bool VacmSecurityToGroupTable::addNewRow(const int securityModel,
                                            const OctetStr& securityName,
                                            const OctetStr& groupName,
                                            const int storageType)
{
  Oidx newIndex;
  newIndex += securityModel;
  newIndex += Oidx::from_string(securityName, TRUE);

  if (find_index(newIndex))
  {
    LOG_BEGIN(loggerModuleName, WARNING_LOG | 5);
    LOG("Vacm: Security to group mapping already exists (security name)");
    LOG(securityName.get_printable());
    LOG_END;

    return FALSE;
  }
  else
  {
    MibTableRow *newRow = add_row(newIndex);

    newRow->get_nth(2)->replace_value(new OctetStr(groupName));
    newRow->get_nth(3)->replace_value(new SnmpInt32(storageType));
    newRow->get_nth(4)->replace_value(new SnmpInt32(1));

    return TRUE;
  }
}

void VacmSecurityToGroupTable::deleteRow(const int securityModel, const OctetStr& securityName)
{
  Oidx o;
  o += securityModel;
  o += Oidx::from_string(securityName, TRUE);
  remove_row(o);
}

/* ********************************************************************
 **********************************************************************

                           VacmAccessTable

 **********************************************************************
 ******************************************************************** */

VacmAccessTableStatus::VacmAccessTableStatus(const Oidx& o, int _base_len)
  : snmpRowStatus(o, READCREATE)
{
  base_len = _base_len;
}

MibEntryPtr VacmAccessTableStatus::clone()
{
  snmpRowStatus* other = new VacmAccessTableStatus(oid, base_len);
  other->set_reference_to_table(my_table);
  return other;
}

VacmAccessTable::VacmAccessTable(VacmSecurityToGroupTable* stogt):
  StorageTable(oidVacmAccessEntry, iVacmAccessTable, lVacmAccessTable)
{
  securityToGroupTable = stogt;

  Oidx tmpoid = Oidx(oidVacmSecurityToGroupEntry);

  // vacmAccessContextPrefix
  add_col(new SnmpAdminString("1", NOACCESS, new OctetStr(""),
			      VMODE_DEFAULT, 0, 32));
  // vacmAccessSecurityModel
  add_col(new SnmpInt32MinMax("2", NOACCESS, 0, VMODE_DEFAULT, 0, 3));
  // vacmAccessSecurityLevel
  add_col(new SnmpInt32MinMax("3", NOACCESS, 0, VMODE_DEFAULT, 0, 3));

  // vacmAccessContextMatch
  add_col(new SnmpInt32MinMax("4", READCREATE, 1, VMODE_DEFAULT, 1, 2));
  // vacmAccessReadViewName
  add_col(new SnmpAdminString("5", READCREATE, new OctetStr(""),
			      VMODE_DEFAULT, 0, 32));
  // vacmAccessWriteViewName
  add_col(new SnmpAdminString("6", READCREATE, new OctetStr(""),
			      VMODE_DEFAULT, 0, 32));
  // vacmAccessNotifyViewName
  add_col(new SnmpAdminString("7", READCREATE, new OctetStr(""),
			      VMODE_DEFAULT, 0, 32));
  // vacmAccessStorageType
  add_storage_col(new StorageType("8", 3));
  // vacmAccessStatus
  add_col(new VacmAccessTableStatus("9", tmpoid.len()));
}

VacmAccessTable::~VacmAccessTable()
{
}

bool VacmAccessTable::ready_for_service(Vbx* pvbs, int sz)
{
  // A row is always ready for service
  return TRUE;
}

void VacmAccessTable::row_added(MibTableRow* new_row,
				const Oidx& ind, MibTable*)
{
  // GroupName (erster Index) muss nicht gesetzt werden.

  Oidx o = Oidx(ind);
  if (o.len() == 0) {
    return;
  }
  MibLeaf* ml;

  ml = new_row->get_nth(0);

  Oidx o2 = o;
  o2 = o2.cut_left(o[0]+2);
  o2 = o2.cut_right(2);
  ml->set_value(o2.as_string());

  ml = new_row->get_nth(1);
  ml->set_value(o[o.len()-2]);

  ml = new_row->get_nth(2);
  ml->set_value(o[o.len()-1]);
}

bool VacmAccessTable::could_ever_be_managed(const Oidx& o, int& result)
{
  if (!MibTable::could_ever_be_managed(o, result)) return FALSE;
  Oidx tmpoid(o);

  // check oid through value_ok() of the INDEX-objects
  if (!(securityToGroupTable->isGroupNameOK(o.cut_right(3+o[oid.len()+2+o[oid.len()+1]]).cut_left(oid.len()+2).as_string())))
    return FALSE;

  Vbx v;
  v.set_value(o.cut_right(2).cut_left(oid.len()+3+o[oid.len()+1]).as_string());
  if (!(generator.get_nth(0)->value_ok(v)))
    return FALSE;

  v.set_value(SnmpInt32(o[o.len()-2]));
  if (!(generator.get_nth(1)->value_ok(v)))
    return FALSE;

  v.set_value(SnmpInt32(o[o.len()-1]));
  if (!(generator.get_nth(2)->value_ok(v)))
    return FALSE;

  return TRUE;
}


bool VacmAccessTable::getViewName(const OctetStr& group,
                                     const OctetStr& context,
                                     const int securityModel,
                                     const int securityLevel,
                                     const int viewType,
                                     OctetStr& viewName)
{
  bool found = FALSE; 
  bool foundMatchModel = FALSE;
  bool foundMatchContextExact = FALSE;
  unsigned int foundContextPrefixLength = 0;
  unsigned int foundSecurityLevel = 0;
  MibTableRow* foundRow = NULL;

  unsigned int groupLen = group.len();
  Oidx ind;

  LOG_BEGIN(loggerModuleName, DEBUG_LOG | 7);
  LOG("Vacm: getViewName: (group) (context) (model) (level) (type)");
  LOG(OctetStr(group).get_printable());
  LOG(OctetStr(context).get_printable());
  LOG(securityModel);
  LOG(securityLevel);
  LOG(viewType);
  LOG_END;

  OidListCursor<MibTableRow> cur;
  cur.init(&content);
  Oidx groupIndex(Oidx::from_string(group));
  if (!cur.lookup(&groupIndex)) {
      cur.init(&content);
  }
  for (; cur.get(); cur.next()) {
    if (cur.get()->get_row_status()->get() != rowActive) {
      continue;
    }

    if (cur.get()->get_index()[0] == groupLen) {
      ind = cur.get()->get_index();

      if (ind.cut_right(ind[ind[0]+1]+3).cut_left(1).as_string() == group) {

	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 7);
	LOG("Vacm: getViewName: (matched group)");
	LOG(OctetStr(ind.cut_right(ind[ind[0]+1]+3).cut_left(1).as_string()).get_printable());
	LOG_END;

        if ((((int)ind[ind.len() - 2] == securityModel) ||
#ifdef _SNMPv3
             ((int)ind[ind.len() - 2] == SNMP_SECURITY_MODEL_ANY)
#else
             (ind[ind.len() - 2] == 0)
#endif
             ) &&
            ((int)ind[ind.len() - 1] <= securityLevel)) {
          OctetStr pref=OctetStr(ind.cut_left(ind[0]+2).cut_right(2).as_string());
          int exactMatch;
          cur.get()->get_nth(3)->get_value(exactMatch);

	  LOG_BEGIN(loggerModuleName, DEBUG_LOG | 8);
	  LOG("Vacm: getViewName: (matching)(prefix)(context)");
	  LOG(exactMatch);
	  LOG(pref.get_printable());
	  LOG(OctetStr(context).get_printable());
	  LOG_END;

          if (((exactMatch == 1) && (pref == context)) ||
              ((exactMatch == 2) &&
	       ((pref.len() <= context.len()) &&
		(pref.nCompare(pref.len(), context) == 0)))) {

	    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 7);
	    LOG("Vacm: getViewName: (matched context)");
	    LOG(pref.get_printable());
	    LOG_END;

            if (found) { // found a row before
              bool replace = FALSE;
              if ((!foundMatchModel) && ((int)ind[ind.len() - 2] == securityModel))
                replace = TRUE;
              else
                if ((!foundMatchModel) || ((int)ind[ind.len() - 2] == securityModel)) {
                  if ((!foundMatchContextExact) && (exactMatch == 1))
                    replace = TRUE;
                  else
		    if ((!foundMatchContextExact) || (exactMatch == 1)) {
                      if (foundContextPrefixLength < pref.len())
                        replace = TRUE;
                      else
                        if (foundContextPrefixLength ==  pref.len())
                          if (foundSecurityLevel < ind[ind.len() - 1])
                            replace = TRUE;
		    }
		}

              if (replace) {
                foundMatchModel = ((int)ind[ind.len() - 2] == securityModel);
                foundMatchContextExact = (exactMatch == 1);
                foundContextPrefixLength = pref.len();
                foundSecurityLevel = ind[ind.len() - 1];
                foundRow = cur.get();
              }
            }
            else { // this is the first row that was found
              found = TRUE;
              foundMatchModel = ((int)ind[ind.len() - 2] == securityModel);
              foundMatchContextExact = (exactMatch == 1);
              foundContextPrefixLength = pref.len();
              foundSecurityLevel = ind[ind.len() - 1];
              foundRow = cur.get();
            }

          }
        }
      }
    }
  }
  if (found) {
    switch (viewType) {
      case mibView_read: { foundRow->get_nth(4)->get_value(viewName); break; }
      case mibView_write: { foundRow->get_nth(5)->get_value(viewName); break; }
      case mibView_notify:{ foundRow->get_nth(6)->get_value(viewName); break; }
    }
    return TRUE;
  }
  return FALSE;
}

bool VacmAccessTable::addNewRow(const OctetStr& groupName,
                                   const OctetStr& prefix,
                                   const int securityModel, const int securityLevel,
                                   const int match, const OctetStr& readView,
                                   const OctetStr& writeView,
                                   const OctetStr& notifyView, const int storageType)
{
  Oidx newIndex;

  newIndex = Oidx::from_string(groupName, TRUE);
  newIndex += Oidx::from_string(prefix, TRUE);
  newIndex += securityModel;
  newIndex += securityLevel;

  if (find_index(newIndex))
    return FALSE;
  else {
    MibTableRow *newRow = add_row(newIndex);

    newRow->get_nth(3)->replace_value(new SnmpInt32(match));
    newRow->get_nth(4)->replace_value(new OctetStr(readView));
    newRow->get_nth(5)->replace_value(new OctetStr(writeView));
    newRow->get_nth(6)->replace_value(new OctetStr(notifyView));
    newRow->get_nth(7)->replace_value(new SnmpInt32(storageType));
    newRow->get_nth(8)->replace_value(new SnmpInt32(1));

    return TRUE;
  }
}
void VacmAccessTable::deleteRow(const OctetStr& groupName, const OctetStr& prefix,
                                const int securityModel, const int securityLevel)
{
  Oidx o;

  o =  Oidx::from_string(groupName, TRUE);
  o += Oidx::from_string(prefix, TRUE);
  o += securityModel;
  o += securityLevel;

  remove_row(o);
}


/*********************************************************************

               VacmViewTreeFamilyTable

 ********************************************************************/

VacmViewTreeFamilyTableStatus::VacmViewTreeFamilyTableStatus(const Oidx& o,
				int _base_len):snmpRowStatus(o, READCREATE)
{
  base_len = _base_len;
}

int VacmViewTreeFamilyTableStatus::set(const Vbx& vb)
{
	undo = value->clone();
	long rs;
	if (vb.get_value(rs) != SNMP_CLASS_SUCCESS)
	    return SNMP_ERROR_WRONG_TYPE;

	switch (rs) {
	case rowNotInService: {
	  OctetStr viewName = ((SnmpAdminString*)my_row->first())->get();
	  ViewNameIndex* views =
	    ((VacmViewTreeFamilyTable*)my_table)->viewsOf(viewName);
	  if (!views) {
	    LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
	    LOG("VacmViewTreeFamilyTableStatus: internal error: view name not found (viewName)");
	    LOG(viewName.get_printable());
	    LOG_END;
	  }
	  else {
	    views->remove(my_row);

	    LOG_BEGIN(loggerModuleName, INFO_LOG | 2);
	    LOG("VacmViewTreeFamilyTable: (sub)view disabled (viewName)");
	    LOG(viewName.get_printable());
	    LOG_END;
	  }
	  break;
	}
	case rowActive: {
	  OctetStr viewName = ((SnmpAdminString*)my_row->first())->get();
	  ViewNameIndex* views =
	    ((VacmViewTreeFamilyTable*)my_table)->viewsOf(viewName);
	  if (!views) {
	    views = new ViewNameIndex(viewName);
	    views->add(my_row);
	    ((VacmViewTreeFamilyTable*)my_table)->viewNameIndex.add(views);

	    LOG_BEGIN(loggerModuleName, INFO_LOG | 2);
	    LOG("VacmViewTreeFamilyTable: adding view name (viewName)");
	    LOG(viewName.get_printable());
	    LOG_END;
	  }
	  else {
	    views->add(my_row);

	    LOG_BEGIN(loggerModuleName, INFO_LOG | 2);
	    LOG("VacmViewTreeFamilyTable: updating view (viewName)");
	    LOG(viewName.get_printable());
	    LOG_END;
	  }
	  break;
	}
	}
	return snmpRowStatus::set(vb);
}

MibEntryPtr VacmViewTreeFamilyTableStatus::clone()
{
  snmpRowStatus* other = new VacmViewTreeFamilyTableStatus(oid, base_len);
  other->set_reference_to_table(my_table);
  return other;
}



VacmViewTreeFamilyTable::VacmViewTreeFamilyTable()
  : StorageTable(oidVacmViewTreeFamilyEntry, iVacmViewTreeFamilyTable,
		 lVacmViewTreeFamilyTable)
{
  Oidx tmpoid = Oidx(oidVacmViewTreeFamilyEntry);

  // VacmViewTreeFamilyViewName
  add_col(new SnmpAdminString("1", NOACCESS, new OctetStr(""),
			      VMODE_DEFAULT, 1, 32));
  // VacmViewTreeFamilySubtree
  add_col(new MibLeaf("2", NOACCESS, new Oid("0.0"),VMODE_DEFAULT));
  // VacmViewTreeFamilyMask
  add_col(new SnmpAdminString("3", READCREATE, new OctetStr(""),
			      VMODE_DEFAULT, 0, 16));
  // VacmViewTreeFamilyType
  add_col(new SnmpInt32MinMax("4", READCREATE, 1, VMODE_DEFAULT, 1, 2));
  // VacmViewTreeFamilyStorageType
  add_storage_col(new StorageType("5", 3));
  // VacmViewTreeFamilyStatus
  add_col(new VacmViewTreeFamilyTableStatus("6", tmpoid.len()));

  buildViewNameIndex();
}

VacmViewTreeFamilyTable::~VacmViewTreeFamilyTable()
{
}

bool VacmViewTreeFamilyTable::ready_for_service(Vbx* pvbs, int sz)
{
  // Defaultwerte sind ok!
  return TRUE;
}

void VacmViewTreeFamilyTable::row_added(MibTableRow* new_row,
					const Oidx& ind, MibTable*)
{
  Oidx o = Oidx(ind);
  if (o.len() == 0) {
    return;
  }    
  MibLeaf* ml;
  ml = new_row->get_nth(0);
  ml->set_value(o.cut_right(o[o[0]+1]+1).cut_left(1).as_string());

  ml = new_row->get_nth(1);
  ml->set_value(o.cut_left(o[0]+2));
}

void VacmViewTreeFamilyTable::row_activated(MibTableRow* row,
					    const Oidx& ind, MibTable*)
{
    // add row to the index
    OctetStr viewName = ((SnmpAdminString*)row->first())->get();

    ViewNameIndex* views = viewsOf(viewName);
    if (views) views->add(row);
    else viewNameIndex.add(new ViewNameIndex(viewName))->add(row);
}

void VacmViewTreeFamilyTable::row_deactivated(MibTableRow* row,
					      const Oidx& ind, MibTable*)
{
    ViewNameIndex* views = viewsOf(((SnmpAdminString*)row->first())->get());
    if (views) {
	views->remove(row);
	if (views->isEmpty()) delete viewNameIndex.remove(views);
    }
}

void VacmViewTreeFamilyTable::row_delete(MibTableRow* row,
					 const Oidx& ind, MibTable* t)
{
	row_deactivated(row, ind, t);
}

bool VacmViewTreeFamilyTable::could_ever_be_managed(const Oidx& o,
						       int& result)

{
  if (!MibTable::could_ever_be_managed(o, result)) return FALSE;
  Oidx tmpoid(o);

  // check oid through value_ok() of the INDEX-objects
  Vbx v;
  v.set_value(o.cut_right(1+o[oid.len()+2+
			  o[oid.len()+1]]).cut_left(oid.len()+2).as_string());
  if (!(generator.get_nth(0)->value_ok(v)))
    return FALSE;

  v.set_value(o.cut_left(oid.len()+3+o[oid.len()+1]));
  if (!(generator.get_nth(1)->value_ok(v)))
    return FALSE;

  return TRUE;
}

int VacmViewTreeFamilyTable::isInMibView(const OctetStr& viewName, const Oidx& subtree)
{
  bool found = FALSE;
  unsigned int foundSubtreeLen = 0;
  MibTableRow* foundRow = NULL;
  Oidx ind;

  LOG_BEGIN(loggerModuleName, DEBUG_LOG | 7);
  LOG("Vacm: isInMibView: (viewName) (subtree)");
  LOG(OctetStr(viewName).get_printable());
  LOG(Oid(subtree).get_printable());
  LOG_END;

  ViewNameIndex* views = viewsOf(viewName);

  if (!views) return VACM_noSuchView;


  ListCursor<MibTableRow> cur;
  for (cur.init(&views->views); cur.get(); cur.next()) {

    ind = cur.get()->get_index();
    ind = ind.cut_left(ind[0]+1);

    if (ind[0] > subtree.len())
      continue;
    ind = ind.cut_left(1);
    OctetStr mask;
    cur.get()->get_nth(2)->get_value(mask);
    bool ok = TRUE;
    for (unsigned int i=0; i<ind.len(); i++) {
      if ((ind[i] != subtree[i]) && (bit(i, mask))) {
	ok = FALSE;
	break;
      }
    }
    if (ok) {
      if (found) { // already found one
	if (foundSubtreeLen <= ind.len()) {
            foundRow = cur.get();
            foundSubtreeLen = ind.len();
	}
      }
      else { // first row found
	found = TRUE;
	foundSubtreeLen = ind.len();
	foundRow = cur.get();
      }
    }
  }
  if (found) {
    int tmpval;
    foundRow->get_nth(3)->get_value(tmpval);
    if (tmpval==1) { //included

      LOG_BEGIN(loggerModuleName, DEBUG_LOG | 9);
      LOG("Vacm: isInMibView: access allowed");
      LOG_END;
      return VACM_accessAllowed;
    }
    else //excluded
      return VACM_notInView;
  }
  return VACM_notInView;
}

bool VacmViewTreeFamilyTable::bit(unsigned int nr, OctetStr& o)
{
  // return TRUE if bit is "1" or o is too short
  if (o.len() <= (nr/8))
    return TRUE;
  return (o[nr/8] & (0x01 << (7 - (nr % 8)))) > 0;
}

/**
  * Build the viewNameIndex from the current contents of the table
  */
void VacmViewTreeFamilyTable::buildViewNameIndex()
{
  OidListCursor<MibTableRow> cur;
  viewNameIndex.clear();
  OctetStr viewName;
  for (cur.init(&content); cur.get(); cur.next()) {

    viewName = ((SnmpAdminString*)cur.get()->first())->get();
    ViewNameIndex* views = viewsOf(viewName);
    if (views) views->add(cur.get());
    else viewNameIndex.add(new ViewNameIndex(viewName))->add(cur.get());
  }
}


bool VacmViewTreeFamilyTable::addNewRow(const OctetStr& viewName, const Oidx& subtree,
                                           const OctetStr& mask, const int type,
                                           const int storageType)
{
  Oidx newIndex;

  newIndex = Oidx::from_string(viewName, TRUE);
  newIndex += subtree.len();
  newIndex += subtree;

  if (find_index(newIndex))
    return FALSE;
  else {
    MibTableRow *newRow = add_row(newIndex);

    newRow->get_nth(2)->replace_value(new OctetStr(mask));
    newRow->get_nth(3)->replace_value(new SnmpInt32(type));
    newRow->get_nth(4)->replace_value(new SnmpInt32(storageType));
    newRow->get_nth(5)->replace_value(new SnmpInt32(1));

    row_activated(newRow, newIndex, 0);
    return TRUE;
  }
}

void VacmViewTreeFamilyTable::row_init(MibTableRow* newRow, const Oidx& ind,
				       MibTable* src)
{
	if (!src) {
		Oidx o(ind);
		o.trim(o.len()-(ind[0]+1)); // cut off subtree
		o = o.cut_left(1); // cut off length
		OctetStr viewName(o.as_string());

		ViewNameIndex* views = viewsOf(viewName);
		if (views) views->add(newRow);
		else viewNameIndex.add(new ViewNameIndex(viewName))->
		       add(newRow);
	}
}

void VacmViewTreeFamilyTable::deleteRow(const OctetStr& viewName, const Oidx& subtree)
{
  Oidx o;

  o = Oidx::from_string(viewName, TRUE);
  o += subtree.len();
  o += subtree;

  remove_row(o);
}


ViewNameIndex* VacmViewTreeFamilyTable::viewsOf(const OctetStr& viewName)
{
  OctetStr vName(viewName);
  ListCursor<ViewNameIndex> cur;
  for (cur.init(&viewNameIndex); cur.get(); cur.next()) {

    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 8);
    LOG("VacmViewTreeFamilyTable: isInMibView: (viewName) (match)");
    LOG(vName.get_printable());
    LOG(cur.get()->name.get_printable());
    LOG_END;

    if (cur.get()->name == vName) return cur.get();
  }
  return 0;
}

/*********************************************************************

               VacmMIB

 ********************************************************************/
VacmMIB::VacmMIB(Vacm::ClassPointers vcp): MibGroup(oidVacmMIBObjects,
						  "snmpVacmMIB")
{
  add(vcp.contextTable);
  add(vcp.securityToGroupTable);
  add(vcp.accessTable);
  add(vcp.viewTreeFamilyTable);
  add(new TestAndIncr("1.3.6.1.6.3.16.1.5.1.0"));
}

Vacm::Vacm()
{
  vcp.contextTable = 0;
  vcp.securityToGroupTable = 0;
  vcp.accessTable = 0;
  vcp.viewTreeFamilyTable = 0;
  vcp.snmpUnknownContexts = 0;
  vcp.snmpUnavailableContexts = 0;
}

Vacm::Vacm(Mib& mib)
{
  vcp.contextTable = new VacmContextTable();
  vcp.securityToGroupTable =new VacmSecurityToGroupTable();
  vcp.accessTable = new VacmAccessTable(vcp.securityToGroupTable);
  vcp.viewTreeFamilyTable = new VacmViewTreeFamilyTable();
  vcp.snmpUnknownContexts = new SnmpUnknownContexts();
  vcp.snmpUnavailableContexts = new SnmpUnavailableContexts();
  mib.add(new VacmMIB(vcp));
  mib.add(vcp.snmpUnknownContexts);
  mib.add(vcp.snmpUnavailableContexts);
}

Vacm::~Vacm(void)
{

}

bool Vacm::addNewContext(const OctetStr &newContext)
{
  return vcp.contextTable->addNewRow(newContext);
}

void Vacm::deleteContext(const OctetStr &context)
{
  vcp.contextTable->deleteRow(context);
}

bool Vacm::addNewGroup(const int securityModel, const OctetStr& securityName,
                          const OctetStr& groupName, const int storageType)
{
  return vcp.securityToGroupTable->addNewRow(securityModel, securityName, groupName, storageType);
}

void Vacm::deleteGroup(const int securityModel, const OctetStr& securityName)
{
  vcp.securityToGroupTable->deleteRow(securityModel, securityName);
}

bool Vacm::addNewAccessEntry(const OctetStr& groupName, const OctetStr& prefix,
                                const int securityModel, const int securityLevel,
                                const int match, const OctetStr& readView, const OctetStr& writeView,
                                const OctetStr& notifyView, const int storageType)
{
  return vcp.accessTable->addNewRow(groupName, prefix, securityModel, securityLevel,
                                      match, readView, writeView, notifyView, storageType);
}

void Vacm::deleteAccessEntry(const OctetStr& groupName, const OctetStr& prefix,
                             const int securityModel, const int securityLevel)
{
  vcp.accessTable->deleteRow(groupName, prefix, securityModel, securityLevel);
}

bool Vacm::addNewView(const OctetStr& viewName, const Oidx& subtree,
                      const OctetStr& mask, const int type, const int storageType)
{
  return vcp.viewTreeFamilyTable->addNewRow(viewName, subtree, mask, type, storageType);
}

void Vacm::deleteView(const OctetStr& viewName, const Oidx& subtree)
{
  vcp.viewTreeFamilyTable->deleteRow(viewName, subtree);
}

void Vacm::incUnknownContexts()
{
  vcp.snmpUnknownContexts->incValue();
}

SnmpInt32 Vacm::getUnknownContexts()
{
  return vcp.snmpUnknownContexts->getValue();
}

int Vacm::isAccessAllowed(const int securityModel, const OctetStr &securityName,
                              const int securityLevel, const int viewType,
                              const OctetStr &context, const Oidx &o)
{
  LOG_BEGIN(loggerModuleName, DEBUG_LOG | 7);
  LOG("Vacm: Access requested for: (model) (name) (level) (type) (context) (oid)");
  LOG(securityModel);
  LOG(securityName.get_printable());
  LOG(securityLevel);
  LOG(viewType);
  LOG(context.get_printable());
  LOG(o.get_printable());
  LOG_END;

  if (!(vcp.contextTable->isContextSupported(context)))
    return VACM_noSuchContext;

  OctetStr groupName;
  if (!(vcp.securityToGroupTable->getGroupName(securityModel, securityName, groupName)))
    return VACM_noGroupName;

  OctetStr viewName;
  if (!(vcp.accessTable->getViewName(groupName, context,
                                     securityModel, securityLevel, viewType, viewName)))
    return VACM_noAccessEntry;

  if (viewName.len() == 0)
    return VACM_noSuchView;

  return (vcp.viewTreeFamilyTable->isInMibView(viewName, o));
}

int Vacm::getViewName(const int securityModel, const OctetStr &securityName,
                         const int securityLevel,
                         const int viewType, const OctetStr &context, OctetStr &viewName)
{
  LOG_BEGIN(loggerModuleName, DEBUG_LOG | 7);
  LOG("Vacm: getViewName for: (model) (name) (level) (type) (context)");
  LOG(securityModel);
  LOG(securityName.get_printable());
  LOG(securityLevel);
  LOG(viewType);
  LOG(context.get_printable());
  LOG_END;

  if (!(vcp.contextTable->isContextSupported(context)))
    return VACM_noSuchContext;

  OctetStr groupName;
  if (!(vcp.securityToGroupTable->getGroupName(securityModel, securityName, groupName)))
    return VACM_noGroupName;

  if (!(vcp.accessTable->getViewName(groupName, context,
                                     securityModel, securityLevel, viewType, viewName)))
    return VACM_noAccessEntry;

  if (viewName.len() == 0)
    return VACM_noSuchView;

  return VACM_viewFound;
}


int Vacm::isAccessAllowed(const OctetStr &viewName, const Oidx &o)
{
  LOG_BEGIN(loggerModuleName, DEBUG_LOG | 7);
  LOG("Vacm: Access requested for: (viewName) (oid)");
  LOG(viewName.get_printable());
  LOG(o.get_printable());
  LOG_END;

  return (vcp.viewTreeFamilyTable->isInMibView(viewName, o));
}

void Vacm::clear() {
   vcp.contextTable->clear();
   vcp.securityToGroupTable->clear();
   vcp.accessTable->clear();
   vcp.viewTreeFamilyTable->clear();    
}

#ifdef AGENTPP_NAMESPACE
}
#endif
