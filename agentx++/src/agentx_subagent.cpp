/*_############################################################################
  _## 
  _##  agentx_subagent.cpp  
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

#include <signal.h>
#include <agentx_pp/agentx_subagent.h>
#include <agent_pp/system_group.h>
#include <snmp_pp/log.h>

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

static const char *loggerModuleName = "agentx++.agentx_subagent";

class AgentXReconnect: public Runnable {
public:
	AgentXReconnect(SubAgentXMib* m) { mib = m; }
	~AgentXReconnect() { }
	virtual void run() {
		mib->reconnect();
	}
private:
	SubAgentXMib* mib;
};

#ifdef _THREADS
class AgentXResponse: public Runnable {
public:
	AgentXResponse(SubAgentXMib* m, AgentXRequest* r) 
		{ mib = m; response = r; }
	~AgentXResponse() { }

	virtual void run() {
		mib->process_response(response);
		// response is deleted by removing it from request list
	}
private:
	SubAgentXMib* mib;
	AgentXRequest* response;
};
#endif

/*--------------------- class AgentXSharedTable ----------------------*/

AgentXSharedTable::AgentXSharedTable(const AgentXSharedTable& other):
  MibTable(other)
{
	indexOIDs = new Oidx[index_len];
	for (u_int i=0; i<index_len; i++) {
		indexOIDs[i] = other.indexOIDs[i];	
	}
	backReference = other.backReference;
	timeout = 0;
        indexStrategy = firstSubIndexOnly;
}

AgentXSharedTable::AgentXSharedTable(const Oidx& o, const index_info* inf, 
				     unsigned int sz, const Oidx* indoids, 
				     SubAgentXMib* br, const OctetStr& c):
  MibTable(o, inf, sz)
{
	indexOIDs = new Oidx[sz];
	for (u_int i=0; i<sz; i++) {
		indexOIDs[i] = indoids[i];	
	}
	myContext = c;
	backReference = br;
	timeout = 0;
    indexStrategy = firstSubIndexOnly;
}

AgentXSharedTable::~AgentXSharedTable()
{
	delete[] indexOIDs;
}

MibTableRow* AgentXSharedTable::init_row(const Oidx& ind, Vbx* vbs)
{
	if (allocate_index(ind)) {
		return MibTable::init_row(ind, vbs);
	}
	return 0;
} 

MibTableRow* AgentXSharedTable::add_row(const Oidx& ind)
{
	if ((ind.len() == 0) || (generator.size() == 0)) return 0;

	// check whether row exists
	MibTableRow* new_row = find_index(ind);
	bool existing = (new_row != 0);
	if (!new_row) {
		new_row = new MibTableRow(generator);
		new_row->set_index(ind);
	}
	// register row
	Oidx subtree(new_row->get_nth(0)->get_oid());
	unsigned int u = (*(generator.last()->key()))[0];
	AgentXRegion r(subtree, (unsigned char)(key()->len()+1), u);
	backReference->register_region(myContext, r, timeout, TRUE, this); 
	//below event is fired when confirmation of registration is received
	//fire_row_changed(rowCreateAndGo, new_row, ind);
	if (existing)
		return new_row;
	return content.add(new_row);	
}

void AgentXSharedTable::remove_row(const Oidx& ind)
{
	Oidx o(ind);
	MibTableRow* r = content.find(&o);
	// deallocate index
	Vbx* vbs = create_index_vbs(ind);
	if (vbs) {
	  backReference->deallocate_index(myContext, vbs, index_len, this, new Oidx(ind)); 
	  delete[] vbs;
	}
	// unregister row
	Oidx subtree(r->get_nth(0)->get_oid());
	unsigned int u = r->last()->get_oid()[key()->len()];
	AgentXRegion reg(subtree, (unsigned char)(key()->len()+1), u);
	backReference->register_region(myContext, reg, 0, FALSE); 

	fire_row_changed(rowDestroy, r, ind);

	if (r) notready_rows.remove(r);
	content.remove(&o);
}

void AgentXSharedTable::get_index_value(SmiUINT32 type, 
					const Oidx& o, Vbx& vb)
{
	switch (type) { 
	case sNMP_SYNTAX_OCTETS: {
	  OctetStr s(o.as_string());
	  vb.set_value(s);
	  break;
	}
	case sNMP_SYNTAX_IPADDR: {
	  IpAddress addr(Oidx(o).get_printable());
	  vb.set_value(addr);
	  break;
	}
	case sNMP_SYNTAX_INT32: {
	  SnmpInt32 i(o[0]);
	  vb.set_value(i);
	  break;
	}
	case sNMP_SYNTAX_TIMETICKS:
	case sNMP_SYNTAX_GAUGE32: {
	  SnmpUInt32 i(o[0]);
	  vb.set_value(i);
	  break;
	}
	case sNMP_SYNTAX_OID: {
	  vb.set_value(o);
	  break;
	}
	} 
}

Oidx AgentXSharedTable::get_index_from_vbs(Vbx* vbs, u_int sz) 
{
	Oidx ind;
	for (u_int i=0; ((i<sz) && (i<index_len)); i++) {
		switch (vbs[i].get_syntax()) {
		case sNMP_SYNTAX_OID: {
		  Oidx o;
		  vbs[i].get_value(o);
		  if (!index_struc[i].implied) ind += o.len();
		  ind += o;
		  break;
		}
		case sNMP_SYNTAX_OCTETS: {
		  OctetStr s;
		  vbs[i].get_value(s);
		  ind += Oidx::from_string(s, !index_struc[i].implied);
		  break;
		}
		case sNMP_SYNTAX_IPADDR: {
		  IpAddress addr;
		  vbs[i].get_value(addr);
		  ind += addr;
		  break;
		}
		case sNMP_SYNTAX_INT32: {
		  long l = 0;
		  vbs[i].get_value(l);
		  ind += (unsigned long)l;
		  break;
		}
		case sNMP_SYNTAX_TIMETICKS:
		case sNMP_SYNTAX_GAUGE32: {
		  unsigned long l=0;
		  vbs[i].get_value(l);
		  ind += l;
		  break;
		}		  
		}
	}
	return ind;
}

