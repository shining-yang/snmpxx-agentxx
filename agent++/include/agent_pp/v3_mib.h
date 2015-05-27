/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - v3_mib.h  
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
#ifndef v3_mib_h_
#define v3_mib_h_

#include <agent_pp/agent++.h>

#ifdef _SNMPv3

#include <agent_pp/mib.h>
#include <snmp_pp/usm_v3.h>
#include <snmp_pp/v3.h>
#include <agent_pp/snmp_textual_conventions.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

/**********************************************************************
 *  
 *  class V3SnmpEngine
 * 
 **********************************************************************/

class AGENTPP_DECL V3SnmpEngine: public MibGroup {

public:

	V3SnmpEngine(void);
};

class AGENTPP_DECL V3SnmpEngineID: public MibLeaf {

public:
       V3SnmpEngineID(const NS_SNMP v3MP *mp);
       void get_request(Request*, int);
private:
       const NS_SNMP v3MP *v3mp;
};

class AGENTPP_DECL V3SnmpEngineBoots: public MibLeaf {

public:
       V3SnmpEngineBoots(const NS_SNMP USM *u);
       void get_request(Request*, int);
private:
       const NS_SNMP USM *usm;
};

class AGENTPP_DECL V3SnmpEngineTime: public MibLeaf {

public:
       V3SnmpEngineTime(const NS_SNMP USM *u);
       void get_request(Request*, int);
private:
       const NS_SNMP USM *usm;
};

class AGENTPP_DECL V3SnmpEngineMaxMessageSize: public MibLeaf {

public:
       V3SnmpEngineMaxMessageSize();
};


class AGENTPP_DECL UsmUserTableStatus: public snmpRowStatus 
{
 public:
  UsmUserTableStatus(const Oidx&, int _base_len, NS_SNMP USM *usm);
  virtual ~UsmUserTableStatus();

  virtual MibEntryPtr clone();
  virtual int set(const Vbx& vb);
  virtual int unset();

  void deleteUsmUser();
  void addUsmUser();

 private:
  int base_len;
  NS_SNMP USM *usm;
};

/**
 * This class implements the USMUserTable as specified in RFC 3414.
 *
 * After construction users can be added using the addNewRow() functions
 * and deleted using the deleteRow() function.
 *
 * Users that are added to the table through SNMP are automatically
 * added to the USM. Also, users that are removed from the table are
 * removed from USM. If the USM creates new localized users after an
 * engine id discovery, these entries are automatically added to this
 * table with storage type volatile.
 *
 * When adding rows using the addNewRow() function that takes passwords,
 * The passwords are passed to the USM class, so engine id discovery
 * will work. As this table does not hold the passwords itself, when
 * writing persistent entries to file, the passwords cannot be stored.
 * Therefore after loading the rows from file, engine id discvery
 * will not work. If engine id discovery is needed, the snmp++
 * USM::save_users() and USM::load_users() have to be used.
 *
 * The difference between user name and security name is documented in
 * the USM class.
 */
class AGENTPP_DECL UsmUserTable: public StorageTable
{
 public:
  UsmUserTable();
  virtual ~UsmUserTable();

  virtual bool ready_for_service(Vbx* pvbs, int sz);
  virtual void row_added(MibTableRow* new_row, const Oidx& ind, MibTable*);
  virtual void row_init(MibTableRow* new_row, const Oidx& ind, MibTable*);

  /**
   * Add a user to the table.
   *
   * It is not recommended to add a user where userName and
   * securityName differ.
   *
   * @note This function takes the localized keys as param.
   */
  MibTableRow *addNewRow(const NS_SNMP OctetStr& engineID,
			 const NS_SNMP OctetStr& userName,
			 const NS_SNMP OctetStr& securityName,
			 int authProtocol, const NS_SNMP OctetStr& authKey,
			 int privProtocol, const NS_SNMP OctetStr& privKey,
			 const bool add_to_usm = TRUE);

  /**
   * Add a user to the table.
   *
   * This function calls the addNewRow() function with security name
   * set to user name (which is recommended). The add_to_usm is only used
   * internally by agent++ and should be kept to TRUE.
   *
   * @note This function takes the localized keys as param.
   */
  MibTableRow *addNewRow(const NS_SNMP OctetStr& engineID,
			 const NS_SNMP OctetStr& userName,
			 int authProtocol, const NS_SNMP OctetStr& authKey,
			 int privProtocol, const NS_SNMP OctetStr& privKey,
			 const bool add_to_usm = TRUE)
      { return addNewRow(engineID, userName, userName, authProtocol, authKey,
			 privProtocol, privKey, add_to_usm); };

