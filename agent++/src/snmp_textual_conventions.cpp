/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - snmp_textual_conventions.cpp  
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

#include <agent_pp/snmp_textual_conventions.h>
#include <agent_pp/system_group.h>
#include <snmp_pp/log.h>

#ifdef AGENTPP_NAMESPACE
using namespace Agentpp;
#endif

static const char *loggerModuleName = "agent++.snmp_textual_conventions";

/*--------------------- class snmpDisplayString -------------------------*/


SnmpDisplayString::SnmpDisplayString(const Oidx& id, mib_access a, 
				     OctetStr* s):
  MibLeaf(id, a, s)
{
	min_size = 0;
	max_size = 255;
}

SnmpDisplayString::SnmpDisplayString(const Oidx& id, mib_access a, 
				     OctetStr* s, bool d):
MibLeaf(id, a, s, (d ? VMODE_DEFAULT : VMODE_LOCKED))
{
	min_size = 0;
	max_size = 255;
}

SnmpDisplayString::SnmpDisplayString(const Oidx& id, mib_access a, 
				     OctetStr* s, bool d, 
				     int min_sz, int max_sz):
  MibLeaf(id, a, s, (d ? VMODE_DEFAULT : VMODE_LOCKED))
{
	min_size = min_sz;
	max_size = max_sz;
}

SnmpDisplayString::~SnmpDisplayString()
{
}

MibEntryPtr SnmpDisplayString::clone()
{
	MibEntryPtr other = 
	  new SnmpDisplayString(oid, access, (OctetStr*)value->clone(), 
	                        (value_mode == VMODE_DEFAULT));
	((SnmpDisplayString*)other)->set_reference_to_table(my_table);
	return other;
}

int SnmpDisplayString::prepare_set_request(Request* req, int& ind) 
{
	int s = MibLeaf::prepare_set_request(req, ind);
	if (s != SNMP_ERROR_SUCCESS) return s;
	OctetStr ostr;
	Vbx vb(req->get_value(ind));
	if (vb.get_value(ostr) != SNMP_CLASS_SUCCESS) 
		return SNMP_ERROR_WRONG_TYPE;
	if ((ostr.len() < min_size) ||
	    (ostr.len() > max_size)) return SNMP_ERROR_WRONG_LENGTH;
	return SNMP_ERROR_SUCCESS;
}

bool SnmpDisplayString::value_ok(const Vbx& vb)
{
	OctetStr ostr;
	if (vb.get_value(ostr) != SNMP_CLASS_SUCCESS) return FALSE;
	for (unsigned int i=0; i<ostr.len(); i++) {
		if (ostr[i] > 127u)
			return FALSE;
		// check for CR NULL or CR LF
		if (ostr[i] == '\r') {
			if (i+1 == ostr.len()) return FALSE;
			if ((ostr[i+1] != 0) && (ostr[i+1] != '\n'))
				return FALSE;
		}
	}
	return TRUE;
}

/*--------------------- class SnmpAdminString -------------------------*/


SnmpAdminString::SnmpAdminString(const Oidx& id, mib_access a, 
				 OctetStr* s):
  OctetStrMinMax(id, a, s, VMODE_NONE, 0, 255)
{
}

SnmpAdminString::SnmpAdminString(const Oidx& id, mib_access a, 
				 OctetStr* s, int m):
  OctetStrMinMax(id, a, s, m, 0, 255)
{
}

SnmpAdminString::SnmpAdminString(const Oidx& id, mib_access a, 
				 OctetStr* s, int m, 
				 int min_sz, int max_sz):
  OctetStrMinMax(id, a, s, m, min_sz, max_sz)
{
}

SnmpAdminString::~SnmpAdminString()
{
}

MibEntryPtr SnmpAdminString::clone()
{
	MibEntryPtr other = 
	  new SnmpAdminString(oid, access, (OctetStr*)value->clone(), 
			      value_mode, min, max);
	((SnmpAdminString*)other)->set_reference_to_table(my_table);
	return other;
}

OctetStr SnmpAdminString::get() 
{
  return *((OctetStr*)value);
}