Vbx* AgentXSharedTable::create_index_vbs(const Oidx& ind) 
{
	Vbx* vbs = new Vbx[index_len];
	for (unsigned int j=0; j<index_len; j++) {
		vbs[j].set_oid(indexOIDs[j]);
	}
	Oidx o(ind);
	unsigned long l = 0;
	for (unsigned int i=0; ((i<index_len) && (l < o.len())); i++) {
		if (index_struc[i].implied) {
			if (i+1 != index_len) { 
				delete[] vbs;
				return 0;			
			}
			if (l < o.len()) {
			  if ((index_struc[i].type == sNMP_SYNTAX_OCTETS) ||
			      (index_struc[i].type == sNMP_SYNTAX_IPADDR)) {
			      if (!check_index(o, l, o.len())) {
					delete[] vbs;
					return 0;
			      }
			  }	
			  get_index_value(index_struc[i].type, 
					  o.cut_left(l), vbs[i]);
			  return vbs;
			}
			delete[] vbs;
			return 0;
		}
		else if ((!index_struc[i].implied) && 
			 (index_struc[i].min != index_struc[i].max)) {
			if (o.len() < o[l]+1) {
				delete[] vbs;
				return 0;
			}
			if ((o[l] < index_struc[i].min) ||
			    (o[l] > index_struc[i].max)) {
				delete[] vbs;
				return 0;
			}
			if ((index_struc[i].type == sNMP_SYNTAX_OCTETS) ||
			    (index_struc[i].type == sNMP_SYNTAX_IPADDR)) {
			  if (!check_index(o, l, l+o[l]+1)) {
				delete[] vbs;
				return 0;
			  }
			}
			// cut off length byte
			Oidx ind(o.cut_left(l+1));
			ind.trim(ind.len()-o[l]);
			get_index_value(index_struc[i].type, ind, vbs[i]);
			l += o[l]+1;
		}
		else {
			if ((index_struc[i].type == sNMP_SYNTAX_OCTETS) ||
			    (index_struc[i].type == sNMP_SYNTAX_IPADDR)) {
			  if (!check_index(o, l, l+index_struc[i].max)) {
				delete[] vbs;
				return 0;
			  }
			}			
			// min == max
			Oidx ind(o.cut_left(l));
			ind.trim(ind.len()-index_struc[i].max);
			get_index_value(index_struc[i].type, ind, vbs[i]); 
			l += index_struc[i].max;
		}
	}
	if (o.len() == l) return vbs;
	delete[] vbs;
	return 0;
}

bool AgentXSharedTable::allocate_index(const Oidx& ind) 
{
    if (indexStrategy == noIndexAllocation) {
        return TRUE;
    }
    else {
        u_int alloc_index_len = index_len;
        Vbx* vbs = create_index_vbs(ind);
	if (!vbs) return FALSE;
        switch (indexStrategy) {
            case alwaysFirstSubIndex: {
                alloc_index_len = 1;
                break;
            }
            case firstSubIndexOnly: {
                alloc_index_len = 1;
                int allocResult = backReference->get_index_registry()->
                        allocate(backReference->get_session()->get_id(), vbs[0], FALSE);
                switch(allocResult) {
                    case AGENTX_INDEX_ALREADY_ALLOCATED: {
                        delete[] vbs;
                        return TRUE;
                    }
                    case AGENTX_OK: {
                        break;
                    }
                    default: {
                        delete[] vbs;
                        return FALSE;
                    }
                }                
                break;
            }
            case anyNonAllocatedSubIndex: {
                bool allocNeeded = FALSE;
                for (unsigned int i=0; i<index_len; i++) {
                    int allocResult = backReference->get_index_registry()->
                        allocate(backReference->get_session()->get_id(), vbs[i], FALSE);
                    if (allocResult == AGENTX_OK) {
                        Vbx* vbsCopy = new Vbx[1];
                        vbsCopy[0] = vbs[i];
                        delete[] vbs;
                        vbs = vbsCopy;
                        alloc_index_len = 1;
                        allocNeeded = TRUE;
                        break;
                    }
                }
                if (!allocNeeded) {
                    delete[] vbs;
                    return TRUE;
                }
                break;
            }
            default:
                break;
                
        }
        
	backReference->allocate_index(myContext, vbs, alloc_index_len, this, 
                                      0 /* index values given*/, new Oidx(ind));
	delete[] vbs;
	return TRUE;
    }
}

void AgentXSharedTable::new_index()
{
	Vbx* vbs = new Vbx[index_len];
	for (unsigned int j=0; j<index_len; j++) {
		vbs[j].set_oid(indexOIDs[j]);
		vbs[j].set_syntax(index_struc[j].type);
	}
	backReference->allocate_index(myContext, vbs, index_len, this, 
				      AGENTX_NEW_INDEX);
	delete[] vbs;
}

void AgentXSharedTable::any_index()
{
	Vbx* vbs = new Vbx[index_len];
	for (unsigned int j=0; j<index_len; j++) {
		vbs[j].set_oid(indexOIDs[j]);
		vbs[j].set_syntax(index_struc[j].type);
	}
	backReference->allocate_index(myContext, vbs, index_len, this, 
				      AGENTX_ANY_INDEX);
	delete[] vbs;
}

