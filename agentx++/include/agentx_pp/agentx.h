/*_############################################################################
  _## 
  _##  agentx.h  
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

#ifndef _agentx_h_
#define _agentx_h_


#include <agent_pp/agent++.h>
#include <agentx_pp/agentx_def.h>
#include <agentx_pp/agentx_pdu.h>
#include <agentx_pp/agentx_queue.h>
#include <agentx_pp/agentx_peer.h>
#include <agentx_pp/agentx_session.h>
#include <agentx_pp/agentx_threads.h>


#define AGENTX_UNIX_SOCKET_STALE    30

#ifndef WIN32
#define AX_UNIX_SOCKET 
#endif
#define AX_TCP_SOCKET

// Define this if sequential access to sockets is needed
//#define LOCK_AGENTX_SOCKET


#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

/*-------------------------- class AgentX -----------------------*/

/**
 * The AgentX class sends and receives AgentX PDUs.
 *
 * @author Frank Fock
 * @version 1.4c
 */
#if !defined (AGENTXPP_DECL_TEMPL_LIST_AGENTXPEER)
#define AGENTXPP_DECL_TEMPL_LIST_AGENTXPEER
	AGENTXPP_DECL_TEMPL template class AGENTXPP_DECL List<AgentXPeer>;
#endif

class AGENTXPP_DECL AgentX {

 public:
  
	/**
	 * Create an AgentX session
	 */
	AgentX();

	/**
	 * Destructor
	 */
	virtual ~AgentX();

	/**
	 * Set connection mode. Currently UNIX domain sockets 
	 * (AX_USE_UNIX_SOCKET) and TCP sockets (AX_USE_TCP_SOCKET) 
	 * are supported. 
	 *
	 * @param mode
	 *    a possibly ored value of AX_USE_UNIX_SOCKET and
	 *    AX_USE_TCP_SOCKET. 
	 */
	void			set_connect_mode(int m) { connectMode = m; }

	/**
	 * Get the connection mode. Thus, whether to use the UNIX domain
	 * socket, the TCP socket, or both.
	 *
	 * @return
	 *    a possibly ored value of AX_USE_UNIX_SOCKET and 
	 *    AX_USE_TCP_SOCKET. 
	 */
	int			get_connect_mode() { return connectMode; }

#ifdef AX_UNIX_SOCKET
	/**
	 * Set the unix port location
	 *
	 * @param location
	 *    an AF_UNIX socket port location
	 */
	void			set_unix_port_loc(const NS_SNMP OctetStr&);
#endif

#ifdef AX_TCP_SOCKET
	/**
	 * Set TCP port.
	 *
	 * @param port
	 *    a TCP port.
	 */
	void			set_tcp_port(int);
#endif
	/**
	 * Return whether the agentX should quit
	 *
	 * @return
	 *    TRUE if the agentX should quit
	 */
	virtual bool		quit() { return stopit; }

	/**
	 * (Re)initialize the AgentX protocol stack. This resets
	 * values like stopit in order to be able to reestablish 
	 * an AgentX connection.
	 */
	virtual void		reinit() { stopit = FALSE; }
	
	
	/**
	 * Send an AgentX PDU.
	 *
	 * @param sd
	 *    a socket descriptor 
	 * @param pdu
	 *    the AgentXPdu instance to be send
	 * @return 
	 *    an error code on failure or AGENTX_CLASS_SUCCESS on 
	 *    success
	 */
	virtual int		send_agentx(int, const AgentXPdu&);

	/**
	 * Receive an AgentX PDU.
	 *
	 * @param sd
	 *    a socket descriptor 
	 * @param pdu
	 *    an empty AgentXPdu instance.
	 * @return 
	 *    an error code on failure or AGENTX_CLASS_SUCCESS on 
	 *    success
	 */
	virtual int		receive_agentx(int, AgentXPdu&);

	/**
	 * Get a cursor on the peer list.
	 *
	 * @return 
	 *    a cursor on the receiver's peer list.
	 */
	virtual ListCursor<AgentXPeer> peers();

