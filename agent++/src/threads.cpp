/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - threads.cpp  
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

#include <agent_pp/threads.h>
#include <agent_pp/mib_entry.h>
#include <agent_pp/mib.h>
#include <snmp_pp/log.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

static const char *loggerModuleName = "agent++.threads";

#ifdef _THREADS
Synchronized ThreadManager::global_lock;
#endif

/**
 * Default constructor
 */
ThreadManager::ThreadManager()
{
}

/**
 * Destructor
 */
ThreadManager::~ThreadManager()
{
#if defined(_THREADS) && !defined(NO_FAST_MUTEXES)
    trylock();
    unlock();
#endif    
}

/**
 * Start synchronized execution.
 */
void ThreadManager::start_synch()
{
#ifdef _THREADS
	lock();
#endif
}

/**
 * End synchronized execution.
 */
void ThreadManager::end_synch()
{
#ifdef _THREADS
	unlock();
#endif
}

/**
 * Start global synchronized execution.
 */
void ThreadManager::start_global_synch()
{
#ifdef _THREADS
	global_lock.lock();
#endif
}

/**
 * End global synchronized execution.
 */
void ThreadManager::end_global_synch()
{
#ifdef _THREADS
	global_lock.unlock();
#endif
}


ThreadSynchronize::ThreadSynchronize(ThreadManager& sync): s(sync)
{
#ifdef _THREADS
	s.start_synch();
#endif
}

ThreadSynchronize::~ThreadSynchronize()
{
#ifdef _THREADS
	s.end_synch();
#endif
}

SingleThreadObject::SingleThreadObject(): ThreadManager()
{
	start_synch();
}

SingleThreadObject::~SingleThreadObject()
{
	end_synch();
}

#ifdef _THREADS

/*--------------------- class Synchronized -------------------------*/

#ifndef _NO_LOGGING
int Synchronized::next_id = 0;
#endif

#define ERR_CHK_WITHOUT_EXCEPTIONS(x) \
	do { \
	    int result = (x); \
	    if (result) { \
		    LOG_BEGIN(loggerModuleName, ERROR_LOG | 0); \
		    LOG("Constructing Synchronized failed at '" #x "' with (result)"); \
		    LOG(result); \
		    LOG_END; \
	    } \
	} while(0)

Synchronized::Synchronized()
{
#ifndef _NO_LOGGING
        id = next_id++;
#endif        
#ifdef POSIX_THREADS
	pthread_mutexattr_t attr;
	ERR_CHK_WITHOUT_EXCEPTIONS( pthread_mutexattr_init(&attr) );
	ERR_CHK_WITHOUT_EXCEPTIONS( pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) );

	memset(&monitor, 0, sizeof(monitor));
	ERR_CHK_WITHOUT_EXCEPTIONS( pthread_mutex_init(&monitor, &attr) );
	ERR_CHK_WITHOUT_EXCEPTIONS( pthread_mutexattr_destroy(&attr) );

	memset(&cond, 0, sizeof(cond));
	ERR_CHK_WITHOUT_EXCEPTIONS( pthread_cond_init(&cond, 0) );
#else
#ifdef WIN32
	// Semaphore initially auto signaled, auto reset mode, unnamed
	semEvent = CreateEvent(0, FALSE, FALSE, 0);
	// Semaphore initially unowned, unnamed
	semMutex = CreateMutex(0, FALSE, 0);
#endif
#endif
	isLocked = FALSE;
}


Synchronized::~Synchronized()
{
#ifdef POSIX_THREADS
	int result;

	result = pthread_cond_destroy(&cond);
	if (result) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 2);
		LOG("Synchronized cond_destroy failed with (result)(ptr)");
		LOG(result);
		LOG((unsigned long)this);
		LOG_END;
	}
	result = pthread_mutex_destroy(&monitor);
