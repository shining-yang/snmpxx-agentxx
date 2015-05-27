/*_############################################################################
  _## 
  _##  agentx_queue.h  
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

#ifndef _agentx_queue_h_
#define _agentx_queue_h_

#include <agentx_pp/agentx_threads.h>
#include <agentx_pp/agentx_pdu.h>

#include <agent_pp/List.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


/*-------------------------- class AgentXQueue -----------------------*/

/**
 * The AgentXQueue class holds pending AgentX requests. Requests that
 * have a timestamp of 0 are supposed to not being sent yet. Thus,
 * requests that have a timestamp value of 0, will be ignored when
 * requests are being timed out. 
 *
 * @author Frank Fock
 * @version 1.4.7
 */
#if !defined (AGENTXPP_DECL_TEMPL_LIST_AGENTXPDU)
#define AGENTXPP_DECL_TEMPL_LIST_AGENTXPDU
	AGENTXPP_DECL_TEMPL template class AGENTXPP_DECL List<AgentXPdu>;
#endif
#if !defined (AGENTXPP_DECL_TEMPL_LISTCURSOR_AGENTXPDU)
#define AGENTXPP_DECL_TEMPL_LISTCURSOR_AGENTXPDU
	AGENTXPP_DECL_TEMPL template class AGENTXPP_DECL ListCursor<AgentXPdu>;
#endif
#if !defined (AGENTXPP_DECL_TEMPL_ORDEREDLIST_AGENTXPDU)
#define AGENTXPP_DECL_TEMPL_ORDEREDLIST_AGENTXPDU
	AGENTXPP_DECL_TEMPL template class AGENTXPP_DECL OrderedList<AgentXPdu>;
#endif
#if !defined (AGENTXPP_DECL_TEMPL_ORDEREDLISTCURSOR_AGENTXPDU)
#define AGENTXPP_DECL_TEMPL_ORDEREDLISTCURSOR_AGENTXPDU
	AGENTXPP_DECL_TEMPL template class AGENTXPP_DECL OrderedListCursor<AgentXPdu>;
#endif

class AGENTXPP_DECL AgentXQueue: public Synchronized {

 public:

	/**
	 * Construct an empty queue
	 */
	AgentXQueue();

	/**
	 * Destructor
	 */
	virtual ~AgentXQueue();

	/**
	 * Add a request to the queue and set its packet id to a unique
	 * identifier. (SYNCHRONIZED)
	 *
	 * @param request
	 *    a new request
	 * @return
	 *    the (new) unique packet id of request.
	 */
	virtual u_int	add(AgentXPdu*);

	/**
	 * Timeout old requests and return the time until next request 
	 * will timeout. (SYNCHRONIZED)
	 *
	 * @param timeout
	 *    a time value. All requests with a timeout value less than
	 *    timeout will be deleted from the queue.
	 * @return
	 *    the time until the next request will timeout.
	 */
	virtual time_t  timeout(time_t);

	/**
	 * Get the first requests that is timed out in respect of 
	 * the given time value.
	 *
	 * @param timeout
	 *    a time value. The first request with a timestamp value less 
	 *    than timeout will be returned.
	 * @return
	 *    a pointer to an AgentXPdu.
	 */
	virtual AgentXPdu* get_late(time_t);

	/**
	 * Remove a request from the queue. (NOT SYNCHRONIZED)
	 *
	 * @param request
	 *    a pointer to the request to be removed. The memory 
	 *    allocated by request will be freed, if the request could 
	 *    be removed successfully.
	 * @return 
	 *    TRUE if the request could be removed successfully.
	 */
	virtual bool remove(AgentXPdu*);

	/**
	 * Cancel (remove from queue) all requests with a given
	 * transaction id. (SYNCHRONIZED)
	 *
	 * @param transactionID
	 *    a transaction ID.
	 */
	virtual void	cancel(u_int);

	/**
	 * Check whether there are any pending requests for a given
	 * transaction id.
	 *
	 * @param transactionID
	 *    a transaction ID.
	 * @return
	 *    TRUE if there is any request for the specified transaction
	 *    id within the queue, FALSE otherwise.
	 */
	virtual bool	pending(u_int);

	/**
	 * Get the request for a given session id and request id.
	 * (NOT SYNCHRONIZED)
	 *
	 * @param session
	 *    a AgentX session
	 * @param transactionID
	 *    the transaction id of a request
	 * @param pending
	 *    if TRUE, only pending (not yet sent) requests are returned,
	 *    otherwise any request that match the given session and
	 *    transaction ID will be returned.
	 * @return 
	 *    a pointer to an AgentXPdu if such a request could be found
	 *    or 0 otherwise. 
	 */
	virtual AgentXPdu* find(u_int, unsigned long, bool pending=FALSE);

	/**
	 * Get the request for the given packet id.
	 * (NOT SYNCHRONIZED)
	 *
	 * @param packetID
	 *    the packet id of a request
	 * @return 
	 *    a pointer an request if such a request could be found
	 *    or 0 otherwise. 
	 */
	virtual AgentXPdu* find(u_int);

	/**
	 * Return a ordered list cursor over the elements of the queue
	 * (NOT SYNCHRONIZED)
	 *
	 * @return
	 *    an OrderedListCursor instance.
	 */
	OrderedListCursor<AgentXPdu> elements();

	/**
	 * Create a new packet id.
	 *
	 * @return
	 *    a packet id.
	 */
	static u_int	create_packet_id();

 protected:
	static u_int	pid;


	OrderedList<AgentXPdu>	queue;
};
#ifdef AGENTPP_NAMESPACE
}
#endif


#endif