	/**
	 * Remove a peer rom the peer list.
	 *
	 * @param victim
	 *    an example for an AgentXPeer instance that should be removed
	 *    from the peer list.
	 * @return 
	 *    a pointer to the removed peer instance or 0 if no such instance
	 *    could be found.
	 */
	virtual AgentXPeer*    	remove_peer(const AgentXPeer&);

#ifdef AX_UNIX_SOCKET
	/**
	 * Bind unix domain socket.
	 *
	 * @return
	 *    TRUE if the socket could be bound successfully.
	 */
	virtual bool		bind_unix() = 0;

	/**
	 * Get the AgentX socket
	 *
	 * @return 
	 *    a socket descriptor.
	 */
        int			get_unix_socket();
#endif

#ifdef AX_TCP_SOCKET	
	/**
	 * Bind TCP socket.
	 *
	 * @return
	 *    TRUE if the socket could be bound successfully.
	 */
	virtual bool		bind_tcp() = 0;

	/**
	 * Get the AgentX socket
	 *
	 * @return 
	 *    a socket descriptor.
	 */
        int			get_tcp_socket();

#endif	

	int			set_file_descriptors(fd_set*);
	bool			check_peer_closed(fd_set*);

	void		       	lock_socket();
	void		       	unlock_socket();
	//virtual bool		bind_tcp(int);

	void			lock_queue() { axQueue.lock(); }
	void			unlock_queue() { axQueue.unlock(); }

	void			lock_peers() { axPeersLock.lock(); }
	void			unlock_peers() { axPeersLock.unlock(); }

	AgentXQueue*		get_queue() { return &axQueue; }

	/**
	 * Check whether two PDUs are a request/response pair.
	 *
	 * @param request
	 *    the "request" PDU.
	 * @param response
	 *    the "response" PDU.
	 * @return
	 *    TRUE if response is a response to request,
	 *    FALSE otherwise.
	 */
	virtual bool	       	is_response(const AgentXPdu&,
					    const AgentXPdu&) = 0;

	/**
	 * Close a session.
	 *
	 * @param session
	 *    a AgentXSession instance.
	 * @param transactionID
	 *    the transcation id to be used.
	 * @param reason
	 *    the reason.
	 */
	void			close_session(const AgentXSession&, u_int,
					      u_char);

	/**
	 * Ping a session.
	 *
	 * @param session
	 *    a AgentXSession instance.
	 * @param transactionID
	 *    the transcation id to be used.
	 */
	void			ping_session(const AgentXSession&, u_int);

	/**
	 * Compute the timeout (timestamp when a PDU/request times out)
	 * 
	 * @param sessionTimeout
	 *    the timeout value of the session. If sessionTimeout is 0
	 *    AGENTX_DEFAULT_TIMEOUT is used to compute the timestamp.
	 * @param regionTimeout
	 *    the timeout value of the region. If regionTimeout is 0
	 *    or if it is not given, the sessionTimeout or 
	 *    AGENTX_DEFAULT_TIMEOUT value is used to compute the 
	 *    timestamp respectively.
	 * @return
	 *    a timestamp (current time + timeout)
	 */
	static time_t		compute_timeout(u_int, u_int = 0);

	bool			stopit;

 protected:

        
#ifdef AX_TCP_SOCKET
	int			axTCPSocket;
	int			axTCPPort;
#endif
#ifdef AX_UNIX_SOCKET
	int			axSocket;
	NS_SNMP OctetStr	axUnixSocketLocation;	
#endif
	AgentXQueue		axQueue;
	List<AgentXPeer>	axPeers;
	Synchronized		axPeersLock;

	Synchronized		axSocketLock;

	int			connectMode;
};



class AGENTXPP_DECL AgentXMaster: public AgentX {

 public:
	AgentXMaster();
	
	virtual ~AgentXMaster();

	/**
	 * Set the connection timeout.
	 *
	 * @param timeout
	 *    the new connection timeout.
	 */
	void			set_timeout(int t) { timeout = t; }

#ifdef AX_UNIX_SOCKET
	virtual bool		bind_unix();
#endif

#ifdef AX_TCP_SOCKET
	virtual bool		bind_tcp();	
#endif