void AgentXSharedTable::index_allocated(const Oidx& ind, int err, int errind,
                                        Vbx* vbs, u_int vbs_length)
{
        
	if (err == AGENTX_OK) {
            switch (indexStrategy) {
                case anyNonAllocatedSubIndex: {
                    Vbx* orig_vbs = create_index_vbs(ind);
                    if (!orig_vbs) return;
                     for (unsigned int i=0; i<index_len; i++) {
                         backReference->get_index_registry()->
                                allocate(backReference->get_session()->get_id(), 
                                         orig_vbs[i], false);
                     }
                     delete[] orig_vbs;
                     break;
                }
                    default:
                        break;
            }
            // default behavior is to add the row when allocation succeeded
            add_row(ind);
	}
    else {
        switch (indexStrategy) {
            case firstSubIndexOnly:
            case anyNonAllocatedSubIndex: {
                if (!vbs) return;
                 for (unsigned int i=0; i<vbs_length; i++) {
                     backReference->get_index_registry()->
                            release(backReference->get_session()->get_id(), 
                                    vbs[i], false);
                 }
                 break;
            }
            default:
                break;
        }            
    }
}


void AgentXSharedTable::clear()
{
	MibTableRow *hrSWRunTableEntry;
	while ((hrSWRunTableEntry = content.first())) {
		remove_row(hrSWRunTableEntry->get_index());
	}
}

void AgentXSharedTable::reinit() 
{
	OidListCursor<MibTableRow> cur;
	for (cur.init(&content); cur.get(); cur.next()) {
	    allocate_index(cur.get()->get_index());
	}
}

/*-------------------------- class SubAgentXMib -----------------------*/

AgentXSlave* SubAgentXMib::agentx = 0;

SubAgentXMib::SubAgentXMib(): Mib()
{
#ifdef _THREADS
	responsePool = 0;
#endif
	priority = AGENTX_DEFAULT_PRIORITY;
	session = 0;
	status = AGENTX_STATUS_CONNECT;
        Oidx context;
        indexRegistry = new AgentXIndexDB(context);
}

SubAgentXMib::SubAgentXMib(const OctetStr& c): Mib(c)
{
#ifdef _THREADS
	responsePool = 0;
#endif
	session = 0;
	priority = AGENTX_DEFAULT_PRIORITY;
	status = AGENTX_STATUS_CONNECT;
	Oidx context(Oidx::from_string(c, FALSE));
        indexRegistry = new AgentXIndexDB(context);
}

SubAgentXMib::~SubAgentXMib()
{
	if (session) {
            delete session;
            session = 0;
        }
        if (indexRegistry) {
            delete indexRegistry;
            indexRegistry = 0;
        }
#ifdef _THREADS
	if (responsePool) {
            delete responsePool;
            responsePool = 0;
        }
#endif
}

void SubAgentXMib::set_request_list(AgentXRequestList* reqList) 
{
	requestList = reqList;
	agentx = reqList->get_agentx();
	if (!agentx) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);
		LOG("SubAgentXMib: config error: given request list has no agentx protocol services, aborting...");
		LOG_END;
#ifdef _NO_LOGGING
		raise(SIGTERM);
#endif
	} 
}

MibEntryPtr SubAgentXMib::add(MibEntryPtr entry)
{
	return add("", entry);
}

MibEntryPtr SubAgentXMib::add_no_reg(MibEntryPtr entry)
{
	return add_no_reg("", entry);
}

MibEntryPtr SubAgentXMib::add_no_reg(const OctetStr& context, 
				     MibEntryPtr entry)
{
	return Mib::add(context, entry);
}

MibEntryPtr SubAgentXMib::add(const OctetStr& context, 
			      MibEntryPtr entry)
{
	MibEntryPtr p = Mib::add(context, entry);
        lock_mib();
	if (status == AGENTX_STATUS_RUN) {
		// need to register region at master
		MibContext* c = get_context(context); 
		if (!c) {
                        unlock_mib();
                        return p;
                }

		MibGroup* g = c->find_group(*entry->key());
		AgentXRegion* r = 0;
		if (g) {
			r = new AgentXRegion(*g->key());
		}
		else {
			g = c->find_group_of(*entry->key());
			if (g) {
				r = new AgentXRegion(*g->key());
				MibEntry* e = c->get(*entry->key());
				if ((e) && (e->type() == AGENTPP_LEAF)) {
					r->set_single(TRUE);
				}
			}
		}
		if (r) {
			unsigned int timeout = 0;
			if (g) 
				timeout = g->get_timeout();
			register_region(context, *r, timeout, TRUE);
			delete r;
		}
	}
        unlock_mib();
	return p;
}

MibEntryPtr SubAgentXMib::register_entry(const OctetStr& context, 
					 MibEntryPtr entry,
					 u_int timeout)
{
	lock_mib();
	if (status == AGENTX_STATUS_RUN) {
		// need to register region at master
		MibContext* c = get_context(context); 
		if (!c) {
                	unlock_mib();
                	return 0;
                }

		MibGroup* g = c->find_group(*entry->key());
		AgentXRegion* r = 0;
		if (g) {
			r = new AgentXRegion(*g->key());
		}
		else {
			g = c->find_group_of(*entry->key());
			if (g) {
				r = new AgentXRegion(*g->key());
				MibEntry* e = c->get(*entry->key());
				if ((e) && (e->type() == AGENTPP_LEAF)) {
					r->set_single(TRUE);
				}
			}
		}
		unlock_mib();
		if (r) {
			register_region(context, *r, timeout, TRUE);
			delete r;
		}
	}
	else {
		unlock_mib();
		return 0;
	}
	return entry;
}

void SubAgentXMib::init_response_pool() 
{
#ifdef _THREADS
	if (responsePool) {
		delete responsePool;
	}
	responsePool = new QueuedThreadPool(1);
	responsePool->start();
#endif	
}