#ifdef NO_FAST_MUTEXES
        if( result == EBUSY ) {
            // wait for other threads ...
            if( EBUSY == pthread_mutex_trylock(&monitor) )
                pthread_mutex_lock(&monitor); // another thread owns the mutex, let's wait ...
            do {
                pthread_mutex_unlock(&monitor);
                result = pthread_mutex_destroy(&monitor);
            } while( EBUSY == result );
        }
#endif
	isLocked = FALSE;
	if (result) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 2);
		LOG("Synchronized mutex_destroy failed with (result)(ptr)");
		LOG(result);
		LOG((unsigned long)this);
		LOG_END;
	}
#else
#ifdef WIN32
	CloseHandle(semEvent);
	CloseHandle(semMutex);
	isLocked = FALSE;
#endif
#endif
}


void Synchronized::wait() {
#ifdef POSIX_THREADS
	cond_timed_wait(0);
#else
#ifdef WIN32
	wait(INFINITE);
#endif
#endif  
}

#ifdef POSIX_THREADS
int Synchronized::cond_timed_wait(const struct timespec *ts) 
{
        int result;
        isLocked = FALSE;
        if (ts) 
              result = pthread_cond_timedwait(&cond, &monitor, ts);
        else 
              result = pthread_cond_wait(&cond, &monitor);
        isLocked = TRUE;
        return result;
}
#endif

bool Synchronized::wait(unsigned long timeout)
{
	bool timeoutOccurred = FALSE;
#ifdef POSIX_THREADS
	struct timespec ts;
#ifdef HAVE_CLOCK_GETTIME
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += (int)timeout/1000;
        int millis = ts.tv_nsec / 1000000 + (timeout % 1000);
        if (millis >= 1000) {
            ts.tv_sec += 1;
        }
        ts.tv_nsec = (millis % 1000) * 1000000;
#else        
	struct timeval  tv;
	gettimeofday(&tv, 0);
	ts.tv_sec  = tv.tv_sec  + (int)timeout/1000;
    int millis = tv.tv_usec / 1000 + (timeout % 1000);
    if (millis >= 1000) {
        ts.tv_sec += 1;
    }
    ts.tv_nsec = (millis % 1000) * 1000000;
#endif        

	int err;
	isLocked = FALSE;
	if ((err = cond_timed_wait(&ts)) > 0) {
		switch(err) {
        case EINVAL:
		  LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
		  LOG("Synchronized: wait with timeout returned (error)");
		  LOG(err);
		  LOG_END;
		case ETIMEDOUT:
		  timeoutOccurred = TRUE;
		  break;
		default:
		  LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		  LOG("Synchronized: wait with timeout returned (error)");
		  LOG(err);
		  LOG_END;
		  break;
		}
	}
#else
#ifdef WIN32
	isLocked = FALSE;
	if (!ReleaseMutex(semMutex)) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 2);
		LOG("Synchronized: releasing mutex failed");
		LOG_END;
	}
	int err;
	err = WaitForSingleObject(semEvent, timeout);
	switch (err) {
	case WAIT_TIMEOUT:
		LOG_BEGIN(loggerModuleName, EVENT_LOG | 8);
		LOG("Synchronized: timeout on wait");
		LOG_END;
		timeoutOccurred = TRUE;
		break;
	case WAIT_ABANDONED:
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 2);
		LOG("Synchronized: waiting for event failed");
		LOG_END;
	}
	if (WaitForSingleObject (semMutex, INFINITE) != WAIT_OBJECT_0) {
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 8);
		LOG("Synchronized: waiting for mutex failed");
		LOG_END;
	}
#endif 
#endif
	isLocked = TRUE;
	return timeoutOccurred;
}

void Synchronized::notify() {
#ifdef POSIX_THREADS
	int result;
	result = pthread_cond_signal(&cond);
	if (result) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("Synchronized: notify failed (result)");
		LOG(result);
		LOG_END;
	}