  /**
   * Add a user to the table and to USM.
   *
   * From the passed params and the local engineID a localized entry
   * is created and added to the table. If addPasswordsToUSM is
   * true, the passwords are passed to the USM, so localized
   * entries can automatically be added when needed.
   *
   * For all users added with passwords, localized entries will be
   * created automatically when needed.
   *
   * @note This function takes the passwords as param.
   */
  MibTableRow *addNewRow(const NS_SNMP OctetStr& userName,
			 const NS_SNMP OctetStr& securityName,
			 int authProtocol,
			 int privProtocol,
			 const NS_SNMP OctetStr& authPassword,
			 const NS_SNMP OctetStr& privPassword,
			 const bool addPasswordsToUSM = true);

  /**
   * Add a user to the table and to USM.
   *
   * From the passed params and the engineID a localized entry
   * is created and added to the table. If addPasswordsToUSM is
   * true, the passwords are passed to the USM, so localized
   * entries can automatically be added when needed.
   *
   * @note This function takes the passwords as param.
   */
  MibTableRow *addNewRow(const NS_SNMP OctetStr& userName,
			 const NS_SNMP OctetStr& securityName,
			 int authProtocol,
			 int privProtocol,
			 const NS_SNMP OctetStr& authPassword,
			 const NS_SNMP OctetStr& privPassword,
			 const NS_SNMP OctetStr& engineID,
			 const bool addPassWordsToUSM = false);

  /**
   * Add a user to the table and to USM.
   *
   * This function calls the addNewRow() function with security name
   * set to user name (which is recommended).
   *
   * @note This function takes the passwords as param.
   */
  MibTableRow *addNewRow(const NS_SNMP OctetStr& userName,
			 int authProtocol,
			 int privProtocol,
			 const NS_SNMP OctetStr& authPassword,
			 const NS_SNMP OctetStr& privPassword)
      { return addNewRow(userName, userName, authProtocol, privProtocol,
			 authPassword, privPassword, true); };

  /**
   * Add a user to the table and to USM.
   *
   * This function calls the addNewRow() function with security name
   * set to user name (which is recommended).
   *
   * @note This function takes the passwords as param.
   */
  MibTableRow *addNewRow(const NS_SNMP OctetStr& userName,
			 int authProtocol,
			 int privProtocol,
			 const NS_SNMP OctetStr& authPassword,
			 const NS_SNMP OctetStr& privPassword,
			 const NS_SNMP OctetStr& engineID,
			 const bool addPassWordsToUSM = false)
      { return addNewRow(userName, userName, authProtocol, privProtocol,
			 authPassword, privPassword, engineID, addPassWordsToUSM); };

  /**
   * Delete a row from the table and from USM.
   *
   * The entry with engineID and userName is removed from the table
   * and from the USM.
   */
  bool deleteRow(const NS_SNMP OctetStr& engineID,
                    const NS_SNMP OctetStr& userName);

  /**
   * Delete all rows from the table and from USM.
   *
   * All entries with the userName are removed from the table
   * and from the USM. If a password was stored in USM, it is also
   * removed.
   * This method is thread-safe regarding concurrent calls on the
   * same user.
   *
   * @param userName
   *    the name of the user whose entries should be deleted.
   */
  void deleteRows(const NS_SNMP OctetStr& userName);
  
  /**
   * Remove all users from the USM. Engine times, boot counter, and ID will
   * not be affected.This method is not synchronized, do not use it while
   * other threads might use the USM.
   */
  void removeAllUsers();

  MibTableRow* get_row(Oidx o) {  return find_index(o); };

  static const Oidx auth_base;
  static const Oidx priv_base;

 protected:
  void initialize_key_change(MibTableRow*);

 private:
  NS_SNMP USM *usm;
};


class AGENTPP_DECL UsmCloneFrom: public MibLeaf
{
 public:
  UsmCloneFrom(Oidx o);
  virtual ~UsmCloneFrom() {};
  virtual int prepare_set_request(Request* req, int& ind);
  virtual void get_request(Request* req, int ind);
  virtual int set(const Vbx& vb);
  virtual bool value_ok(const Vbx& vb);
  virtual MibEntryPtr clone();
 private:
  NS_SNMP USM *usm;
};

class AGENTPP_DECL UsmKeyChange: public MibLeaf
{
 public:
  UsmKeyChange(Oidx o, int keylen, int hashfunction, int typeOfKey,
	       UsmKeyChange* ukc, NS_SNMP USM *u);
  UsmKeyChange(Oidx o, NS_SNMP USM *u);
  virtual ~UsmKeyChange();
  
  virtual int unset();
  void initialize(int keylen, int hashfunction, int typeOfKey, UsmKeyChange* ukc);
  virtual void get_request(Request* req, int ind);
  virtual int prepare_set_request(Request* req, int& ind);
  virtual int set(const Vbx& vb);
  virtual bool value_ok(const Vbx& vb);
  virtual MibEntryPtr clone();

 protected:
  bool process_key_change(NS_SNMP OctetStr& os);

  int type_of_key;
  int key_len;
  int hash_function;
  UsmKeyChange* otherKeyChangeObject;
  NS_SNMP USM *usm;
};