bool SubAgentXMib::init()
{
	init_response_pool();
	// (re)initialize AgentX protocol
	agentx->reinit();
	// load persistent objects
	Mib::init();

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	LOG("SubAgentXMib: opening socket");
	LOG_END;

	bool ok = FALSE;
#ifdef AX_UNIX_SOCKET
	if (agentx->get_connect_mode() & AX_USE_UNIX_SOCKET) {
		if (agentx->bind_unix()) {
			ok = TRUE;
		}
	}
#endif
#ifdef AX_TCP_SOCKET
	if ((!ok) && (agentx->get_connect_mode() & AX_USE_TCP_SOCKET)) {
		if (agentx->bind_tcp()) {
			ok = TRUE;
		}
	}
#endif
	if (!ok) { 	  
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("SuAgentXMib: could not bind any port. Aborting.");
		LOG_END;
		return FALSE;
	}
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
	LOG("SubAgentXMib: contacting master, please wait");
	LOG_END;

	/* May be you want to change the user id back to the 
	 * saved (real) user id:
       	set_thread_pool(0);
	setuid(getuid());
	set_thread_pool(new ThreadPool(4));
	*/
	int _status = 0;
	if ((_status = open_session()) != AGENTX_OK) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("SubAgentXMib: could not open session (error)");
		LOG(_status);
		LOG_END;
		return FALSE;
	}

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
	LOG("SubAgentXMib: connected - now registering..");
	LOG_END;	

	threadPool->execute(new AgentXReconnect(this));

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 1);
	LOG("SubAgentXMib: nodes registering...");
	LOG_END;
	return TRUE;
}

bool SubAgentXMib::remove(const Oidx& oid)
{
	return remove("", oid);
}

bool SubAgentXMib::remove_no_unreg(const Oidx& oid)
{
	return remove_no_unreg("", oid);
}

bool SubAgentXMib::remove_no_unreg(const OctetStr& context, const Oidx& oid)
{
	return Mib::remove(context, oid);
}

bool SubAgentXMib::remove(const OctetStr& context, const Oidx& oid)
{
	lock_mib();
	if (status == AGENTX_STATUS_RUN) {
		// need to unregister region at master

		// everything here is a bit expensive, but removing should not
		// be that frequent
		MibContext* c = get_context(context);
		if (c) {
			MibGroup* g = c->find_group(oid);
			AgentXRegion* r = 0;
			if (g) {
				r = new AgentXRegion(*g->key());
			}
			else {
				g = c->find_group_of(oid);
				if (g) {
					r = new AgentXRegion(*g->key());
					MibEntry* e = c->get(oid);
					if ((e) && 
					    (e->type() == AGENTPP_LEAF)) {
						r->set_single(TRUE);
					}
				}
			}
			unlock_mib();
			if (r) {
				register_region(context, *r, 0, FALSE);
				delete r;
			}
		}
		else unlock_mib();
	}
        else unlock_mib();
	return Mib::remove(context, oid);
}

int SubAgentXMib::open_session() 
{
	if (session) {
		// disconnect
		delete session;
	}
	session = create_session();
	status = AGENTX_STATUS_CONNECT;
	return agentx->open_session(*session);
}

AgentXSession* SubAgentXMib::create_session() 
{
	AgentXSession* s = new AgentXSession(0);
	s->set_timeout(AGENTX_DEFAULT_TIMEOUT);
	s->set_peer(AgentXPeer(agentx->get_socket(), 0, 
			       AGENTX_DEFAULT_TIMEOUT, 0));
	return s;
}

unsigned long SubAgentXMib::ping_master() 
{
	if (session) {
		unsigned long tid = requestList->create_transaction_id();
		agentx->ping_session(*session, tid);
		return tid;
	}
	return 0;
}

void SubAgentXMib::ping_received(const AgentXPdu& ping) 
{
	LOG_BEGIN(loggerModuleName, INFO_LOG | 1);
	LOG("SubAgentXMib: PING response received (sid)(tid)(pid)(uptime)");
	LOG(ping.get_session_id());
	LOG(ping.get_transaction_id());
	LOG(ping.get_packet_id());
	LOG(ping.get_time());
	LOG_END;
}

void SubAgentXMib::reconnect()
{
	lock_mib();

        status = AGENTX_STATUS_REGISTER;

	// first register the groups
	OidListCursor<MibContext> content(&contexts);
	for (; content.get(); content.next()) {
		OidListCursor<MibGroup> cur(content.get()->get_groups());
		for (; cur.get(); cur.next()) {

			MibGroup* g = cur.get();

			LOG_BEGIN(loggerModuleName, INFO_LOG | 2);
			LOG("SubAgentXMib: registering group (context)(oid)");
			LOG(content.get()->get_name().get_printable());
			LOG(g->key()->get_printable());
			LOG_END;
			
			AgentXRegion* region = new AgentXRegion(*(g->key()));

			if (!register_region(content.get()->get_name(),
					     *region, g->get_timeout(), TRUE)){
				LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
				LOG("SubAgentXMib: register group failed for (oid)");
				LOG(g->key()->get_printable());
				LOG_END;
			}
			delete region;
		}
		OidListCursor<MibEntry> entries(content.get()->get_content());
		for (; entries.get(); entries.next()) {
			if (!content.get()->
			    find_group_of(*entries.get()->key())) {
				
				if (entries.get()->type() ==
				    AGENTPP_TABLE) {
				    LOG_BEGIN(loggerModuleName, INFO_LOG | 2);
				    LOG("SubAgentXMib: registering shared table rows (oid)");
				    LOG(entries.get()->key()->get_printable());
				    LOG_END;
				    ((MibTable*)entries.get())->reinit();
				    continue;
				}

				AgentXRegion* region = 
				  new AgentXRegion(*(entries.get()->key()));
				if (entries.get()->type() == AGENTPP_LEAF)
					region->set_single(TRUE);

				LOG_BEGIN(loggerModuleName, INFO_LOG | 2);
				LOG("SubAgentXMib: registering node (oid)(single)");
				LOG(entries.get()->key()->get_printable());
				LOG((region->is_single()) ? "true" : "false");
				LOG_END;
				// register node with default timeout
				register_region(content.get()->get_name(),
					        *region, 0, TRUE);
				delete region;
			}
		}
	}

        // From now on, add(), register_entry(), and remove() have to
        // issue their own registration requests.
        status = AGENTX_STATUS_RUN;

	unlock_mib();
}