#else
#ifdef WIN32
	numNotifies = 1;
	if (!SetEvent(semEvent)) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("Synchronized: notify failed");
		LOG_END;
	}
#endif
#endif
}


void Synchronized::notify_all() {
#ifdef POSIX_THREADS
	int result;
	result = pthread_cond_broadcast(&cond);
	if (result) {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("Synchronized: notify_all failed (result)");
		LOG(result);
		LOG_END;
	}
#else
#ifdef WIN32
	numNotifies = (char)0x80;
	while (numNotifies--)
		if (!SetEvent(semEvent)) {
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			LOG("Synchronized: notify failed");
			LOG_END;
		}
#endif 
#endif 
}

bool Synchronized::lock() {
#ifdef POSIX_THREADS
    if (pthread_mutex_lock(&monitor) == 0) {
        if (isLocked) {
            // This thread owns already the lock, but
            // we do not like recursive locking. Thus
            // release it immediately and print a warning!
            if (pthread_mutex_unlock(&monitor) != 0) {
                LOG_BEGIN(loggerModuleName, WARNING_LOG | 0);
                LOG("Synchronized: unlock failed on recursive lock (id)");
                LOG(id);
                LOG_END;
            }
            else {
                LOG_BEGIN(loggerModuleName, WARNING_LOG | 5);
                LOG("Synchronized: recursive locking detected (id)!");
                LOG(id);
                LOG_END;
            }
        }
        else {
            isLocked = TRUE;
            // no logging because otherwise deep (virtual endless) recursion
        }            
        return TRUE;
    }
    else {
        LOG_BEGIN(loggerModuleName, DEBUG_LOG | 8);
        LOG("Synchronized: lock failed (id)");
        LOG(id);
        LOG_END;
        return FALSE;
    }
#else
#ifdef WIN32
    if (WaitForSingleObject(semMutex, INFINITE) != WAIT_OBJECT_0) {
	LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
	LOG("Synchronized: lock failed");
	LOG_END;
	return FALSE;
    }
    if (isLocked) {
	// This thread owns already the lock, but
        // we do not like recursive locking. Thus
        // release it immediately and print a warning!
	if (!ReleaseMutex(semMutex)) {
            LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
            LOG("Synchronized: unlock failed (id)");
            LOG(id);
            LOG_END;
            return FALSE;
	}      
	LOG_BEGIN(loggerModuleName, WARNING_LOG | 5);
	LOG("Synchronized: recursive locking detected (id)!");
        LOG(id);
	LOG_END;	
    }
    isLocked = TRUE;
    return TRUE;
#endif
#endif
}

bool Synchronized::unlock() {
        bool wasLocked = isLocked;
	isLocked = FALSE;
#ifdef POSIX_THREADS
        int err;
	if ((err = pthread_mutex_unlock(&monitor)) != 0) {            
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
		LOG("Synchronized: unlock failed (id)(error)(wasLocked)");
        LOG(id);
        LOG(err);
        LOG(wasLocked);
		LOG_END;
        isLocked = wasLocked;
        return FALSE;
    }
#else
#ifdef WIN32
	if (!ReleaseMutex(semMutex)) {
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
		LOG("Synchronized: unlock failed (id)(isLocked)(wasLocked)");
        LOG(id);
        LOG(isLocked);
        LOG(wasLocked);
		LOG_END;
        isLocked = wasLocked;
		return FALSE;
	}
#endif
#endif
        return TRUE;
}