	/**
	 * Process a subagent's connect request.
	 *
	 * @param fileDescrSet
	 *    a file descriptor set.
	 * @param 
	 *    a pointer to a pending descriptors counter.
	 */
	virtual void		connect_request(fd_set*, int*);

	/**
	 * Check whether two PDUs are a request/response pair.
	 *
	 * @param request
	 *    the "request" PDU.
	 * @param response
	 *    the "response" PDU.
	 * @return
	 *    TRUE if response is a response to request,
	 *    FALSE otherwise.
	 */
	virtual bool	       	is_response(const AgentXPdu&,
					    const AgentXPdu&);

 protected:
	
	/**
	 * Accept a new connection on the unix port and return the socket
	 * descriptor.
	 *
	 * @return
	 *    a socket descriptor on success and a value < 0 on failure.
	 */
#ifdef AX_UNIX_SOCKET 
	virtual int		accept_unix();
#endif
	int	timeout;

	/**
	 * Validate the supplied source address against the addresses
	 * in the agentppAgentXExtSrcAddrTable.
	 * @return
	 *    TRUE if the source address is valid, FALSE otherwise.
	 */
	virtual bool		validate_address(const NS_SNMP OctetStr&) const;
};



class AGENTXPP_DECL AgentXSlave: public AgentX {

 public:

	AgentXSlave();
	virtual ~AgentXSlave();

#ifdef AX_UNIX_SOCKET
	virtual bool		bind_unix();
#endif

#ifdef AX_TCP_SOCKET
	/**
	 * Set the masters IP address. Default is localhost (127.0.0.1).
	 *
	 * @param ipAddress
	 *    the IP address of an AgentX master. If the port portion
	 *    is not 0 it is used instead of the standard AgentX TCP port. 
	 */
	void			set_master_address(const NS_SNMP UdpAddress& addr)
					{ masterAddress = addr; }

	virtual bool		bind_tcp();	
#endif

	/**
	 * Wait a given time for an incoming request.
	 *
	 * @param timeout
	 *    the maximum time in milli seconds to wait for an 
	 *    incoming request.
	 * @param status
	 *    returns the current status of the connection with 
	 *    the master.
	 * @return 
	 *    a pointer to the received request, or 0, if within the 
	 *    timeout period no request has been received.
	 */
	virtual AgentXPdu*  receive(const u_int, int&);

	/**
	 * Send an AgentXPdu.
	 *
	 * @param pdu
	 *    the pdu to send.
	 * @return
	 *    an error status.
	 */
	virtual int		send(const AgentXPdu&);

	/**
	 * Send an AgentXPdu as a synchronous request.
	 *
	 * @param pdu
	 *    the pdu to send. Packet id and send time are set.
	 * @param response
	 *    the pdu to hold the received response.
	 * @return
	 *    an error status.
	 */
	virtual int		send_synch(AgentXPdu&, AgentXPdu&);

	/**
	 * Initiates (opens) a AgentX session with the master agent.
	 *
	 * @param session
	 *    an AgentXSession instance.
	 * @return
	 *    AGENTX_OK if session opened successfully, any other
	 *    AGENTX error code on failure.
	 */
	virtual int		open_session(AgentXSession&);

	/**
	 * Check whether two PDUs are a request/response pair.
	 *
	 * @param request
	 *    the "request" PDU.
	 * @param response
	 *    the "response" PDU.
	 * @return
	 *    TRUE if response is a response to request,
	 *    FALSE otherwise.
	 */
	virtual bool	       	is_response(const AgentXPdu&,
					    const AgentXPdu&);

	void		       	lock_rcv_socket();
	void		       	unlock_rcv_socket();

	/**
	 * Get the socket descriptor used to communicate with 
	 * the AgentX master agent.
	 *
	 * @return
	 *    a socket descriptor of an UNIX socket or a TCP socket.
	 */
	int			get_socket() { return axSlaveSocket; }

 protected:
	
	int			axSlaveSocket;

	typedef enum { UNCONNECTED, UNIX, TCP } ConnectionType;
	
	ConnectionType		connectionType;
	
#ifdef AX_TCP_SOCKET
	NS_SNMP UdpAddress	masterAddress;
#endif	

};

#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