/*--------------------- class SnmpEngineID -------------------------*/


SnmpEngineID::SnmpEngineID(const Oidx& id, mib_access a, 
                           OctetStr* s):
  MibLeaf(id, a, s)
{
}

SnmpEngineID::SnmpEngineID(const Oidx& id, mib_access a, 
                           OctetStr* s, int d):
  MibLeaf(id, a, s, d)
{
}

SnmpEngineID::~SnmpEngineID()
{
}

MibEntryPtr SnmpEngineID::clone()
{
	MibEntryPtr other = 
	  new SnmpEngineID(oid, access, (OctetStr*)value->clone(), 
			   value_mode);
	((SnmpEngineID*)other)->set_reference_to_table(my_table);
	return other;
}

int SnmpEngineID::prepare_set_request(Request* req, int& ind) 
{
	int s = MibLeaf::prepare_set_request(req, ind);
	if (s != SNMP_ERROR_SUCCESS) return s;

	OctetStr ostr;
	Vbx vb(req->get_value(ind));
	if (vb.get_value(ostr) != SNMP_CLASS_SUCCESS) 
		return SNMP_ERROR_WRONG_TYPE;
	if ((ostr.len() < 5) ||
	    (ostr.len() > 32)) return SNMP_ERROR_WRONG_LENGTH;
	return SNMP_ERROR_SUCCESS;
}

OctetStr SnmpEngineID::create_engine_id(const OctetStr& userText) 
{
	// 8 = v3EngineID, 1370h = 4976 = AGENT++ enterprise ID
	OctetStr engineID((const unsigned char*)"\x80\x00\x13\x70\x05", 5);
	engineID += userText;
	return engineID;
}


OctetStr SnmpEngineID::create_engine_id(unsigned short p) 
{
	// 8 = v3EngineID, 1370h = 4976 = AGENT++ enterprise ID
	OctetStr engineID((const unsigned char*)"\x80\x00\x13\x70\x05", 5);
	unsigned char port[3];
	port[0] = p/256;
	port[1] = p%256;
	port[2] = 0;
	char hname[255];
	size_t len = 255;
	if (gethostname(hname, len) == 0) {
		OctetStr host((unsigned char*)hname, 
			      (strlen(hname)>23) ? 23 : strlen(hname));
		engineID += OctetStr(host);
		engineID += OctetStr(port, 2);
	}
	else {
		time_t ct = time(0);
		char* tp = ctime(&ct);
		OctetStr t((unsigned char*)tp, 
			   (strlen(tp)>23) ? 23 : strlen(tp));
		engineID += t;
		engineID += OctetStr(port, 2);
	}
	return engineID;
}

/*--------------------------- class snmpSpinLock -------------------------*/

/**
 *  SnmpTagValue
 *
 */

SnmpTagValue::SnmpTagValue(const Oidx& id):
   MibLeaf(id, READCREATE, new OctetStr(""), VMODE_DEFAULT)
{
}

SnmpTagValue::SnmpTagValue(const Oidx& id, mib_access a, OctetStr* v, int m):
   MibLeaf(id, a, v, m)
{
}

SnmpTagValue::~SnmpTagValue()
{
}

MibEntryPtr SnmpTagValue::clone()
{
	MibEntryPtr other = new SnmpTagValue(oid);
	((SnmpTagValue*)other)->replace_value(value->clone());
	((SnmpTagValue*)other)->set_reference_to_table(my_table);
	return other;
}

bool SnmpTagValue::value_ok(const Vbx& vb)
{
	OctetStr ostr;
	if (vb.get_value(ostr) != SNMP_CLASS_SUCCESS) return FALSE;
	int length = ostr.len();
	if (length == 0) return TRUE;
	if ((length < 0) || (length > 255)) return FALSE; 	

	for (int i=0; i<length; i++) {
		if (is_delimiter(ostr[i])) return FALSE;
	}
	return TRUE;
}

bool SnmpTagValue::is_delimiter(char c) 
{
	return ((c == 32) || (c == 9) || (c == 13) || (c == 11));
}