class AGENTPP_DECL UsmOwnKeyChange: public UsmKeyChange
{
 public:
  UsmOwnKeyChange(Oidx o, NS_SNMP USM *u) : UsmKeyChange(o, u) {};
  UsmOwnKeyChange(Oidx o, int keylen, int hashfunction, int typeOfKey,
		  UsmKeyChange* ukc, NS_SNMP USM *u)
    : UsmKeyChange(o, keylen, hashfunction, typeOfKey, ukc, u){};
  virtual ~UsmOwnKeyChange();
  
  virtual int prepare_set_request(Request* req, int& ind);
  virtual MibEntryPtr clone();

 private:

};


/**********************************************************************
 *  
 *  class UsmStatsUnsupportedSecLevels
 * 
 **********************************************************************/

class AGENTPP_DECL UsmStatsUnsupportedSecLevels: public MibLeaf {

public:
	UsmStatsUnsupportedSecLevels(const NS_SNMP USM *u);
        void get_request(Request*, int);
private:
	const NS_SNMP USM *usm;
};

/**********************************************************************
 *  
 *  class UsmStatsNotInTimeWindows
 * 
 **********************************************************************/

class AGENTPP_DECL UsmStatsNotInTimeWindows: public MibLeaf {

public:
	UsmStatsNotInTimeWindows(const NS_SNMP USM *u);
        void get_request(Request*, int);
private:
	const NS_SNMP USM *usm;
};

/**********************************************************************
 *  
 *  class UsmStatsUnknownUserNames
 * 
 **********************************************************************/

class AGENTPP_DECL UsmStatsUnknownUserNames: public MibLeaf {

public:
	UsmStatsUnknownUserNames(const NS_SNMP USM *u);
        void get_request(Request*, int);
private:
	const NS_SNMP USM *usm;
};

/**********************************************************************
 *  
 *  class UsmStatsUnknownEngineIDs
 * 
 **********************************************************************/

class AGENTPP_DECL UsmStatsUnknownEngineIDs: public MibLeaf {

public:
	UsmStatsUnknownEngineIDs(const NS_SNMP USM *u);
        void get_request(Request*, int);
private:
	const NS_SNMP USM *usm;
};

/**********************************************************************
 *  
 *  class UsmStatsWrongDigests
 * 
 **********************************************************************/

class AGENTPP_DECL UsmStatsWrongDigests: public MibLeaf {

public:
	UsmStatsWrongDigests(const NS_SNMP USM *u);
        void get_request(Request*, int);
private:
	const NS_SNMP USM *usm;
};

/**********************************************************************
 *  
 *  class UsmStatsDecryptionErrors
 * 
 **********************************************************************/

class AGENTPP_DECL UsmStatsDecryptionErrors: public MibLeaf {

public:
	UsmStatsDecryptionErrors(const NS_SNMP USM *u);
        void get_request(Request*, int);
private:
	const NS_SNMP USM *usm;
};


/**********************************************************************
 *  
 *  class UsmStats
 * 
 **********************************************************************/

class AGENTPP_DECL UsmStats: public MibGroup {

public:

	UsmStats(void);
};

class	AGENTPP_DECL usm_mib: public MibGroup {

public:

        usm_mib(UsmUserTable*);
};


class AGENTPP_DECL MPDGroup: public MibGroup {

public:

       MPDGroup(void);
};

class AGENTPP_DECL MPDGroupSnmpUnknownSecurityModels: public MibLeaf {

public:
	MPDGroupSnmpUnknownSecurityModels(const NS_SNMP v3MP *mp);
        void get_request(Request*, int);
private:
	const NS_SNMP v3MP *v3mp;
};

class AGENTPP_DECL MPDGroupSnmpInvalidMsgs: public MibLeaf {

public:
	MPDGroupSnmpInvalidMsgs(const NS_SNMP v3MP *mp);
        void get_request(Request*, int);
private:
	const NS_SNMP v3MP *v3mp;
};

class AGENTPP_DECL MPDGroupSnmpUnknownPDUHandlers: public MibLeaf {

public:
	MPDGroupSnmpUnknownPDUHandlers(const NS_SNMP v3MP *mp);
        void get_request(Request*, int);
private:
	const NS_SNMP v3MP *v3mp;
};


class AGENTPP_DECL usmUserAuthProtocol: public MibLeaf {

 public:
	usmUserAuthProtocol(const Oidx&, NS_SNMP USM *u);
	virtual bool value_ok(const Vbx&);
	virtual MibEntryPtr clone();
 private:
	NS_SNMP USM *usm;
};

class AGENTPP_DECL usmUserPrivProtocol: public MibLeaf {

 public:
	usmUserPrivProtocol(const Oidx&, NS_SNMP USM *u);
	virtual bool value_ok(const Vbx&);
	virtual int prepare_set_request(Request*, int&);
	virtual MibEntryPtr clone();
 private:
	NS_SNMP USM *usm;
};
#ifdef AGENTPP_NAMESPACE
}
#endif
#endif

#endif