Synchronized::TryLockResult Synchronized::trylock() {
#ifdef POSIX_THREADS
    if (pthread_mutex_trylock(&monitor) == 0) {
        if (isLocked) {
            // This thread owns already the lock, but
            // we do not like true recursive locking. Thus
            // release it immediately and print a warning!
            if (pthread_mutex_unlock(&monitor) != 0) {
                LOG_BEGIN(loggerModuleName, WARNING_LOG | 0);
                LOG("Synchronized: unlock failed on recursive trylock (id)");
                LOG(id);
                LOG_END;
            }
            else {
                LOG_BEGIN(loggerModuleName, WARNING_LOG | 5);
                LOG("Synchronized: recursive trylocking detected (id)!");
                LOG(id);
                LOG_END;
            }
            return OWNED;
        }
        else {
            isLocked = TRUE;
            LOG_BEGIN(loggerModuleName, DEBUG_LOG | 8);
            LOG("Synchronized: trylock success (id)");
            LOG(id);
            LOG_END;
        }            
        return LOCKED;
    }
    else {
            LOG_BEGIN(loggerModuleName, DEBUG_LOG | 8);
            LOG("Synchronized: try lock busy (id)");
            LOG(id);
            LOG_END;
            return BUSY;
    }
#else
#ifdef WIN32
    int status = WaitForSingleObject(semMutex, 0);
    if (status != WAIT_OBJECT_0) {
        LOG_BEGIN(loggerModuleName, DEBUG_LOG | 9);
        LOG("Synchronized: try lock failed (id)");
        LOG(id);
        LOG_END;
        return BUSY;
    }
    if (isLocked) {
        if (!ReleaseMutex(semMutex)) {
            LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
            LOG("Synchronized: unlock failed (id)");
            LOG(id);
            LOG_END;
        }
        return OWNED;
    }
    else {
            isLocked = TRUE;
            LOG_BEGIN(loggerModuleName, DEBUG_LOG | 8);
            LOG("Synchronized: trylock success (id)");
            LOG(id);
            LOG_END;
    }
    return LOCKED;
#endif
#endif
}


/*------------------------ class Thread ----------------------------*/

ThreadList Thread::threadList; 

#ifdef POSIX_THREADS
void* thread_starter(void* t)
{
	Thread *thread = (Thread*)t;
	Thread::threadList.add(thread);

#ifndef NO_FAST_MUTEXES
	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
	LOG("Thread: started (tid)");
	LOG((AGENTPP_OPAQUE_PTHREAD_T)(thread->tid));
	LOG_END;
#endif

	thread->get_runnable().run();

#ifndef NO_FAST_MUTEXES
	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
	LOG("Thread: ended (tid)");
	LOG((AGENTPP_OPAQUE_PTHREAD_T)(thread->tid));
	LOG_END;
#endif
	Thread::threadList.remove(thread);
	thread->status = Thread::FINISHED;

	return t;
}
#else
#ifdef WIN32
DWORD thread_starter(LPDWORD lpdwParam)
{
	Thread *thread = (Thread*) lpdwParam;
	Thread::threadList.add(thread);

	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
	LOG("Thread: started (tid)");
	LOG(thread->tid);
	LOG_END;

	thread->get_runnable().run();

	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
	LOG("Thread: ended (tid)");
	LOG(thread->tid);
	LOG_END;

	Thread::threadList.remove(thread);
	thread->status = Thread::FINISHED;

	::SetEvent(thread->threadEndEvent);

	return 0;	
}
#endif
#endif

Thread::Thread()
{
	stackSize = AGENTPP_DEFAULT_STACKSIZE;
	runnable  = (Runnable*)this;
	status    = IDLE;
#ifdef WIN32
	threadHandle = INVALID_HANDLE_VALUE;
	threadEndEvent = ::CreateEvent(NULL, true, false, NULL);
#endif //WIN32
}

Thread::Thread(Runnable &r)
{
	stackSize = AGENTPP_DEFAULT_STACKSIZE;
	runnable  = &r;
	status    = IDLE;
#ifdef WIN32
	threadHandle = INVALID_HANDLE_VALUE;
	threadEndEvent = ::CreateEvent(NULL, true, false, NULL);
#endif //WIN32
}

void Thread::run()
{
	LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
	LOG("Thread: empty run method!");
	LOG_END;
}