void SubAgentXMib::synchronize_uptime(u_int time) 
{
	u_int now = sysUpTime::get();
#if !defined(_WIN32) && HAVE_CLOCK_GETTIME
        clock_gettime(CLOCK_MONOTONIC, &sysUpTime::start);
        sysUpTime::start.tv_sec -= time / 100;
        if (sysUpTime::start.tv_nsec < (time % 100) * 10000000) {
            sysUpTime::start.tv_sec--;
            sysUpTime::start.tv_nsec -= ((time % 100) * 10000000) - 1000000000;            
        }
        else {
            sysUpTime::start.tv_nsec -= (time % 100) * 10000000;        
        }
#else
	sysUpTime::start += now - time;
#endif        
}

void SubAgentXMib::process_response(AgentXRequest* r)
{
	synchronize_uptime(((AgentXPdu*)r->get_pdu())->get_time());
	// first match response to pending request
	AgentXQueue* queue = agentx->get_queue();
	queue->lock();

	AgentXPdu* response = r->get_agentx_pdu();
	AgentXPdu* pending = queue->find(response->get_packet_id());
	if (!pending) {
		queue->unlock();
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 2);
		LOG("SubAgentXMib: could not match response with a pending request (pid)");
		LOG(response->get_packet_id());
		LOG_END;
		// remove request from list
		((AgentXRequestList*)requestList)->remove(r);
		return;
	}
	switch (pending->get_agentx_type()) {
	case AGENTX_OPEN_PDU: {
		session->set_id(response->get_session_id());
		session->peer.nSessions++;
		status = AGENTX_STATUS_OPEN;
		queue->remove(pending);
		queue->unlock();
		break;
	}
	case AGENTX_PING_PDU: {
		queue->remove(pending);
		queue->unlock();
		ping_received(*response);
		break;
	}
	case AGENTX_INDEXALLOCATE_PDU: 
	case AGENTX_INDEXDEALLOCATE_PDU: {
		int type = pending->get_agentx_type();
		queue->remove(pending);
		queue->unlock();
		pendingMessagesLock.lock();
		AgentXPendingMessage* m = 
		  get_pending_message(response->get_packet_id());
		if (m) pendingMessages.remove(m);
		pendingMessagesLock.unlock();
		int sz = response->get_vb_count();
		Vbx* vbs = new Vbx[sz];
		response->get_vblist(vbs, sz);
		if (m) {
			LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
			LOG("SubAgentXMib: index (de)allocate response (oid)(err)(errind)(values..)");
			LOG(response->get_error_status());
			LOG(response->get_error_index());
			LOG(m->receiver->key()->get_printable());
			for (int i=0; i<sz; i++) {
			  LOG(vbs[i].get_printable_oid());
			  LOG(vbs[i].get_printable_value());
			}
			LOG_END;
			if (type == AGENTX_INDEXALLOCATE_PDU)
				index_allocated(response->get_error_status(),
						response->get_error_index(),
						vbs, sz, m->receiver, 
                                                response->get_packet_id(), 
                                                m->row_index);
			else
				index_deallocated(response->get_error_status(),
						  response->get_error_index(),
						  vbs, sz, m->receiver, 
                                                  response->get_packet_id(), 
                                                  m->row_index);
			delete m;
		}
		else {
			if (type == AGENTX_INDEXALLOCATE_PDU)
				index_allocated(response->get_error_status(),
						response->get_error_index(),
						vbs, sz, 0, 0, 0);
			else
				index_deallocated(response->get_error_status(),
						  response->get_error_index(),
						  vbs, sz, 0, 0, 0);
		}
		delete[] vbs;
		break;
	}
	case AGENTX_UNREGISTER_PDU: 
	case AGENTX_REGISTER_PDU: {
		AgentXRegion reg(pending->get_region());
		int type = pending->get_agentx_type();
		queue->remove(pending);
		queue->unlock();		
		pendingMessagesLock.lock();
		AgentXPendingMessage* m = 
		  get_pending_message(response->get_packet_id());
		if (m) pendingMessages.remove(m);
		pendingMessagesLock.unlock();

		if (response->get_error_status() != AGENTX_OK) {
			LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
			if (type == AGENTX_UNREGISTER_PDU)
			  LOG("SubAgentXMib: unregistration failed for (oid)(err)");
			else
			  LOG("SubAgentXMib: registration failed for (oid)(err)");
			LOG(reg.get_lower().get_printable());
			LOG((response->get_error_status() == 
			     AGENTX_DUPLICATE_REGISTRATION) ? 
			    "duplicate registration" : "request denied");
			LOG(response->get_error_status());
			LOG_END;
			registration_failed(reg, response->get_error_status(),
					    (m) ? m->receiver : 0);
		}
		else {
			LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
			if (type == AGENTX_UNREGISTER_PDU)
			  LOG("SubAgentXMib: unregistration success (oid)");
			else
			  LOG("SubAgentXMib: registration success (oid)");
			LOG(reg.get_lower().get_printable());
			LOG_END;
			registration_success(reg, (m) ? m->receiver : 0);
		}
		if (m) delete m;
		break;
	}
	default: {
		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
		LOG("SubAgentXMib: unhandled response (tid)(pid)");
		LOG(pending->get_transaction_id());
		LOG(pending->get_packet_id());
		LOG_END;
	  
		queue->remove(pending);
		queue->unlock();		
		break;
	}
	}
	((AgentXRequestList*)requestList)->remove(r);
#ifdef sched_yield
	// Added here because it has been reported that there might be
	// a race condition when closing connection to master agent 
	sched_yield();
#endif
}