int SnmpTagValue::prepare_set_request(Request* req, int& ind) 
{
	int s = MibLeaf::prepare_set_request(req, ind);
	if (s != SNMP_ERROR_SUCCESS) return s;

	OctetStr ostr;
	Vbx vb(req->get_value(ind));
	if (vb.get_value(ostr) != SNMP_CLASS_SUCCESS) 
		return SNMP_ERROR_WRONG_TYPE;
	if ((ostr.len()< 0) || (ostr.len() > 255))
		return SNMP_ERROR_WRONG_LENGTH;
	return SNMP_ERROR_SUCCESS;
}


/**
 *  SnmpTagList
 *
 */

SnmpTagList::SnmpTagList(const Oidx& id, mib_access a, OctetStr* v, int m):
   MibLeaf(id, a, v, m)
{
}

SnmpTagList::SnmpTagList(const Oidx& id):
   MibLeaf(id, READCREATE, new OctetStr(""), VMODE_DEFAULT)
{

}

SnmpTagList::~SnmpTagList()
{
}

MibEntryPtr SnmpTagList::clone()
{
	MibEntryPtr other = new SnmpTagList(oid, access, 0, get_value_mode());
	((SnmpTagList*)other)->replace_value(value->clone());
	((SnmpTagList*)other)->set_reference_to_table(my_table);
	return other;
}

bool SnmpTagList::value_ok(const Vbx& vb)
{
	OctetStr ostr;
	if (vb.get_value(ostr) != SNMP_CLASS_SUCCESS) return FALSE;
	
	// pointer into ostr!
	char* s = (char*)ostr.data();
	if (s) {
		int length = ostr.len();
		if (length > 255) return FALSE; 

		if ((length>0) && 
		    (SnmpTagValue::is_delimiter(s[0]))) return FALSE;
		if ((length>0) && (SnmpTagValue::is_delimiter(s[length-1])))
			return FALSE;
		for (int i=0; i<length; i++) {
			if ((SnmpTagValue::is_delimiter(s[i])) && 
			    ((i+1<length) && 
			     (SnmpTagValue::is_delimiter(s[i+1])))) 
				return FALSE;
		}
	}
	return TRUE;
}

bool SnmpTagList::contains(const char* tag) 
{
	if (!tag) return FALSE;

	int len = ((OctetStr*)value)->len();
	char* l = new char[len+1];
	strncpy(l, (char*)((OctetStr*)value)->data(), len);
	l[len] = 0;

	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 10);
	LOG("SnmpTagList: contains: (taglist)(tag)");
	LOG(l);
	LOG(tag);
	LOG_END;

	char* start = l;
	while ((l+strlen(l) - start >= (int)strlen(tag)) && 
	       (start = strstr(start, tag))) {

	  if (((start == l) || (SnmpTagValue::is_delimiter(*(start-1)))) &&
	      ((l+strlen(l)-start == (int)strlen(tag)) ||
	       (SnmpTagValue::is_delimiter(*(start+strlen(tag)))))) {
		delete[] l;
		return TRUE;
	  }
	  start++;
	}
	delete[] l;
	return FALSE;
}



/**
 *  TestAndIncr
 *
 */

TestAndIncr::TestAndIncr(const Oidx& o):
  MibLeaf(o, READWRITE, new SnmpInt32(0))
{
}

TestAndIncr::~TestAndIncr()
{
}

long TestAndIncr::get_state()
{
	return (long)*((SnmpInt32*)value);
}

void TestAndIncr::set_state(long l)
{
	*((SnmpInt32*)value) = l;
}

int TestAndIncr::set(const Vbx& vb)
{
	// place code for handling operations triggered
	// by this set here
	int status = MibLeaf::set(vb);
	if (get_state() == 2147483647) {
		set_state(0);
	}
	else {
		set_state(get_state()+1);
	}
	return status;
}