Thread::~Thread() 
{ 
	if (status != IDLE)
	{
		join(); 
	}
#ifdef WIN32
	::CloseHandle(threadEndEvent);
	if(threadHandle != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(threadHandle);
		threadHandle = INVALID_HANDLE_VALUE;
	}
#endif
}


Runnable& Thread::get_runnable()
{
	return *runnable;
}

void Thread::join()
{
#ifdef POSIX_THREADS
	if (status) {
		void* retstat;
		int err = pthread_join(tid, &retstat);
		if (err) {
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			LOG("Thread: join failed (error)");
			LOG(err);
			LOG_END;
		}
		status = IDLE;
		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 4);
		LOG("Thread: joined thread successfully (tid)");
		LOG((AGENTPP_OPAQUE_PTHREAD_T)tid);
		LOG_END;
	}
	else {
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
		LOG("Thread: thread not running (tid)");
		LOG((AGENTPP_OPAQUE_PTHREAD_T)tid);
		LOG_END;
	}
#else
#ifdef WIN32
	if (status) {
		if (WaitForSingleObject(threadEndEvent, 
					INFINITE) != WAIT_OBJECT_0) {
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			LOG("Thread: join failed");
			LOG_END;
		}
		status = IDLE;
		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 4);
		LOG("Thread: joined thread successfully");
		LOG_END;
	}
	else {
		LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
		LOG("Thread: thread not running");
		LOG_END;
	}
#endif
#endif
}

void Thread::start()
{
#ifdef POSIX_THREADS
	if (status == IDLE) {
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setstacksize(&attr, stackSize);
		int err = pthread_create(&tid, &attr, thread_starter, this);
		if (err) {
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			LOG("Thread: cannot start thread (error)");
			LOG(err);
			LOG_END;
			status = IDLE;
		}
		else 
			status = RUNNING;
		pthread_attr_destroy(&attr);
	}
	else {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("Thread: thread already running!");
		LOG_END;
	}
#else
#ifdef WIN32
	DWORD *targ = (DWORD*)this;
  
	if (status == IDLE) {

        if(threadHandle != INVALID_HANDLE_VALUE)
		{
			::CloseHandle(threadHandle);
			threadHandle = INVALID_HANDLE_VALUE;
		}

        threadHandle = 
		  CreateThread (0, // no security attributes
				stackSize, 
				(LPTHREAD_START_ROUTINE)thread_starter, 
				targ, 
				0,   
				&tid);   
		
		if (threadHandle == 0) {
			LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
			LOG("Thread: cannot start thread");
			LOG_END;
			status = IDLE;
		}
		else {
			status = RUNNING;
		}
	}
	else {
		LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
		LOG("Thread: thread already running!");
		LOG_END;
	}
#endif
#endif
}

void  Thread::sleep(long millis)
{
#ifdef WIN32
	Sleep (millis);
#else
	nsleep((int)(millis/1000), (millis%1000)*1000000);
#endif
}

void Thread::sleep(long millis, int nanos)
{
#ifdef WIN32
	sleep (millis);
#else
	nsleep((int)(millis/1000), (millis%1000)*1000000 + nanos);
#endif
}

void Thread::nsleep(int secs, long nanos)
{
#ifdef WIN32
	DWORD millis = secs*1000 + nanos/1000000;
	Sleep(millis);
#else
	long s = secs + nanos / 1000000000;
	long n = nanos % 1000000000;

#ifdef _POSIX_TIMERS
	struct timespec interval, remainder;
	interval.tv_sec = (int)s;
	interval.tv_nsec = n;
	if (nanosleep(&interval, &remainder) == -1) {
		if (errno == EINTR) {
			LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
			LOG("Thread: sleep interrupted");
			LOG_END;
		}
	}
#else
	struct timeval interval;
	interval.tv_sec = s;
	interval.tv_usec = n/1000;
	fd_set writefds, readfds, exceptfds;
	FD_ZERO(&writefds);
	FD_ZERO(&readfds);
	FD_ZERO(&exceptfds);
	if (select(0, &writefds, &readfds, &exceptfds, &interval) == -1) {
		if (errno == EINTR) {
			LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
			LOG("Thread: sleep interrupted");
			LOG_END;
		}
	}
#endif
#endif
}

