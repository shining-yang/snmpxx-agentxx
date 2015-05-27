/*_############################################################################
  _## 
  _##  agentx_request.h  
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

#ifndef _agentx_request_h_
#define _agentx_request_h_

#include <agentx_pp/agentx.h>
#include <agent_pp/request.h>
#include <agent_pp/vacm.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

class NoVacm: public Vacm {
 public:
	NoVacm();
	virtual ~NoVacm();
  /**
   * Does the user have the rights to access the Oid.
   *
   * All parameters are used to check if access is allowed.
   *
   * @param[in] securityModel The security model used
   * @param[in] securityName  The security name of the user
   * @param[in] securityLevel The security level
   * @param[in] viewType Read, write or notify view
   * @param[in] context The MIB context
   * @param[in] o The Oid the user wants to access
   *
   * @return true if access is allowed
   */
  virtual int isAccessAllowed(const int securityModel,
                              const NS_SNMP OctetStr &securityName,
                              const int securityLevel, const int viewType,
                              const NS_SNMP OctetStr &context, const Oidx &o)
  { return VACM_accessAllowed; }

  /**
   * Is the oid within the view.
   *
   * Check if the given Oid is within the named view.
   *
   * @param[in] viewName Name of the view
   * @param[in] o Oid to check
   *
   * @return true if the oid is within the view
   */
  virtual int isAccessAllowed(const NS_SNMP OctetStr &viewName, const Oidx &o)
  { return VACM_accessAllowed; }

  /**
   * Get the view name for the given params
   *
   * @param[in] securityModel The security model used
   * @param[in] securityName  The security name of the user
   * @param[in] securityLevel The security level
   * @param[in] viewType Select one of mibView_read, write or notify
   * @param[in] context The MIB context
   * @param[out] viewName If found, the name of the matching view
   *
   * @return VACM_viewFound on success, error codes on failure
   */
  virtual int getViewName(const int securityModel, const NS_SNMP OctetStr &securityName,
                          const int securityLevel, const int viewType,
                          const NS_SNMP OctetStr &context, NS_SNMP OctetStr &viewName) 
  { return VACM_viewFound; }
};

/**
 * The AgentXRequest class represents requests made to an AgentX++ 
 * subagent. It is derived from the Request class from AGENT++.
 * Instead of holding a Pdux instance this class holds an AgentXPdu
 * instance.
 *
 * @author Frank Fock
 * @version 1.5
 */

class AGENTXPP_DECL AgentXRequest: public Request {
friend class AgentXRequestList;
friend class SubAgentXMib;
public:

	/**
	 * Construct a Request.
	 *
	 * @param pdu
	 *    a AgentXPdu instance.
	 */
         AgentXRequest(const AgentXPdu&);

	/**
	 * Destructor.
	 */
	virtual ~AgentXRequest();

	/**
	 * Return the object identifier of the specified sub-request.
	 *
	 * @param index
	 *    the index of the sub-request (starting from 0). 
	 * @return 
	 *    An object identifier.
	 */
	virtual Oidx		get_oid(int);		

	virtual void	        set_oid(const Oidx&, int);	

	/**
	 * Return the AgentX request type.
	 *
	 * @return 
	 *    an AgentX PDU type.
	 */
	virtual unsigned short 	get_agentx_type()  	
	       		{ return ((AgentXPdu*)pdu)->get_agentx_type(); }

	/**
	 * Return a pointer to the AgentXPdu.
	 *
	 * @return 
	 *    a pointer to an AgentXPdu instance.
	 */
	AgentXPdu*	get_agentx_pdu() { return (AgentXPdu*)pdu; }

	/**
	 * Add a repetition row to the GETBULK request PDU.
	 *
	 * @return TRUE if there was enough room in the response PDU for
	 *         another repetition, FALSE otherwise.
	 */ 
	virtual bool		add_rep_row();

	/**
	 * Trim the request's variable bindings to a given number
	 * of elements. Elements are removed starting from the end.
	 *
	 * @param count
	 *    the count of variable bindings that should remain. 
	 */ 
	virtual void		trim_request(int);

	/**
	 * Lock the request for exclusive usage. This is used for 
	 * processing SET requests.
	 */
	void			lock();

	/**
	 * Unlock the request.
	 */
	void			unlock();

 private:
	Synchronized mutex; 
};


/*---------------------- class AgentXRequestList ------------------------*/

/**
 * The AgentXRequestList implements a RequestList for AgentX sub-agents.
 *
 * @author Frank Fock
 * @version 1.5
 */

class AGENTXPP_DECL AgentXRequestList: public RequestList {

public:
	/**
	 * Default constructor
	 */
	AgentXRequestList();

	/**
	 * Constructor with AgentX protocol services.
	 *
	 * @param agentxProtocol
	 *    a pointer to an AgentXSlave instance that provides 
	 *    the AgentX protocol operations needed for sending and 
	 *    receiving AgentX PDUs.
	 */
	AgentXRequestList(AgentXSlave*);

	/**
	 * Destructor
	 */
	virtual ~AgentXRequestList();

	/**
	 * Wait a given time for an incoming request.
	 *
	 * @param timeout
	 *    the maximum time in 1/100 seconds to wait for an 
	 *    incoming request.
	 * @return
	 *    a pointer to the received request or 0 if within the 
	 *    timeout period no request has been received.
	 */
	virtual Request*		receive(int);

	/**
	 * Answer a Request by sending the corresponding response PDU.
	 *
	 * @param req - A pointer to a processed Request.
	 */
	virtual void			answer(Request*);

	/**
	 * Remove a given request from the request list and delete it.
	 *
	 * @param request
	 *    a pointer to a request
	 */
	void				remove(Request*);

	/**
	 * Get the agentx protocol service provider for sub-agents.
	 *
	 * @return 
	 *    a pointer to an AgentXSlave instance.
	 */
	AgentXSlave*			get_agentx() { return agentx; }

	/**
	 * Time out any unfinished set requests that have not reached
	 * their next state within AGENTX_TIMEOUT seconds.
	 * These requests allocate locks on corresponding MIB entries 
	 * and probably resources on that entries. Thus, the 
	 * cleanup_set_request method has to be called for these entries
	 * and the locks have to be released.
	 *
	 * Note: This method is not synchronized! Call RequestList::lock()
	 * before using it. This method is automatically called within
	 * AgentXRequestList::add_request. 
	 */
	void				timeout_set_requests();

	/**
	 * Terminate any pending set requests and call cleanup_set_request()
	 * on any allocated resources. This method should be called when
	 * connection to the master agent got lost.
	 */
	void				terminate_set_requests();

protected:

	/**
	 * Add a Request to the RequestList.
	 *
	 * @param req - A pointer to a Request.
	 * @return A pointer to the added Request.
	 */
	virtual Request*		add_request(Request*);

	// External reference to the AgentX protocol services. 
	// Do not delete in Destructor! 
	AgentXSlave*		agentx;
};

#ifdef AGENTPP_NAMESPACE
}
#endif


#endif