int TestAndIncr::prepare_set_request(Request* req, int& reqind) 
{
	long v = 0;
	if (req->get_value(reqind).get_value(v) != SNMP_CLASS_SUCCESS)
	    return SNMP_ERROR_WRONG_TYPE;
	if (v != get_state()) return SNMP_ERROR_INCONSIST_VAL;
	return SNMP_ERROR_SUCCESS;
}

bool TestAndIncr::value_ok(const Vbx& vb)
{
	long v;
	if (vb.get_value(v) != SNMP_CLASS_SUCCESS)
	    return FALSE;
	if ((v < 0) || (v > 2147483647)) return FALSE;
	return TRUE;
}

/*--------------------------- class storageType -------------------------*/

StorageType::StorageType(const Oidx& o, int def):
  MibLeaf(o, READCREATE, new SnmpInt32(def), VMODE_DEFAULT)
{
}

MibEntryPtr StorageType::clone()
{
	MibEntryPtr other = new StorageType(oid, get_state());
	((StorageType*)other)->replace_value(value->clone());
	((StorageType*)other)->set_reference_to_table(my_table);
	return other;
}

bool StorageType::value_ok(const Vbx& vb)
{
	long v;
	if (vb.get_value(v) != SNMP_CLASS_SUCCESS)
	    return FALSE;
	if ((v < 1) || (v > 5)) return FALSE;
	if ((valid()) && (get_state() < 4) && (v >=4)) return FALSE;
	if ((valid()) && (get_state() >= 4)) return FALSE;
	return TRUE;
}

bool StorageType::row_is_volatile()
{
	return (get_state() <= 2);
}

void StorageType::set_state(long state) 
{
	if ((state >= 1) && (state <= 5)) {
		*((SnmpInt32*)value) = state;
	}
}

long StorageType::get_state() 
{
	return (long)*((SnmpInt32*)value);
}


/*--------------------------- class StorageTable ------------------------*/

StorageTable::StorageTable(const StorageTable& other):
  MibTable(other) 
{
	storage_type = other.storage_type;
}

StorageTable::StorageTable(const Oidx& o): MibTable(o)
{
	storage_type = 0;
}

StorageTable::StorageTable(const Oidx& o, unsigned int ilen): 
  MibTable(o, ilen)
{
	storage_type = 0;
}

StorageTable::StorageTable(const Oidx& o, unsigned int ilen, bool a):
  MibTable(o, ilen, a)
{
	storage_type = 0;
}

StorageTable::StorageTable(const Oidx& o, 
			   const index_info* istruc, unsigned int ilen):
  MibTable(o, istruc, ilen)
{
	storage_type = 0;
}

StorageTable::~StorageTable()
{
}

bool StorageTable::is_persistent(MibTableRow* row) 
{
	if (row->get_nth(storage_type)) {
		if (((StorageType*)(row->get_nth(storage_type)))->
		    row_is_volatile()) {
			return FALSE;
		}
	}
	return TRUE;
}	

void StorageTable::add_storage_col(StorageType* col) 
{
	storage_type = generator.size();
	MibTable::add_col(col);
}

void StorageTable::set_storage_type(MibTableRow* row, int storageType) 
{
	if (row->get_nth(storage_type)) {
	    ((StorageType*)(row->get_nth(storage_type)))->
		set_state(storageType);
	}
}

void StorageTable::reset() 
{
	OidListCursor<MibTableRow> cur;
	for (cur.init(&content); cur.get();) {
		long type = ((StorageType*)
			     (cur.get()->get_nth(storage_type)))->get_state();
		if ((type != 4) && (type != 5)) {
		    MibTableRow* victim = cur.get();
		    cur.next();
		    delete content.remove(victim);
		}
		else {
		    cur.next();
		}
	}
}


/*------------------------- class SnmpInt32MinMax ------------------------*/

SnmpInt32MinMax::SnmpInt32MinMax(const Oidx& o, mib_access _access, 
				 const int def_val, 
                                 int vmode, int _min, int _max):
  MibLeaf(o, _access, new SnmpInt32(def_val), vmode) 
{
	min = _min;
	max = _max;
}