void SubAgentXMib::do_process_request(Request* req)
{
#ifdef _THREADS
	LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	LOG("SubAgent: starting thread execution");
	LOG_END;
#endif
	int n = req->subrequests();
	int i;
	AgentXPdu * r = (AgentXPdu*)req->get_pdu();
	switch (r->get_agentx_type()) {
	case AGENTX_GET_PDU: {
	  
		  LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
		  LOG("SubAgentXMib: GET (tid)(pid)(oid)...");
		  LOG(r->get_transaction_id());
		  LOG(r->get_packet_id());
		  for (i=0; i<n; i++)
			LOG(req->get_oid(i).get_printable());
		  LOG_END;
		  
		  for (i=0; i<n; i++) {
			if (!process_request(req, i)) break; 
		  }
		  break;
	}
	case AGENTX_GETNEXT_PDU: {

		  LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
		  LOG("SubAgentXMib: GETNEXT (tid)(pid)(oid)...");
		  LOG(r->get_transaction_id());
		  LOG(r->get_packet_id());
		  for (i=0; i<n; i++) {
			OctetStr str((const unsigned char*)req->get_oid(i).get_printable(), 50);
			LOG(str.get_printable());
		  }
		  LOG_END;

		  for (i=0; i<n; i++) {
			if (!process_request(req, i)) break;
		  }
		  break;
	}
	case AGENTX_GETBULK_PDU: {

		  LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
		  LOG("SubAgentXMib: GETNEXT (tid)(pid)");
		  LOG(r->get_transaction_id());
		  LOG(r->get_packet_id());
		  LOG_END;
		  if (n <= 0) break;
		  process_get_bulk_request(req);
		  break;
	}
	case AGENTX_TESTSET_PDU: {
		  
		  LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
		  LOG("SubAgentXMib: TESTSET (tid)(pid)(oid)...");
		  LOG(r->get_transaction_id());
		  LOG(r->get_packet_id());
		  for (int j=0; j<n; j++) 
			LOG(req->get_oid(j).get_printable());
		  LOG_END;
		  if (n <= 0) break;
		  process_prepare_set_request(req);
		  break;		
	}
	case AGENTX_COMMITSET_PDU: {

		  LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
		  LOG("SubAgentXMib: COMMITSET (tid)(pid)(oid)...");
		  LOG(r->get_transaction_id());
		  LOG(r->get_packet_id());
		  for (int j=0; j<n; j++) 
			LOG(req->get_oid(j).get_printable());
		  LOG_END;

		  if (n <= 0) break;
		  if (process_commit_set_request(req) != SNMP_ERROR_SUCCESS) {
		  
			LOG_BEGIN(loggerModuleName, WARNING_LOG | 2);
			LOG("SubAgentXMib: commit failed (tid)(pid)");
			LOG(r->get_transaction_id());
			LOG(r->get_packet_id());
			LOG_END;
		  }
		  break;
	}
	case AGENTX_UNDOSET_PDU: {

		  LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
		  LOG("SubAgentXMib: UNDOSET (tid)(pid)(oid)...");
		  LOG(r->get_transaction_id());
		  LOG(r->get_packet_id());
		  for (int j=0; j<n; j++) 
			LOG(req->get_oid(j).get_printable());
		  LOG_END;

		  process_undo_set_request(req);
		  break;
	}
	case AGENTX_CLEANUPSET_PDU: {
		  LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
		  LOG("SubAgentXMib: CLEANUPSET (tid)(pid)(oid)...");
		  LOG(r->get_transaction_id());
		  LOG(r->get_packet_id());
		  for (int j=0; j<n; j++) 
			LOG(req->get_oid(j).get_printable());
		  LOG_END;

		  if (n <= 0) break;
		  process_cleanup_set_request(req);
		  ((AgentXRequestList*)requestList)->remove(req);
		  // do not finalize <-> cleanup is not answered
		  return;
	}
	case AGENTX_RESPONSE_PDU: {
#ifdef _THREADS
		  AgentXResponse* resp = 
		      new AgentXResponse(this, (AgentXRequest*)req); 
		  // asynchronously process responses
		  responsePool->execute(resp);
#else
		  process_response((AgentXRequest*)req);
#endif		  
		  LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
		  LOG("Agent: finished thread execution");
		  LOG_END;
		  // do not finalize <-> it´s a response
		  return;
	}
	case AGENTX_CLOSE_PDU: {
		  ((AgentXRequestList*)requestList)->remove(req);
		  LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		  LOG("SubAgentXMib: received close PDU");
		  LOG_END;
		  agentx->stopit = TRUE;
		  status = AGENTX_STATUS_CLOSED;
		  ((AgentXRequestList*)requestList)->lock();
		  ((AgentXRequestList*)requestList)->terminate_set_requests();
		  ((AgentXRequestList*)requestList)->unlock();
		  return;
	}
	}
	finalize(req);

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	LOG("Agent: finished thread execution");
	LOG_END;
}


int SubAgentXMib::find_next(MibContext* context,
			    const Oidx& oid, MibEntryPtr& entry, 
			    Request* req, const int reqind, Oidx& nextOid)
{
	int err = Mib::find_next(context, oid, entry, req, reqind, nextOid);
	if (err == SNMP_ERROR_SUCCESS) {
		AgentXPdu* r = (AgentXPdu*)req->get_pdu();
		AgentXSearchRange range = r->get_range(reqind);
		// test for valid range
		if (range.get_lower() <= range.get_upper()) {
                    if (!is_complex_node(entry)) { 
                      if (!range.includes(*entry->key()))
                            return sNMP_SYNTAX_NOSUCHOBJECT;
                    }
                    else {
                      LOG_BEGIN(loggerModuleName, DEBUG_LOG | 3);
                      LOG("SubAgentXMib: find next (lower)(upper)(incl)(succ)");
                      LOG(range.get_lower().get_printable());
                      LOG(range.get_upper().get_printable());
                      LOG((range.is_lower_included()) ? "TRUE":"FALSE");
                      LOG((nextOid.len() == 0) ? 
                          entry->find_succ(oid, req).get_printable() : nextOid.get_printable());
                      LOG_END;
                      if (((nextOid.len() > 0) && !range.includes(nextOid))
                          || ((nextOid.len() == 0) && 
                              (!range.includes(entry->find_succ(oid, req))))) {
                            LOG_BEGIN(loggerModuleName, DEBUG_LOG | 3);
                            LOG("SubAgentXMib: find next (failed)");
                            LOG_END;
                            return sNMP_SYNTAX_NOSUCHOBJECT;
                      }
                    }
		}
	}
	return err;
}