#ifdef AGENTPP_USE_THREAD_POOL

/*--------------------- class TaskManager --------------------------*/

TaskManager::TaskManager(ThreadPool *tp, int stackSize):thread(*this)
{
	threadPool = tp;
	task       = 0;
	go         = TRUE;
	thread.set_stack_size(stackSize);
	thread.start();
	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
	LOG("TaskManager: thread started");
	LOG_END;
}

TaskManager::~TaskManager() {
	lock();
	go = FALSE;
	notify();
	unlock();
	thread.join();
	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
	LOG("TaskManager: thread stopped");
	LOG_END;
}

void TaskManager::run() {
	lock();
	while (go) {
		if (task) {
			task->run();
			delete task;
			task = 0;
			unlock();
			threadPool->idle_notification();
			lock();
		}
		else {
			wait();
		}
	}
    if (task) {
        delete task;
        task = 0;
    }
	unlock();
}

bool TaskManager::set_task(Runnable *t)
{
	lock();
	if (!task) {
		task = t;
		notify();
		unlock();
		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 2);
		LOG("TaskManager: after notify");
		LOG_END;
		return TRUE;
	}
	else {
		unlock();
		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 2);
		LOG("TaskManager: got already a task");
		LOG_END;
		return FALSE;
	}
}

/*--------------------- class ThreadPool --------------------------*/


void ThreadPool::execute(Runnable *t)
{
	lock();
	TaskManager *tm = 0;
	while (!tm) {
		ArrayCursor<TaskManager> cur;
		for (cur.init(&taskList); cur.get(); cur.next()) {
			tm = cur.get();
			if (tm->is_idle()) {
				LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
				LOG("TaskManager: task manager found");
				LOG_END;

				unlock();
				if (tm->set_task(t)) {
				    return;
				}
				else {
				    // task could not be assigned
				    tm = 0;
				    lock();
				}
			}
			tm = 0;
		}
		if (!tm) wait(1000);
	}
	unlock();
}

bool ThreadPool::is_idle() 
{
	lock();
	ArrayCursor<TaskManager> cur;
	for (cur.init(&taskList); cur.get(); cur.next()) {
		if (!cur.get()->is_idle()) {
			unlock();
			return FALSE;
		}
	}
	unlock();
	return TRUE;
}

bool ThreadPool::is_busy() 
{
	lock();
	ArrayCursor<TaskManager> cur;
	for (cur.init(&taskList); cur.get(); cur.next()) {
		if (cur.get()->is_idle()) {
			unlock();
			return FALSE;
		}
	}
	unlock();
	return TRUE;
}


void ThreadPool::terminate() 
{
        lock();
        ArrayCursor<TaskManager> cur;
	for (cur.init(&taskList); cur.get(); cur.next()) {
		cur.get()->stop();
        }
        notify();
        unlock();    
}

ThreadPool::ThreadPool(int size)
{
	for (int i=0; i<size; i++) {
		taskList.add(new TaskManager(this));
	}
}

ThreadPool::ThreadPool(int size, int stack_size)
{
	stackSize = stack_size;
	for (int i=0; i<size; i++) {
		taskList.add(new TaskManager(this, stackSize));
	}
} 

ThreadPool::~ThreadPool()
{
        terminate();
}

/*--------------------- class QueuedThreadPool --------------------------*/

QueuedThreadPool::QueuedThreadPool(int size): ThreadPool(size)
{
}

QueuedThreadPool::QueuedThreadPool(int size, int stack_size):
	ThreadPool(size, stack_size)
{
} 