SnmpInt32MinMax::SnmpInt32MinMax(const Oidx& o, mib_access _access, 
                                 int _min, int _max):
  MibLeaf(o, _access, new SnmpInt32(0), VMODE_NONE) 
{
	min = _min;
	max = _max;
}


MibEntryPtr SnmpInt32MinMax::clone()
{
	MibEntryPtr other = 
	  new SnmpInt32MinMax(oid, access, 0, get_value_mode(), min, max);
	((SnmpInt32MinMax*)other)->replace_value(value->clone());
	((SnmpInt32MinMax*)other)->set_reference_to_table(my_table);
	return other;
}


bool SnmpInt32MinMax::value_ok(const Vbx& v)
{
	SnmpInt32 si;
	if (v.get_value(si) != SNMP_CLASS_SUCCESS) return FALSE;
	if (((int)si<min) || ((int)si>max)) return FALSE;
	return TRUE;
}

int SnmpInt32MinMax::get_state() 
{
	return *((SnmpInt32*)value);
} 

void SnmpInt32MinMax::set_state(int i)
{
	*((SnmpInt32*)value) = i;
}

/*------------------------- class OctetStrMinMax ------------------------*/

OctetStrMinMax::OctetStrMinMax(const Oidx& o, mib_access _access, 
			       OctetStr* def_val, 
			       int vmode, u_int _min, u_int _max):
  MibLeaf(o, _access, def_val, vmode) 
{
	min = _min;
	max = _max;
}

OctetStrMinMax::OctetStrMinMax(const Oidx& o, mib_access _access, 
			       u_int _min, u_int _max):
  MibLeaf(o, _access, new OctetStr(""), VMODE_NONE) 
{
	min = _min;
	max = _max;
}


MibEntryPtr OctetStrMinMax::clone()
{
	MibEntryPtr other = 
	  new OctetStrMinMax(oid, access, 0, get_value_mode(), min, max);
	((OctetStrMinMax*)other)->replace_value(value->clone());
	((OctetStrMinMax*)other)->set_reference_to_table(my_table);
	return other;
}

int OctetStrMinMax::prepare_set_request(Request* req, int& ind) 
{
	OctetStr ostr;
	Vbx vb(req->get_value(ind));
	if (vb.get_value(ostr) != SNMP_CLASS_SUCCESS) 
		return SNMP_ERROR_WRONG_TYPE;
	if ((ostr.len()<min) || (ostr.len() > max))
		return SNMP_ERROR_WRONG_LENGTH;
	return MibLeaf::prepare_set_request(req, ind);
}

/*----------------- class SnmpMessageProcessingModel -------------------*/

SnmpMessageProcessingModel::SnmpMessageProcessingModel(
  const Oidx& id, mib_access a, int i, int m):
  SnmpInt32MinMax(id, a, i, m, 0, 3)
{
}

SnmpMessageProcessingModel::~SnmpMessageProcessingModel()
{
}

MibEntryPtr SnmpMessageProcessingModel::clone()
{
	MibEntryPtr other = 
	  new SnmpMessageProcessingModel(oid, access, 0, get_value_mode());
	((SnmpMessageProcessingModel*)other)->replace_value(value->clone());
	((SnmpMessageProcessingModel*)other)->set_reference_to_table(my_table);
	return other;
}


/*--------------------- class SnmpSecurityLevel ------------------------*/

SnmpSecurityLevel::SnmpSecurityLevel(const Oidx& id, mib_access a, 
				     int i, int m):
   SnmpInt32MinMax(id, a, i, m, 1, 3)
{
}

SnmpSecurityLevel::~SnmpSecurityLevel()
{
}

MibEntryPtr SnmpSecurityLevel::clone()
{
	MibEntryPtr other = new SnmpSecurityLevel(oid, access, 0, 
					      get_value_mode());
	((SnmpSecurityLevel*)other)->replace_value(value->clone());
	((SnmpSecurityLevel*)other)->set_reference_to_table(my_table);
	return other;
}

/*--------------------- class SnmpSecurityModel ------------------------*/

SnmpSecurityModel::SnmpSecurityModel(const Oidx& id, mib_access a, 
				     int i, int m):
   SnmpInt32MinMax(id, a, i, m, 0, 3)
{
}