int SubAgentXMib::notify(const OctetStr& context, const Oidx& trapoid,
			 Vbx* vbs, int sz, unsigned int timestamp)
{
	if (!session) {
		not_connected();
		return SNMP_CLASS_TL_FAILED;
	}
	AgentXPdu* pdu = 
	  new AgentXPdu(session->get_byte_order(), AGENTX_NOTIFY_PDU);
	pdu->set_flags(0x00);
	pdu->set_session_id(session->get_id());
	pdu->set_transaction_id(requestList->create_transaction_id());

	pdu->set_timeout(session->get_timeout());
	pdu->set_timestamp(agentx->compute_timeout(session->get_timeout()));
	pdu->set_context(context);

	//payload

	int size = sz + ((timestamp) ? 2 : 1); 
	Vbx* payload = new Vbx[size];
	int to = 0;
	if (timestamp) {
		payload[0].set_oid(SNMP_SYSUPTIME_OID);
		TimeTicks tt(timestamp);
		payload[0].set_value(tt);
	        to = 1;
	}
	payload[to].set_oid(SNMP_TRAP_OID);
	payload[to].set_value(trapoid);
	to++;
	for (int i=0; i<sz; i++, to++) {
		payload[to] = vbs[i]; 
	}
	pdu->set_vblist(payload, size);

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	LOG("AgentXSlave: sending notify PDU (trap)");
	LOG(payload[(timestamp)?1:0].get_printable_oid());
	LOG_END;

	agentx->get_queue()->add(pdu);
	agentx->send(*pdu);
	delete[] payload;
	return SNMP_ERROR_SUCCESS;
}


void SubAgentXMib::remove_pending_messages(MibEntry* entry) 
{
	pendingMessagesLock.lock();
	ListCursor<AgentXPendingMessage> cur;
	for (cur.init(&pendingMessages); cur.get(); ) {
		if (cur.get()->receiver == entry) {
			AgentXPendingMessage* victim = cur.get();
			cur.next();
			delete pendingMessages.remove(victim);
		}
		else cur.next();
	}
	pendingMessagesLock.unlock();
} 

AgentXPendingMessage* SubAgentXMib::get_pending_message(u_int pid)
{
	ListCursor<AgentXPendingMessage> cur;
	for (cur.init(&pendingMessages); cur.get(); cur.next()) {
		if (cur.get()->packet_id == pid)
			return cur.get();
	}
	return 0;
}

u_int SubAgentXMib::allocate_index(const OctetStr& context, 
	 			   Vbx* indexValues, int size, 
				   AgentXSharedTable* table, 
				   u_char indexType,
                                   Oidx* rowIndex)
{
	if (!session) {
		not_connected();
                delete rowIndex;
		return 0;
	}
	AgentXPdu* pdu = 
	  new AgentXPdu(session->get_byte_order(), AGENTX_INDEXALLOCATE_PDU);
	pdu->set_flags(indexType);
	pdu->set_session_id(session->get_id());
        u_int tid = requestList->create_transaction_id();
	pdu->set_transaction_id(tid);

	pdu->set_timeout(session->get_timeout());
	pdu->set_timestamp(agentx->compute_timeout(session->get_timeout()));
	pdu->set_context(context);
	//payload
	pdu->set_vblist(indexValues, size);

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	LOG("AgentXSlave: sending index allocate PDU (sz)(type)");
	LOG(size);
	switch (indexType) {
	case 0:
	  LOG("USER");
	  break;
	case AGENTX_NEW_INDEX:
	  LOG("NEW_INDEX");
	  break;
	case AGENTX_ANY_INDEX:
	  LOG("ANY_INDEX");
	  break;
	}
	LOG_END;

	agentx->get_queue()->add(pdu);
	pendingMessagesLock.lock();
        u_int pid = pdu->get_packet_id();
	pendingMessages.add(new AgentXPendingMessage(pid, table, rowIndex));
	pendingMessagesLock.unlock();
	agentx->send(*pdu);
        return pid;
}

u_int SubAgentXMib::deallocate_index(const OctetStr& context, 
				     Vbx* indexValues, int size, 
				     AgentXSharedTable* table,
                                     Oidx* rowIndex)
{
	if (!session) {
		not_connected();
                delete rowIndex;
		return 0;
	}
	AgentXPdu* pdu = 
	  new AgentXPdu(session->get_byte_order(), AGENTX_INDEXDEALLOCATE_PDU);
	pdu->set_flags(0x00);
	pdu->set_session_id(session->get_id());
        u_int tid = requestList->create_transaction_id();
	pdu->set_transaction_id(tid);

	pdu->set_timeout(session->get_timeout());
	pdu->set_timestamp(agentx->compute_timeout(session->get_timeout()));
	pdu->set_context(context);
	//payload
	pdu->set_vblist(indexValues, size);

	LOG_BEGIN(loggerModuleName, EVENT_LOG | 2);
	LOG("AgentXSlave: sending index deallocate PDU (sz)");
	LOG(size);
	LOG_END;

	agentx->get_queue()->add(pdu);
	pendingMessagesLock.lock();
        u_int pid = pdu->get_packet_id();
	pendingMessages.add(new AgentXPendingMessage(pid, table, rowIndex));
	pendingMessagesLock.unlock();
	agentx->send(*pdu);
        return pid;
}

