/*_############################################################################
  _## 
  _##  agentx.cpp  
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


#include <agentx_pp/agentx_def.h>

#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#include <io.h>
#include <winsock.h>
#include <time.h>
#include <sys/timeb.h>
#else
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <unistd.h>
#endif

#include <errno.h>
#include <string.h>
#include <signal.h>

#include <agentx_pp/agentx.h> 
#include <agentx_pp/agentx_mib.h>
#include <agentx_pp/agentpp_agentx_mib.h>
#include <agent_pp/system_group.h>
#include <agent_pp/snmp_target_mib.h>
#include <agent_pp/snmp_community_mib.h>

#include <snmp_pp/log.h>

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

static const char *loggerModuleName = "agentx++.agentx";


#ifdef WIN32
static void winSockInit()
{
  WSADATA WSAData;
  (void)WSAStartup(0x0101, &WSAData);
}
#endif


AgentX::AgentX()
{
	connectMode = 0;
#ifdef WIN32
	// initialize Winsock 
	winSockInit();
#endif
#ifdef AX_UNIX_SOCKET
	axUnixSocketLocation = 0;
	connectMode |= AX_USE_UNIX_SOCKET;
#endif
	stopit = FALSE;
#ifdef AX_TCP_SOCKET
	axTCPSocket = 0;
	axTCPPort = 0;
	connectMode |= AX_USE_TCP_SOCKET;
#endif
}

AgentX::~AgentX() 
{
}
	
#ifdef AX_UNIX_SOCKET
void AgentX::set_unix_port_loc(const OctetStr& loc)
{
	axUnixSocketLocation = loc;
}
#endif

#ifdef AX_TCP_SOCKET
void AgentX::set_tcp_port(int p) 
{
	axTCPPort = p;
}
#endif

void AgentX::lock_socket() 
{
#ifdef LOCK_AGENTX_SOCKET
	axSocketLock.lock();
#endif
}	

void AgentX::unlock_socket()
{
#ifdef LOCK_AGENTX_SOCKET
	axSocketLock.unlock();
#endif
}

int AgentX::set_file_descriptors(fd_set* fds)
{
	int max = 0;
	FD_ZERO( fds );
#ifdef AX_UNIX_SOCKET
	if (axSocket > 0)  {
		FD_SET(axSocket, fds);
		if (axSocket > max) 
			max = axSocket;
	}
#endif
#ifdef AX_TCP_SOCKET
	if (axTCPSocket > 0) {
		FD_SET(axTCPSocket, fds);
		if (axTCPSocket > max) 
			max = axTCPSocket;
	}
#endif
	lock_peers();
	ListCursor<AgentXPeer> cur;
	for (cur.init(&axPeers); cur.get(); cur.next()) {
		if (!cur.get()->closing) {
			FD_SET(cur.get()->sd, fds);
			if (cur.get()->sd > max)
				max = cur.get()->sd;
		}
	}
	unlock_peers();
	return max+1;
}
	

bool AgentX::check_peer_closed(fd_set* fds)
{
	bool found = FALSE;
	ListCursor<AgentXPeer> cur;
	lock_peers();
	for (cur.init(&axPeers); cur.get(); ) {
		if (FD_ISSET(cur.get()->sd, fds)) {
			AgentXPeer* victim = cur.get();
			cur.next();
			delete axPeers.remove(victim);
			found = TRUE;
		}
		else cur.next();
	}
	unlock_peers();
	return found;
}
	
	
int AgentX::send_agentx(int sd, const AgentXPdu& pdu) 
{
	if (stopit) return AGENTX_NOT_OPEN;
	
	AgentXOctets octets;
	int status = pdu.encode(octets);
	if (status != AGENTX_SUCCESS) {
		return status;
	}
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
	LOG("AgentX: sending agentx pdu (sd)(type)(sid)(tid)(pid)(err)(errind)");
	LOG(sd);
	LOG(pdu.get_agentx_type());
	LOG(pdu.get_session_id());
	LOG(pdu.get_transaction_id());
	LOG(pdu.get_packet_id());
	LOG(pdu.get_error_status());
	LOG(pdu.get_error_index());
	LOG_END;
	lock_socket();
#ifdef WIN32
	int err = send(sd,(const char*)octets.data(), octets.len(), 0);
	unlock_socket();
	if (err == SOCKET_ERROR) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("AgentX: send failed due to socket (error)");
		LOG(WSAGetLastError());
		LOG_END;
		return AGENTX_BADF;
	}
#else
	int size = write(sd, (const void *)octets.data(), octets.len());
	unlock_socket();
	if (size == 0) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("AgentX: send failed due to socket EOF");
		LOG_END;
		return AGENTX_EOF;
	}
	else if (size < 0) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("AgentX: send failed due to (errno)");
		LOG(errno);
		LOG_END;
		return AGENTX_BADF;
	}
#endif
        return AGENTX_SUCCESS;
}

int AgentX::receive_agentx(int sd, AgentXPdu& pdu) 
{

	u_char		buffer[AGENTX_HEADER_LEN+1];
	u_int		payloadLen;
	bool		netByteOrder;
	int		status;

	//  read header
	unsigned int bytesRead = 0;
	while (bytesRead < AGENTX_HEADER_LEN) {
#ifdef WIN32
	    if ((status = recv(sd, (char*)(buffer+bytesRead), 
			   AGENTX_HEADER_LEN, 0)) <= 0) {

		if (status == 0) return AGENTX_DISCONNECT;
		if (status == SOCKET_ERROR) {
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			LOG("AgentX: receive socket error (errno)");
			LOG(WSAGetLastError());
			LOG_END;
			return AGENTX_DISCONNECT;
		}
		else {
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			LOG("AgentX: receive unknown error (status)");
			LOG(status);
			LOG_END;
			return AGENTX_DISCONNECT;
		}
	    }
#else
	    if ((status = read(sd, (void *)(buffer+bytesRead), 
			       AGENTX_HEADER_LEN)) <= 0) {

		if (status == 0) return AGENTX_DISCONNECT;
		if (errno == EBADF) return AGENTX_BADF;
		else {
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			LOG("AgentX: receive unknown error (errno)");
			LOG(errno);
			LOG_END;
			return AGENTX_ERROR;
		}
	    }
#endif
	    bytesRead += status;
	}
	if (buffer[0] != 1) return AGENTX_BADVER;

	netByteOrder = (buffer[2] & AGENTX_NETWORK_BYTE_ORDER ) ? TRUE : FALSE;
	AgentXOctets octets;
	octets.append(buffer, AGENTX_HEADER_LEN);
	u_int pos = 16;
	payloadLen   = octets.decode_int(pos, netByteOrder);
	
	u_char* pbuf = new u_char[payloadLen+1];
	
	//  read payload
	if (payloadLen > 0) {
		bytesRead = 0; 
		while (bytesRead < payloadLen) {
#ifdef WIN32
			if ((status = recv(sd, (char *)(pbuf+bytesRead), 
					   payloadLen-bytesRead, 0)) <= 0) {
				if (status == SOCKET_ERROR) {
#else
			if ((status = read(sd, (void *)(pbuf+bytesRead), 
					   payloadLen-bytesRead)) <= 0) {
				if (status == 0) {
#endif
				    delete[] pbuf;
				    return AGENTX_DISCONNECT;
				}
#ifndef WIN32			
				switch (errno) {
				    case EBADF: {
					delete[] pbuf;
					return AGENTX_BADF;
				    }
				    case EINTR: {
					continue;
				    }
				    default: {
#endif
					LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
					LOG("AgentX: receive payload: unknown error (errno)");
#ifdef WIN32
					LOG(WSAGetLastError());
#else
					LOG(errno);
#endif
					LOG_END;
					delete[] pbuf;
					return AGENTX_DISCONNECT;
#ifndef WIN32
				    }
				}
#endif
			}
			else {
				bytesRead += status;
			}
	  }
	}
	//  decode
	octets.append(pbuf, payloadLen);
	delete[] pbuf;
	return pdu.decode(octets);
}


time_t AgentX::compute_timeout(u_int sessionTimeout, u_int regionTimeout)
{
	time_t ct;
	time_t timeout;
	time(&ct);
	if (regionTimeout != 0 ) {
		timeout = regionTimeout + ct;
	}
	else if (sessionTimeout != 0 ) {
		timeout = sessionTimeout + ct;
	}
	else {
		timeout = AGENTX_DEFAULT_TIMEOUT + ct;
	}
	return timeout;
}

void AgentX::ping_session(const AgentXSession& session, u_int tid)
{
	AgentXPdu* pdu = new AgentXPdu(session.get_byte_order(), 
				       AGENTX_PING_PDU);
	pdu->set_packet_id(axQueue.create_packet_id());
	pdu->set_flags(0x00);
	pdu->set_session_id(session.get_id());
	pdu->set_transaction_id(tid);
	axQueue.add(pdu);

	send_agentx(session.get_peer().sd, *pdu);	
}


void AgentX::close_session(const AgentXSession& session, u_int tid, u_char r)
{
	AgentXPdu pdu(session.get_byte_order(), AGENTX_CLOSE_PDU);
	pdu.set_packet_id(axQueue.create_packet_id());
	pdu.set_flags(0x00);
	pdu.set_session_id(session.get_id());
	pdu.set_transaction_id(tid);
	pdu.set_reason(r);

	send_agentx(session.get_peer().sd, pdu);
}

#ifdef AX_UNIX_SOCKET
int AgentX::get_unix_socket() 
{
	return axSocket;
}
#endif

#ifdef AX_TCP_SOCKET
int AgentX::get_tcp_socket() 
{
	return axTCPSocket;
}
#endif

ListCursor<AgentXPeer> AgentX::peers() 
{
	ListCursor<AgentXPeer> cur;
	cur.init(&axPeers);
	return cur;
}

AgentXPeer* AgentX::remove_peer(const AgentXPeer& example)
{
	axPeersLock.lock();
	AgentXPeer* victim = 0;
	ListCursor<AgentXPeer> cur;
	for (cur.init(&axPeers); cur.get(); cur.next()) {
		if (*cur.get() == example) {
			victim = axPeers.remove(cur.get());
			break;
		}
	}
	axPeersLock.unlock();
	return victim;
}

AgentXMaster::AgentXMaster(): AgentX() 
{
	timeout = AGENTX_DEFAULT_TIMEOUT;
}

AgentXMaster::~AgentXMaster() 
{
	axSocketLock.lock();
#ifdef AX_UNIX_SOCKET
	if (axSocket > 0) {
		close(axSocket);
		OctetStr masterLoc;
		if (axUnixSocketLocation.len()>0) {
			masterLoc = axUnixSocketLocation;
			masterLoc += "master";
		}
		else {
			masterLoc = "/var/agentx/master";
		}
		unlink(masterLoc.get_printable());
	}
#endif
#ifdef AX_TCP_SOCKET
#ifdef WIN32
	if (axTCPSocket > 0) closesocket(axTCPSocket);
#else
	if (axTCPSocket > 0) close(axTCPSocket);
#endif
#endif
	axSocketLock.unlock();
}

#ifdef AX_UNIX_SOCKET

bool AgentXMaster::bind_unix()
{
	int                sd;
	struct sockaddr_un addr;
	
	lock_socket();

	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	if (axUnixSocketLocation.len()>0) {
		OctetStr masterLoc(axUnixSocketLocation);
#ifdef WIN32
		if (_access(masterLoc.get_printable(), 0) == -1) {
			if (_mkdir(masterLoc.get_printable()) == -1) {
#else
		if (access(masterLoc.get_printable(), X_OK) == -1) {
			if (mkdir(masterLoc.get_printable(), 
				  S_IRWXU | S_IRWXG) == -1) {
#endif
				LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
				LOG("AgentX: could not create dir (location)");
				LOG(masterLoc.get_printable());
				LOG_END;
#ifdef _NO_LOGGING
				raise(SIGTERM);
#endif
			} 
		}
		masterLoc += "master";
		strncpy(addr.sun_path, 
			masterLoc.get_printable(), 
			masterLoc.len());
		// null terminate
		addr.sun_path[masterLoc.len()] = 0;
	}
	else {
		strcpy(addr.sun_path, "/var/agentx/master");
	}
	
	if ((sd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {

		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("AgentX: could not open unix domain socket (location)");
		LOG(addr.sun_path);
		LOG_END;

		unlock_socket();
		return FALSE;
	}

	unlink(addr.sun_path);

	if (bind(sd, (struct sockaddr *)&addr, 
		 sizeof(struct sockaddr_un)) != 0) {

		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("AgentX: could not bind unix domain socket (location)(errno)");
		LOG(addr.sun_path);
		LOG(errno);
		LOG_END;
		
		close(sd);
		unlock_socket();
		return FALSE;
	}

	if (listen(sd, 5) != 0) {

		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("AgentX: could not listen on unix domain socket (loc)");
		LOG(addr.sun_path);
		LOG_END;
		
		close(sd);
		unlock_socket();
		return FALSE;
	}

	LOG_BEGIN(loggerModuleName, INFO_LOG | 1);
	LOG("AgentX: listening for AgentX requests on UNIX (port)");
	LOG(addr.sun_path);
	LOG_END;

	axSocket = sd;
	unlock_socket();
	return TRUE;
}
#endif

#ifdef AX_TCP_SOCKET
bool AgentXMaster::bind_tcp()
{
	struct sockaddr_in addr;
	struct servent     *svc;

	addr.sin_family         = AF_INET;
	addr.sin_addr.s_addr    = INADDR_ANY;

	if (axTCPPort == 0) {
		if ((svc = getservbyname("agentx", "tcp")) != NULL)
			axTCPPort = svc->s_port;
		else
			axTCPPort = htons(AGENTX_SVC_PORT);
	} 
	else
		axTCPPort = htons(axTCPPort);

	addr.sin_port = axTCPPort;

#ifndef WIN32
#ifndef __CYGWIN32__
	if ((ntohs(axTCPPort) < 1024) && (geteuid() != 0)) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
		LOG("AgentXMaster: bind_tcp: must be started as root");
		LOG_END;
#ifdef _NO_LOGGING
		raise(SIGTERM);
#endif
		return FALSE;
	}
#endif  // __CYGWIN32__
#endif  // WIN32
	if ((axTCPSocket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
		LOG("AgentXMaster: bind_tcp: socket error (errno)");
		LOG(errno);
		LOG_END;
#ifdef _NO_LOGGING
		raise(SIGTERM);
#endif
		return FALSE;
	}

	int i = 1;
	if (setsockopt(axTCPSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&i,
		       sizeof(i)) < 0 ) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("AgentXMaster: bind_tcp: setsockopt (errno)");
		LOG(errno);
		LOG_END;
#ifdef WIN32
		closesocket(axTCPSocket);
#else
		close(axTCPSocket);
#endif
		return FALSE;
	}

	if (bind(axTCPSocket, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
#ifdef WIN32
		closesocket(axTCPSocket);
#else
		close(axTCPSocket);
#endif
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
		LOG("AgentXMaster: bind_tcp: bind (errno)");
		LOG(errno);
		LOG_END;
#ifdef _NO_LOGGING
		raise(SIGTERM);
#endif
		return FALSE;
	}

	if (listen(axTCPSocket, 5) != 0) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("AgentXMaster: bind_tcp: listen (errno)");
		LOG(errno);
		LOG_END;
#ifdef WIN32
		closesocket(axTCPSocket);
#else
		close(axTCPSocket);
#endif
		return FALSE;
	}

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
	LOG("AgentXMaster: listening on TCP (socket)(port)");
	LOG(axTCPSocket);
	LOG(ntohs(axTCPPort));
	LOG_END;

	return TRUE;	
}

#endif

bool AgentXMaster::validate_address(const OctetStr& fromAddress) const
{
#ifdef _SNMPv3
	if (!agentppAgentXExtSrcAddrEntry::instance)
		return TRUE;
	List<MibTableRow>* tags = agentppAgentXExtSrcAddrEntry::instance->
	    get_rows_cloned(TRUE);
	ListCursor<MibTableRow> cur;
	for (cur.init(tags); cur.get(); cur.next()) {
	    agentppAgentXExtSrcAddrTag* l = 
		(agentppAgentXExtSrcAddrTag*)cur.get()->get_nth(0);
	    OctetStr tag(l->get_state());

	    if ((snmpTargetAddrEntry::instance) &&
		(snmpTargetAddrExtEntry::instance)) {
		if (snmpTargetAddrExtEntry::instance->
		    passes_filter(fromAddress, tag)) {
		    delete tags;
		    return TRUE;
		}
	    }
	}
	delete tags;
	LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
	LOG("AgentXMaster: unauthorized connection request (from): ");
	LOG(fromAddress.get_printable());
	LOG_END;
	return FALSE;
#else
	return TRUE;
#endif
}

void AgentXMaster::connect_request(fd_set *fds, int* nfds)
{
	int    sd;
	time_t t;
#ifdef SNMP_PP_IPv6
	struct sockaddr_storage tcpAddr;
#else
	struct sockaddr_in tcpAddr;
#endif // SNMP_PP_IPv6
#if !defined __MINGW32__ && ( defined __GNUC__ || defined __FreeBSD__ || defined _AIX )
	socklen_t fromlen;
#else
	int fromlen;
#endif
	fromlen = sizeof(tcpAddr);


	LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
	LOG("AgentXMaster: accepting new connections");
	LOG_END;

#ifdef AX_TCP_SOCKET	
	if ((axTCPSocket > 0) && FD_ISSET(axTCPSocket, fds)) {
	    LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
	    LOG("AgentXMaster: new subagent connection on TCP port");
	    LOG_END;
	    sd = accept(axTCPSocket, (struct sockaddr *)&tcpAddr, &fromlen);
	    if (sd == -1) {
		LOG_BEGIN(loggerModuleName, INFO_LOG | 1);
		LOG("AgentXMaster: connection not accepted (error)");
		LOG(errno);
		LOG_END;
	    } 
	    else {
		time(&t);
		u_int ind = 0;
		UdpAddress fromAddress;
		bool addressOK = TRUE;
		Oidx addressType;
		if (((sockaddr_in&)tcpAddr).sin_family == AF_INET) {
		    // IPv4
		    fromAddress = 
			inet_ntoa(((sockaddr_in&)tcpAddr).sin_addr);
		    fromAddress.set_port(
			ntohs(((sockaddr_in&)tcpAddr).sin_port));
		    addressType = TRANSPORT_DOMAIN_TCP_IPv4;
		}
#ifdef SNMP_PP_IPv6
		else if (tcpAddr.ss_family == AF_INET6)
		{
		    // IPv6
		    char tmp_buffer[INET6_ADDRSTRLEN+1];
		    
		    inet_ntop(AF_INET6, 
			      &(((sockaddr_in6&)tcpAddr).sin6_addr),
			      tmp_buffer, INET6_ADDRSTRLEN);
		    
		    fromAddress = tmp_buffer;
		    fromAddress.set_port(
			ntohs(((sockaddr_in6&)tcpAddr).sin6_port));

		    addressType = TRANSPORT_DOMAIN_TCP_IPv6;
		}
#endif // SNMP_PP_IPv6
		else
		{
		    LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		    LOG("AgentXMaster: Unkown socket address family (id)");
		    LOG(((sockaddr_in&)tcpAddr).sin_family);
		    LOG_END;
		    addressOK = FALSE;
		}
		OctetStr taddress;
		for (int i=0; i<fromAddress.get_length(); i++) {
		    taddress += (unsigned char)fromAddress[i];
		}
		if ((addressOK) && 
		    (agentppAgentXExtSrcAddrValidation::instance) &&
		    (agentppAgentXExtSrcAddrValidation::instance->
		     get_state() == 1)) {
		    addressOK = validate_address(taddress);
		}
		if (addressOK) {	    
		    if (agentxConnectionEntry::instance) {
			ind = agentxConnectionEntry::instance->
			    add(addressType, taddress);
		    }
		    lock_peers();
		    axPeers.add(new AgentXPeer(sd, t, timeout, ind));
		    unlock_peers();
		    LOG_BEGIN(loggerModuleName, EVENT_LOG | 4);
		    LOG("AgentXMaster: new peer added (sd)(name)(connecttime)(timeout)");
		    LOG(sd);
		    LOG((long)t);
		    LOG((int)timeout);
		    LOG_END;
		}
		else {
#ifdef WIN32
		    _close(sd);
#else
		    close(sd);
#endif
		}
	    }
	    (*nfds)--;
	}
#endif

#ifdef AX_UNIX_SOCKET	
	if ((axSocket > 0) && (FD_ISSET(axSocket, fds))) {
		LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
		LOG("AgentXMaster: new subagent connection on UNIX port");
		LOG_END;
		
		sd = accept_unix();
		if (sd > 0) {
			time(&t);

			u_int ind = 0;
			if (agentxConnectionEntry::instance) {
			  OctetStr* n = 0;
			  struct sockaddr_un addr;
#ifdef __hpux
			  int len = sizeof(struct sockaddr_un);
#else
			  socklen_t len = sizeof(struct sockaddr_un);
#endif
			  if (getsockname(sd, (struct sockaddr*)&addr, 
					  &len) == 0) {
			    n = new OctetStr((unsigned char*)addr.sun_path,
					     strlen(addr.sun_path));
			  }
			  else n = new OctetStr();
			  ind = agentxConnectionEntry::instance->
				  add(TRANSPORT_DOMAIN_LOCAL, *n);
			  delete n;
			}
			lock_peers();
			axPeers.add(new AgentXPeer(sd, t, timeout, ind));
			unlock_peers();
			LOG_BEGIN(loggerModuleName, EVENT_LOG | 4);
			LOG("AgentXMaster: new peer added (sd)(name)(connecttime)(timeout)");
			LOG(sd);
			LOG((long)t);
			LOG((int)timeout);
			LOG_END;
		}
		(*nfds)--;
	}
#endif
}

#ifdef AX_UNIX_SOCKET
int AgentXMaster::accept_unix() 
{
#ifdef __hpux
	int		   len;
#else
	socklen_t          len;
#endif
	int                sd;
	struct sockaddr_un addr;
	struct stat        statbuf;
	time_t             staletime;

	len = sizeof(struct sockaddr_un);
	sd = accept(axSocket, (struct sockaddr *)&addr, &len);
	if (sd < 0) {
		LOG_BEGIN(loggerModuleName, INFO_LOG | 1);
		LOG("AgentXMaster: connection not accepted (error)");
		LOG(sd);
		LOG_END;
		return sd;
	}

	len -= sizeof(addr.sun_family);
	addr.sun_path[len] = 0x00;
  
	if (stat(addr.sun_path, &statbuf) < 0) {
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
		LOG("AgentXMaster: unable to stat unix domain (socket)");
		LOG(OctetStr((unsigned char*)addr.sun_path, 
			     len).get_printable());
		LOG_END;
	}
	else {

		if ((statbuf.st_mode & (S_IRWXG | S_IRWXO)) ||
		    (statbuf.st_mode & S_IRWXU) != S_IRWXU ) {
			 LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			 LOG("AgentXMaster: bad permissions on unix socket");
			 LOG_END;
			 return AGENTX_ERROR;
		}

		staletime = time(NULL)-AGENTX_UNIX_SOCKET_STALE;
		if ((statbuf.st_atime < staletime) ||
		    (statbuf.st_ctime < staletime) ||
		    (statbuf.st_mtime < staletime)) {
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			LOG("AgentXMaster: stale unix socket");
			LOG_END;
			return AGENTX_ERROR;
		}
	}
	unlink(addr.sun_path);
	
	return sd;
}
#endif

bool AgentXMaster::is_response(const AgentXPdu& request, 
				  const AgentXPdu& response)
{
  return ((request.get_packet_id() == response.get_packet_id()) &&
	  (request.get_session_id() == response.get_session_id()) &&
	  (request.get_transaction_id() == response.get_transaction_id()) &&
	  (request.get_agentx_type() != AGENTX_RESPONSE_PDU) &&
	  (response.get_agentx_type() == AGENTX_RESPONSE_PDU));
}


AgentXSlave::AgentXSlave() 
{
#ifdef AX_TCP_SOCKET
	masterAddress = "127.0.0.1/0";
#endif
	connectionType = UNCONNECTED;
	axSlaveSocket = 0;
}

AgentXSlave::~AgentXSlave()
{
	axSocketLock.lock();
#ifdef WIN32
	if (axSlaveSocket > 0) closesocket(axSlaveSocket);
#else
	if (axSlaveSocket > 0) close(axSlaveSocket);
#endif
#ifdef AX_UNIX_SOCKET
	if (connectionType == UNIX) {
		struct sockaddr_un addr;
		memset(&addr, 0, sizeof(addr));
		sprintf(addr.sun_path, "%ssubagent%05d", 
			axUnixSocketLocation.get_printable(), (int)getpid());
		unlink(addr.sun_path);	
	} 
#endif
	axSocketLock.unlock();
}


#ifdef AX_UNIX_SOCKET
bool AgentXSlave::bind_unix()
{
	struct sockaddr_un addr;
	int                sd;
	int                len;

	lock_socket();
	
	sd = socket(PF_UNIX, SOCK_STREAM, 0);

	if (sd == -1) {

		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("AgentX: could not open unix domain socket");
		LOG_END;
		unlock_socket();
		return FALSE;
	}
	
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	sprintf(addr.sun_path, "%ssubagent%05d", 
		axUnixSocketLocation.get_printable(), (int)getpid());
	len = sizeof(addr.sun_family) + strlen(addr.sun_path) + 1;
	
	unlink(addr.sun_path);
	
	if (bind(sd, (struct sockaddr *)&addr, len) < 0) {

		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("AgentX: could not bind unix domain socket (location)");
		LOG(addr.sun_path);
		LOG_END;
		
		close(sd);
		unlock_socket();
		return FALSE;
	}

	LOG_BEGIN(loggerModuleName, INFO_LOG | 1);
	LOG("SubAgentX: bound local (socket)(sd)");
	LOG(addr.sun_path);
	LOG(sd);
	LOG_END;

	if (chmod(addr.sun_path, AGENTX_USOCK_PERM) < 0) {
	  
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("AgentX: could not chmod on unix domain socket (loc)");
		LOG(addr.sun_path);
		LOG_END;
		
		close(sd);
		unlock_socket();
		return FALSE;
	}
  
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	sprintf(addr.sun_path, "%smaster", 
		axUnixSocketLocation.get_printable() );
	len = sizeof(addr.sun_family) + strlen(addr.sun_path) + 1;
	
	if (connect(sd, (struct sockaddr *)&addr, len) == -1) {
	  
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("AgentX: could not connect unix domain socket (location)");
		LOG(addr.sun_path);
		LOG_END;
		close(sd);
		unlock_socket();
		return FALSE;
	}
	
	LOG_BEGIN(loggerModuleName, INFO_LOG | 1);
	LOG("SubAgentX: connected local socket with master (sd)(socket)");
	LOG(sd);
	LOG(addr.sun_path);
	LOG_END;

	axSocket  = sd;
	axSlaveSocket = axSocket;
	connectionType = UNIX;
	
	unlock_socket();
	return TRUE;
}
#endif

#ifdef AX_TCP_SOCKET
bool AgentXSlave::bind_tcp()
{
#ifdef SNMP_PP_IPv6
	struct sockaddr_storage addr;
#else
	struct sockaddr_in	addr;
#endif
	struct servent		*svc;	

	// UX only supports UDP type addresses (addr and port) right now
	if (masterAddress.get_type() != Address::type_udp)
	    return FALSE;// unsupported address type

	axTCPPort = htons(((UdpAddress &)masterAddress).get_port());
	if (((UdpAddress &)masterAddress).get_port() == 0) {
		if ((svc = getservbyname("agentx", "tcp")) != NULL)
			axTCPPort = svc->s_port;
		else
			axTCPPort = htons(AGENTX_SVC_PORT);
	}

	if (((UdpAddress &)masterAddress).get_ip_version() == 
	    Address::version_ipv4) {
	    // prepare the destination address
	    memset(&addr, 0, sizeof(addr));
	    ((sockaddr_in&)addr).sin_family = AF_INET;
	    ((sockaddr_in&)addr).sin_addr.s_addr
	       = inet_addr(((IpAddress &)
			    masterAddress).IpAddress::get_printable());
	    ((sockaddr_in&)addr).sin_port = axTCPPort;
	}
	else
	{
#ifdef SNMP_PP_IPv6
		memset(&addr, 0, sizeof(addr));
		inet_pton(AF_INET6, ((IpAddress &)
				     masterAddress).IpAddress::get_printable(),
			  &((sockaddr_in6&)addr).sin6_addr);
		((sockaddr_in6&)addr).sin6_family = AF_INET6;
		((sockaddr_in6&)addr).sin6_port = axTCPPort;
#else		
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("AgentX: could not open TCP socket: unsupported address type");
		LOG_END;
		return FALSE;
#endif
	}

	if ((axTCPSocket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {

		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("AgentX: could not open TCP socket (port)(errno)");
		LOG(ntohs(axTCPPort));
#ifdef WIN32
		LOG(WSAGetLastError());
#else
		LOG(errno);
#endif
		LOG_END;
		return FALSE;
	}
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
	LOG("AgentXSlave: listening on TCP (socket)(port)");
	LOG(axTCPSocket);
	LOG(ntohs(axTCPPort));
	LOG_END;	
	if (connect(axTCPSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
#ifdef WIN32
		closesocket(axTCPSocket);
#else
		close(axTCPSocket);
#endif
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("AgentX: could not connect TCP socket (port)(errno)");
		LOG(ntohs(axTCPPort));
		LOG(errno);
		LOG_END;	  
		return FALSE;
	}
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
	LOG("AgentXSlave: connnected TCP (socket)(port)");
	LOG(axTCPSocket);
	LOG(ntohs(axTCPPort));
	LOG_END;	

	axSlaveSocket = axTCPSocket;
	connectionType = TCP;

	return TRUE;
}
#endif

AgentXPdu* AgentXSlave::receive(const u_int timeout, int& status) 
{
	fd_set fdSet;
	int sd = 0;

	FD_ZERO(&fdSet);
	FD_SET(axSlaveSocket, &fdSet);
	sd = axSlaveSocket;

	struct timeval tv;

	tv.tv_sec  = timeout/1000;    // wait up to sec seconds
	tv.tv_usec = (timeout%1000)*1000;

	int sel;
	if ((sel = select(FD_SETSIZE, &fdSet, NULL, NULL, &tv)) < 0) {
		if (errno == EINTR) {
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 5);
			LOG("AgentXSlave: select interrupted");
			LOG_END;
			return 0; 
		}
		else
		{
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			LOG("AgentXSlave: lost connection with master");
			LOG_END;
			stopit = TRUE;
			return 0; 
		}
	}
	else if (sel == 0)  {
		LOG_BEGIN(loggerModuleName, INFO_LOG | 5);
		LOG("AgentXSlave: no request within (milli seconds)");
		LOG(timeout);
		LOG_END;
		return 0;
	} 
	else {
		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 4);
		LOG("AgentXSlave: received something on ports");
		LOG_END;

		//  read a pdu
      		lock_socket();
		AgentXPdu* pdu = new AgentXPdu();
		if ((status = receive_agentx(sd, *pdu)) != AGENTX_OK) {
			if (status == AGENTX_DISCONNECT ) {
			    if (axSlaveSocket > 0) { 
#ifdef WIN32
				closesocket(axSlaveSocket);
#else
				close(axSlaveSocket);
#endif
				axSlaveSocket = 0;
			    }
			    unlock_socket();
			    LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			    LOG("AgentXSlave: lost connection with master");
			    LOG_END;
			    stopit = TRUE;
			    delete pdu;
			    return 0;
			}
		}
      		unlock_socket();
		return pdu;
	}
}

bool AgentXSlave::is_response(const AgentXPdu& request, 
				 const AgentXPdu& response)
{
  return ((request.get_packet_id() == response.get_packet_id()) &&
	  (request.get_agentx_type() != AGENTX_RESPONSE_PDU) &&
	  (response.get_agentx_type() == AGENTX_RESPONSE_PDU));
}

int AgentXSlave::send(const AgentXPdu& pdu) 
{
	return send_agentx(axSlaveSocket, pdu);
}

int AgentXSlave::send_synch(AgentXPdu& pdu, AgentXPdu& response)
{
	pdu.set_packet_id(axQueue.create_packet_id());

	int status = send_agentx(axSlaveSocket, pdu);

	if (status != AGENTX_OK) return status;
	// wait for response
	AgentXPdu* resp = 0;
#ifdef WIN32
	struct _timeb tstruct;
	_ftime(&tstruct);
	time_t stop = tstruct.time*1000 + tstruct.millitm;
        time_t current = stop;
#else
#ifdef HAVE_CLOCK_GETTIME
        struct timespec tsp;
        clock_gettime(CLOCK_MONOTONIC, &tsp);
        unsigned long stop = (tsp.tv_sec*1000)+(tsp.tv_nsec/1000000);
#else
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	unsigned long stop    = (tv.tv_sec*1000)+(tv.tv_usec/1000);
#endif        
        unsigned long current = stop;
#endif
	stop += pdu.get_timeout()*1000;
	do {
		if (resp) { 
			delete resp; 
			resp = 0; 
		}
		resp = receive(pdu.get_timeout()*1000, status);
#ifdef WIN32
		_ftime(&tstruct);
		current = tstruct.time*1000 + tstruct.millitm;
#else
#ifdef HAVE_CLOCK_GETTIME
                clock_gettime(CLOCK_MONOTONIC, &tsp);
                current = (tsp.tv_sec*1000)+(tsp.tv_nsec/1000000);
#else
		gettimeofday(&tv, &tz);
		current = (tv.tv_sec*1000)+(tv.tv_usec/1000);
#endif                
#endif
		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 5);
		LOG("AgentXSlave: received (sd)(current)(stop)(status)");
		LOG(axSlaveSocket);
		LOG(current);
		LOG(stop);
		LOG(status);
		LOG_END;

	} while ((current<stop) &&
		 ((!resp) || (!is_response(pdu, *resp))));

	if (status != AGENTX_OK) {
		delete resp;
		return status;
	}
	if ((!resp) || (!is_response(pdu, *resp))) {
		delete resp;		
		return AGENTX_TIMEOUT;
	}
	response = *resp;
	delete resp;
	return status;
}

int AgentXSlave::open_session(AgentXSession& session) 
{
	AgentXPdu*	pdu;

	pdu = new AgentXPdu(session.get_byte_order(), AGENTX_OPEN_PDU);
	pdu->set_flags(0x00);
	pdu->set_session_id(session.get_id());
	pdu->set_transaction_id(0);

	pdu->set_timeout(session.get_timeout());
	Vbx vb(session.get_oid(), session.get_descr());
	*pdu += vb;
	
	pdu->set_timestamp(compute_timeout(session.get_timeout()));
        int pid = axQueue.add(pdu);
	
	AgentXPdu response;
	int status = send_synch(*pdu, response);
	if (status == AGENTX_OK) {
		LOG_BEGIN(loggerModuleName, INFO_LOG | 1);
		LOG("AgentXSlave: opened session (sid)(pid)");
		LOG(response.get_session_id());
		LOG(pid);
		LOG_END;
		session.set_id(response.get_session_id());
	}
	return status;
}

#ifdef AGENTPP_NAMESPACE
}
#endif