SnmpSecurityModel::~SnmpSecurityModel()
{
}

MibEntryPtr SnmpSecurityModel::clone()
{
	MibEntryPtr other = new SnmpSecurityModel(oid, access, 0, 
					      get_value_mode());
	((SnmpSecurityModel*)other)->replace_value(value->clone());
	((SnmpSecurityModel*)other)->set_reference_to_table(my_table);
	return other;
}

/*------------------------ class TimeStamp -----------------------------*/

TimeStamp::TimeStamp(const Oidx& id, mib_access a, int m):
   MibLeaf(id, a, new TimeTicks(0), m)
{
}

TimeStamp::~TimeStamp()
{
}

void TimeStamp::update()
TS_SYNCHRONIZED(
{
	*((TimeTicks*)value) = sysUpTime::get();
})

MibEntryPtr TimeStamp::clone()
{
	MibEntryPtr other = new TimeStamp(oid, access, value_mode);
	((TimeStamp*)other)->replace_value(value->clone());
	((TimeStamp*)other)->set_reference_to_table(my_table);
	return other;
}

/*----------------------- class TimeStampTable --------------------------*/

TimeStampTable::TimeStampTable(const Oidx& o, const index_info* inf, 
			       unsigned int sz, TimeStamp* lc):
   MibTable(o, inf, sz)
{
	lastChange = lc;
}

TimeStampTable::~TimeStampTable()
{
	lastChange = 0;
}

void TimeStampTable::row_added(MibTableRow*, const Oidx&, 
			       MibTable*)
{
	lastChange->update();
}

void TimeStampTable::row_delete(MibTableRow*, const Oidx&, 
				MibTable*)
{
	lastChange->update();
}

void TimeStampTable::updated()
{
	lastChange->update();
}
	
/*----------------------- class DateAndTime --------------------------*/

DateAndTime::DateAndTime(const Oidx& id, 
			 mib_access a, 
			 int mode):
   MibLeaf(id, a, new OctetStr(), mode)
{
	update();
}

DateAndTime::~DateAndTime()
{
}

MibEntryPtr DateAndTime::clone()
{
	MibEntryPtr other = new DateAndTime(oid, access, value_mode);
	((DateAndTime*)other)->replace_value(value->clone());
	((DateAndTime*)other)->set_reference_to_table(my_table);
	return other;
}

OctetStr DateAndTime::get_state()
{
	return *((OctetStr*)value);
}

void DateAndTime::set_state(const OctetStr& s)
{
	*((OctetStr*)value) = s;
}

void DateAndTime::update() 
{
	time_t c = sysUpTime::get_currentTime();
	struct tm* dt = localtime(&c);
	if (!dt) return; // TODO: possibly log an error; Use localtime_r!
	OctetStr val;
	val += (unsigned char)((dt->tm_year+1900) >> 8) & 0xFF;
	val += (unsigned char)(dt->tm_year+1900) & 0xFF;
	val += (unsigned char)dt->tm_mon+1;
	val += (unsigned char)dt->tm_mday;
	val += (unsigned char)dt->tm_hour;
	val += (unsigned char)dt->tm_min;
	val += (unsigned char)dt->tm_sec;
	val += (unsigned char)0;
#if defined __FreeBSD__ || defined __APPLE__
	if (dt->tm_gmtoff >= 0)
		val += '+';
	else
		val += '-';
	unsigned int tz = (unsigned int)abs(dt->tm_gmtoff);
	long timezone = dt->tm_gmtoff;
#else
	// initialize timezone needed?
	// tzset();
#ifdef __CYGWIN__
	long timezone = _timezone;
#endif
	if (timezone < 0)
		val += '+';
	else
		val += '-';
	unsigned int tz = (unsigned int)abs(timezone);
#endif
	val += (unsigned char)((tz / 3600) + 
			       ((dt->tm_isdst > 0)? ((timezone>0)?-1:1) : 0));
	val += (unsigned char)((tz % 3600)/60);
	set_state(val);
}