QueuedThreadPool::~QueuedThreadPool()
{
	go = FALSE;
	join();
}

void QueuedThreadPool::assign(Runnable* t) 
{
	TaskManager *tm = 0;
	ArrayCursor<TaskManager> cur;
	for (cur.init(&taskList); cur.get(); cur.next()) {
		tm = cur.get();
		if (tm->is_idle()) {				
			LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
			LOG("TaskManager: task manager found");
			LOG_END;
			Thread::unlock();
			if (!tm->set_task(t)) {
			    tm = 0;
			    Thread::lock();
			}
			else {
			    Thread::lock();
			    break;
			}
		}
		tm = 0;
	}
	if (!tm) {
		queue.add(t);
		Thread::notify();
	}
}

void QueuedThreadPool::execute(Runnable *t)
{
	Thread::lock();
	assign(t);
	Thread::unlock();
}

void QueuedThreadPool::run() 
{
	go = TRUE;
	Thread::lock();
	while (go) {
		Runnable* t = queue.removeFirst();
		if (t) {
			assign(t);
		}
		Thread::wait(1000);
	}
	Thread::unlock();
}

void QueuedThreadPool::idle_notification() 
{
	Thread::lock();
	Thread::notify();
	Thread::unlock();
	ThreadPool::idle_notification();
}


void MibTask::run()
{
	(task->called_class->*task->method)(task->req);       	
}

#ifdef NO_FAST_MUTEXES

LockRequest::LockRequest(Synchronized* s)
{ 
	target = s; 
	lock();
}

LockRequest::~LockRequest()
{
	unlock();
}
 
LockQueue::LockQueue()
{
	go = TRUE;
	start();
}

LockQueue::~LockQueue()
{
	go = FALSE;
	lock();
	// wake up queue
	notify();
	unlock();

	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
	LOG("LockQueue: end queue");
	LOG_END;

	// join thread here, before pending list is deleted 
	if (is_alive()) join();

	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
	LOG("LockQueue: queue stopped");
	LOG_END;

	pendingRelease.clear();
	pendingLock.clear();
}

void LockQueue::run() 
{
	lock();
	while ((!pendingLock.empty()) || (!pendingRelease.empty()) || (go)) {
		while (!pendingRelease.empty()) {
			LockRequest* r = pendingRelease.removeFirst();
			r->target->unlock();
			r->lock();
			r->notify();
			r->unlock();
		}
		int pl = pendingLock.size();
		int pending = pl;
		for (int i=0; i<pl; i++) {
			LockRequest* r = pendingLock.removeFirst();
                        // Only if target is not locked at all - also not by
                        // this lock queue - then inform requester:
			if (r->target->trylock() == LOCKED) {
				r->lock();
				r->notify();
				r->unlock();
				pending--;
			}
			else 
				pendingLock.addLast(r);
		}
		LOG_BEGIN(loggerModuleName, DEBUG_LOG | 9);
		LOG("LockQueue: waiting for next event (pending)");
		LOG(pending);
		LOG_END;

		// do not wait forever because we cannot 
		// be sure that all instrumentation code notifies
		// us correctly.
		wait(5000);
	}
	unlock();
}

void LockQueue::acquire(LockRequest* r)
{
	lock();
	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 2);
	LOG("LockQueue: adding lock request (ptr)");
	LOG((long)r->target);
	LOG_END;
	pendingLock.addLast(r);
	notify();
	unlock();
}

void LockQueue::release(LockRequest* r)
{
	lock();
	LOG_BEGIN(loggerModuleName, DEBUG_LOG | 2);
	LOG("LockQueue: adding release request (ptr)");
	LOG((long)r->target);
	LOG_END;
	pendingRelease.addLast(r);
	notify();
	unlock();
}


#endif // NO_FAST_MUTEXES

#endif 
#endif // _THREADS

#ifdef AGENTPP_NAMESPACE
}
#endif