void SubAgentXMib::index_allocated(u_int err, u_int errind,
				   Vbx* vbs, int sz, AgentXSharedTable* table,
                                   u_int packetID, Oidx* index)
{
	if (table) {
                Oidx ind;
                if (index) {
                    ind = *index;
                }
                else {
                    ind = table->get_index_from_vbs(vbs, sz);
                }
		table->start_synch();
		table->index_allocated(ind, err, errind, vbs, sz);
		table->end_synch();
	}
}

bool SubAgentXMib::register_region(const OctetStr& context, 
				      const AgentXRegion& region,
				      unsigned int timeout,
				      bool action,
				      AgentXSharedTable* table)
{
	if (!session) {
		not_connected();
		return FALSE;
	}
	u_int tid = requestList->create_transaction_id();
	AgentXPdu*	pdu;
	int             result;

	pdu = new AgentXPdu(session->get_byte_order(), 
			    (action) ? 
			    AGENTX_REGISTER_PDU : AGENTX_UNREGISTER_PDU );
	pdu->set_flags(0x00);
	pdu->set_session_id(session->get_id());
	pdu->set_transaction_id(tid);

	if (timeout > 0) 
		pdu->set_timeout(timeout);
	else
		pdu->set_timeout(session->get_timeout());
	pdu->set_timestamp(agentx->compute_timeout(session->get_timeout()));

	pdu->set_priority(priority);
	pdu->set_context(context);

	//payload
	pdu->set_region(region);

	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 2);
	if (action)
		LOG("AgentXSlave: sending register PDU (context)(oid)(rangeid)(upper)");
	else
		LOG("AgentXSlave: sending unregister PDU (context)(oid)(rangeid)(upper)");
	LOG(OctetStr(context).get_printable());
	LOG(Oidx(region.get_lower()).get_printable());
	LOG(region.get_subid());
	LOG(Oidx(region.get_upper()).get_printable());
	LOG_END;

	agentx->get_queue()->add(pdu);

	if (table) {
		pendingMessagesLock.lock();
		pendingMessages.add(
		   new AgentXPendingMessage(pdu->get_packet_id(), table));
		pendingMessagesLock.unlock();
	}
	
	result = agentx->send(*pdu);
	if (result != AGENTX_OK) {
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
		LOG("AgentXSlave: error sending (un)register PDU (error)");
		LOG(result);
		LOG_END;
		return FALSE;
	}
	return TRUE;
}

void SubAgentXMib::registration_success(const AgentXRegion& region, 
					AgentXSharedTable* table)
{
	if (table) {
		Oidx ind(table->index(region.get_lower()));
		// synchronize with table
		table->start_synch();
		MibTableRow* r = table->find_index(ind);
		if (r) {
			table->fire_row_changed(rowCreateAndGo, r, ind);
		}
		table->end_synch();
	}
}

bool SubAgentXMib::add_agent_caps(const OctetStr& context,
				     const Oidx& id,
				     const OctetStr& descr)
{
	if (!session) {
		not_connected();
		return FALSE;
	}
	if (!get_context(context)) return FALSE;

	u_int tid = requestList->create_transaction_id();
	AgentXPdu*	pdu;
	int             result;

	pdu = new AgentXPdu(session->get_byte_order(), 
			    AGENTX_ADDAGENTCAPS_PDU);
	pdu->set_flags(0x00);
	pdu->set_session_id(session->get_id());
	pdu->set_transaction_id(tid);

	pdu->set_timeout(session->get_timeout());
	pdu->set_timestamp(agentx->compute_timeout(session->get_timeout()));
	pdu->set_context(context);

	//payload
	Vbx vb(id);
	vb.set_value(descr);
	*pdu += vb;

	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 2);
	LOG("AgentXSlave: sending add agent caps PDU (context)(id)(descr)");
	LOG(OctetStr(context).get_printable());
	LOG(vb.get_printable_oid());
	LOG(vb.get_printable_value());
	LOG_END;

	agentx->get_queue()->add(pdu);
	result = agentx->send(*pdu);
	if (result != AGENTX_OK) {
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
		LOG("AgentXSlave: error sending (un)register PDU (error)");
		LOG(result);
		LOG_END;
	}
	return TRUE;
}

void SubAgentXMib::remove_agent_caps(const OctetStr& context,
				     const Oidx& id)
{
	if (!session) {
		not_connected();
		return;
	}
	u_int tid = requestList->create_transaction_id();
	AgentXPdu*	pdu;
	int             result;

	pdu = new AgentXPdu(session->get_byte_order(), 
			    AGENTX_REMOVEAGENTCAPS_PDU);
	pdu->set_flags(0x00);
	pdu->set_session_id(session->get_id());
	pdu->set_transaction_id(tid);

	pdu->set_timeout(session->get_timeout());
	pdu->set_timestamp(agentx->compute_timeout(session->get_timeout()));
	pdu->set_context(context);

	//payload
	Vbx vb(id);
	*pdu += vb;

	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 2);
	LOG("AgentXSlave: sending remove agent caps PDU (context)(id)");
	LOG(OctetStr(context).get_printable());
	LOG(vb.get_printable_oid());
	LOG_END;

	agentx->get_queue()->add(pdu);
	result = agentx->send(*pdu);
	if (result != AGENTX_OK) {
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
		LOG("AgentXSlave: error sending (un)register PDU (error)");
		LOG(result);
		LOG_END;
	}	
}

void SubAgentXMib::not_connected()
{
	LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
	LOG("SubAgentXMib: not connected with master agent!");
	LOG_END;
}

void SubAgentXMib::delete_request(Request* req) 
{
	AgentXPdu* pdu = (AgentXPdu*)req->get_pdu();

	// check if we need request for further processing
	switch (pdu->get_agentx_type()) {
	    case AGENTX_TESTSET_PDU:
	    case AGENTX_COMMITSET_PDU:
		((AgentXRequest*)req)->unlock();
		break;
	    default:
		delete req;
	} 
}


#ifdef AGENTPP_NAMESPACE
}
#endif

