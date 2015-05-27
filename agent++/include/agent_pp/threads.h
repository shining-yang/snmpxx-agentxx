/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - threads.h  
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


#ifndef multi_h_
#define multi_h_

#include <agent_pp/agent++.h>

#ifdef _THREADS
#ifdef _WIN32THREADS
#include <winbase.h>
#else
#include <pthread.h>
#endif
#endif

#include <time.h>
#include <sys/types.h>

#include <agent_pp/List.h>


#define MULTI_THREADED	TRUE
#define SINGLE_THREADED FALSE
#define AGENTPP_DEFAULT_STACKSIZE 0x10000

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

class MibEntry;
class Request;
class Mib;


typedef enum { STANDARD_CB, SHADOW_CB } cb_type;

typedef void (Mib::*mib_method_t)(Request*);


// The MibMethodCall class stores function pointers of the class Mib

class AGENTPP_DECL MibMethodCall {

public:
  MibMethodCall(Mib* c, 
		void (Mib::*m) (Request *),
		Request* r): 

	called_class(c), method(m), req(r) {
  }

  MibMethodCall(const MibMethodCall& other) {

	called_class    = other.called_class;
	method		= other.method;
	req		= other.req;
  }
    
  Mib*		called_class;
  mib_method_t  method;	
  Request*	req;
};


void* method_routine_caller(void *);



#define TS_SYNCHRONIZED(x) { ThreadSynchronize _ts_synchronize(*this); x }

#ifdef _THREADS
#ifndef WIN32
#ifndef POSIX_THREADS
#error "This SYSTEM does not support threads. Undefine _THREADS in agent++.h"
#endif
#endif


/**
 * The Runnable interface should be implemented by any class whose 
 * instances are intended to be executed by a thread. The class must 
 * define a method of no arguments called run. 
 *
 * This interface is designed to provide a common protocol for objects 
 * that wish to execute code while they are active. For example, 
 * Runnable is implemented by class Thread. Being active simply means 
 * that a thread has been started and has not yet been stopped. 
 *
 * In addition, Runnable provides the means for a class to be active 
 * while not subclassing Thread. A class that implements Runnable can 
 * run without subclassing Thread by instantiating a Thread instance and
 * passing itself in as the target. In most cases, the Runnable interface 
 * should be used if you are only planning to override the run() method 
 * and no other Thread methods. This is important because classes should
 * not be subclassed unless the programmer intends on modifying or 
 * enhancing the fundamental behavior of the class. 
 * 
 * @author Frank Fock
 * @version 3.5
 */

 class AGENTPP_DECL Runnable {
   
 public:
	Runnable() { }
	virtual ~Runnable()  { }

	/**
	 * When an object implementing interface Runnable is used to 
	 * create a thread, starting the thread causes the object's run 
	 * method to be called in that separately executing thread.
	 */
	virtual void run() = 0;
 };

 /**
  * The Synchronized class implements services for synchronizing
  * access between different threads.
  *
  * @author Frank Fock
  * @version 4.0
  */
class AGENTPP_DECL Synchronized {
public:
        enum TryLockResult { LOCKED = 1, BUSY = 0, OWNED = -1 };

        Synchronized();
	~Synchronized();

	/**
	 * Causes current thread to wait until another thread 
	 * invokes the notify() method or the notifyAll() 
	 * method for this object.
	 */
	void		wait();

	/**
	 * Causes current thread to wait until either another 
	 * thread invokes the notify() method or the notifyAll() 
	 * method for this object, or a specified amount of time 
	 * has elapsed.
	 *
	 * @param timeout 
	 *    timeout in milliseconds.
	 * @param
	 *    return TRUE if timeout occured, FALSE otherwise.
	 */
	bool		wait(unsigned long timeout);    

	/**
	 * Wakes up a single thread that is waiting on this 
	 * object's monitor.
	 */
	void		notify();
	/**
	 * Wakes up all threads that are waiting on this object's 
	 * monitor.
	 */
	void		notify_all();

	/**
	 * Enter a critical section. If this thread owned this
         * lock already, the call succeeds too (returns TRUE), but there
         * will not be recursive locking. Unlocking will always free the lock. 
         * 
	 * @return
	 *    TRUE if the attempt was successful, FALSE otherwise.
	 */
	bool		lock();

	/**
	 * Try to enter a critical section. If this thread owned this
         * lock already, the call succeeds too (returns TRUE), but there
         * will not be recursive locking. Unlocking will always free the lock. 
	 *
	 * @return
	 *     LOCKED if there was no lock before and now the calling thread
         *     owns the lock; BUSY = if another thread owns the lock;
         *     OWNED if the lock is already owned by the calling thread.
	 */
	TryLockResult	trylock();

	/**
	 * Leave a critical section. If this thread called lock or trylock
         * more than once successfully, this call will nevertheless release
         * the lock (non-recursive locking).
         * @return
         *    TRUE if the unlock succeeded, FALSE if there was no lock 
         *    to unlock.
	 */
	bool		unlock();
        

 private:
#ifndef _NO_LOGGING     
        static int      next_id;
        int             id; 
#endif        
#ifdef POSIX_THREADS
	int		cond_timed_wait(const timespec*);
	pthread_cond_t  cond;
	pthread_mutex_t monitor;
#else
#ifdef WIN32
	char            numNotifies;
	HANDLE          semEvent;
	HANDLE          semMutex;
#endif
#endif
	bool		isLocked;
 };

 /**
  * The Lock class implements a synchronization object, that
  * when created enters the critical section of the given 
  * Synchronized object and leaves it when the Lock object is
  * destroyed. The execution of the critical section can be
  * suspended by calling the wait function.
  *
  * @author Frank Fock
  * @version 3.5
  */
 class AGENTPP_DECL Lock {
 public:
	/**
	 * Create a locking object for a Synchronized instance,
	 * which will be locked by calling this constructor.
	 *
	 * @param sync
	 *   a Synchronized instance.
	 */
	Lock (Synchronized &s):sync(s) { sync.lock();   }

	/**
	 * The destructor will release the lock on the sync
	 * object.
	 */
	~Lock()                        { sync.unlock(); }

	
	/**
	 * Causes current thread to wait until either another 
	 * thread invokes the notify() method or the notifyAll() 
	 * method for this object, or a specified amount of time 
	 * has elapsed.
	 *
	 * @param timeout 
	 *    timeout in milliseconds.
	 */
	void		wait (long timeout) 
			{ if (timeout<0) sync.wait(); else sync.wait(timeout);}

	/**
	 * Wakes up a single thread that is waiting on this 
	 * object's monitor.
	 */
	void		notify ()      { sync.notify (); }

 private:
	Synchronized &sync;
 };

 class AGENTPP_DECL ThreadList;

 /**
  * A thread is a thread of execution in a program.
  *
  * There are two ways to create a new thread of execution. One is to 
  * declare a class to be a subclass of Thread. This subclass should 
  * override the run method of class Thread. An instance of the subclass 
  * can then be allocated and started. 
  *
  * The other way to create a thread is to declare a class that 
  * implements the Runnable interface. That class then implements the run 
  * method. An instance of the class can then be allocated, passed as an 
  * argument when creating Thread, and started. 
  * 
  * @author Frank Fock
  * @version 3.5.7
  */
 class AGENTPP_DECL Thread : public Synchronized, public Runnable {

	enum ThreadStatus { IDLE, RUNNING, FINISHED }; 

	friend class Synchronized;
#ifdef WIN32
	friend DWORD thread_starter(LPDWORD lpdwParam);
#else
	friend void* thread_starter(void*);
#endif
 public:
	/**
	 * Create a new thread.
	 */
	Thread();

	/**
	 * Create a new thread which will execute the given Runnable.
	 *
	 * @param runnable
	 *    a Runnable subclass.
	 */
	Thread(Runnable &r);

	/**
	 * Destroy thread. If thread is running or has been finished but
	 * not joined yet, then join it.
	 */
	virtual ~Thread();

	/**
	 * Causes the currently executing thread to sleep (temporarily 
	 * cease execution) for the specified number of milliseconds.
	 *
	 * @param millis
	 *    number of milliseconds to sleep.
	 */
	static  void      sleep(long millis);

	/**
	 * Causes the currently executing thread to sleep (cease 
	 * execution) for the specified number of milliseconds plus 
	 * the specified number of nanoseconds.
	 *
	 * @param millis
	 *    the length of time to sleep in milliseconds.
	 * @param nanos
	 *    0-999999 additional nanoseconds to sleep.
	 */
	static  void      sleep(long millis, int nanos);

	/**
	 * If this thread was constructed using a separate Runnable 
	 * run object, then that Runnable object's run method is called; 
	 * otherwise, this method does nothing and returns. 
	 *
	 * Subclasses of Thread should override this method. 
	 */
	virtual void      run();

	/**
	 * Get the Runnable object used for thread execution.
	 *
	 * @return 
	 *    a Runnable instance which is either the Thread itself
	 *    when created through the default constructor or the
	 *    Runnable object given at creation time.
	 */
	Runnable&	  get_runnable();

	/**
	 * Waits for this thread to die.
	 */
	void              join();

	/**
	 * Causes this thread to begin execution; the system calls the 
	 * run method of this thread.
	 */
	void              start();

	/**
	 * Before calling the start method this method can be used
	 * to change the stack size of the thread.
	 * 
	 * @param stackSize
	 *    the thread's stack size in bytes.
	 */
	void              set_stack_size(long s) { stackSize = s; } 

	/**
	 * Check whether thread is alive.
	 *
	 * @return
	 *    Returns TRUE if the thread is running; otherwise FALSE. 
	 */
	bool		  is_alive() { return (status == RUNNING); }

	/**
	 * Clone this thread. This method must not be called on
	 * running threads.
	 */
	Thread*		clone() { return new Thread(get_runnable()); }
	
 private:
	Runnable*         runnable;
	ThreadStatus      status;
	long              stackSize;
#ifdef POSIX_THREADS
	pthread_t         tid;
#else
#if WIN32
	HANDLE            threadHandle;
	DWORD		  tid;
	HANDLE            threadEndEvent;
#endif
#endif
	static ThreadList threadList; 
	static void       nsleep(int secs, long nanos);
 };


 /**
  * The ThreadList class implements a singleton class that holds
  * a list of all currently running Threads.
  *
  * @author Frank Fock
  * @version 3.5
  */
#if !defined (AGENTPP_DECL_TEMPL_ARRAY_THREAD)
#define AGENTPP_DECL_TEMPL_ARRAY_THREAD
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL Array<Thread>;
#endif

 class AGENTPP_DECL ThreadList : public Synchronized {
 public:
	ThreadList() { }
	~ThreadList() { list.clear(); /* do no delete threads */ }

	void	add(Thread* t)		{ lock(); list.add(t); unlock(); }
	void	remove(Thread* t)	{ lock(); list.remove(t); unlock(); }
	int	size() const		{ return list.size(); }
	Thread* last() { lock(); Thread* t = list.last(); unlock(); return t; }

 protected:   
	Array<Thread> list; 
 };  


 class TaskManager;

#ifdef AGENTPP_USE_THREAD_POOL

#if !defined (AGENTPP_DECL_TEMPL_ARRAY_TASKMANAGER)
#define AGENTPP_DECL_TEMPL_ARRAY_TASKMANAGER
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL Array<TaskManager>;
#endif

 /**
  * The ThreadPool class provides a pool of threads that can be
  * used to perform an arbitrary number of tasks. 
  * 
  * @author Frank Fock
  * @version 3.5.19
  */
 class AGENTPP_DECL ThreadPool : public Synchronized {

 protected:
	Array<TaskManager> taskList;
	int stackSize;
 public:
	/**
	 * Create a ThreadPool with a given number of threads.
	 *
	 * @param size
	 *    the number of threads started for performing tasks.
	 *    The default value is 4 threads.
	 */
	ThreadPool(int size = 4);

	
	/**
	 * Create a ThreadPool with a given number of threads and
	 * stack size.
	 *
	 * @param size
	 *    the number of threads started for performing tasks.
	 *    The default value is 4 threads.
	 * @param stackSize
	 *    the stack size for each thread.
	 */
	ThreadPool(int size, int stackSize);

	/**
	 * Destructor will wait for termination of all threads.
	 */
	virtual ~ThreadPool();

	/**
	 * Execute a task. The task will be deleted after call of
	 * its run() method.
	 */
	virtual void	execute(Runnable*);

	/**
	 * Check whether the ThreadPool is idle or not.
	 *
	 * @return
	 *    TRUE if non of the threads in the pool is currently 
	 *    executing any task.
	 */
	bool is_idle();

	/**
	 * Check whether the ThreadPool is busy (i.e., all threads are 
         * running a task) or not.
	 *
	 * @return
	 *    TRUE if non of the threads in the pool is currently 
	 *    idle (not executing any task).
	 */
	bool is_busy();
        
	/**
	 * Get the size of the thread pool.
	 * @return
	 *    the number of threads in the pool.
	 */
	unsigned int size() { return taskList.size(); }

	/**
	 * Get the stack size.
	 *
	 * @return 
	 *   the stack size of each thread in this thread pool.
	 */
	int	stack_size() { return stackSize; }

	/** 
	 * Notifies the thread pool about an idle thread (synchronized).
	 */
	virtual void	idle_notification() { lock(); notify(); unlock(); }
        
        /**
         * Gracefully stops all running task managers after their current
         * task execution. The ThreadPool cannot be used thereafter and should
         * be destroyed. This call blocks until all threads are stopped.
         */
        void terminate();
 };


 
#if !defined (AGENTPP_DECL_TEMPL_LIST_RUNNABLE)
#define AGENTPP_DECL_TEMPL_LIST_RUNNABLE
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL List<Runnable>;
#endif

 /**
  * The QueuedThreadPool class provides a pool of threads that can be
  * used to perform an arbitrary number of tasks. If a task is added
  * and there is currently no idle thread available to perform the task,
  * then the task will be queued for later processing. Consequently,
  * the execute method never blocks (in contrast to ThreadPool). 
  *
  * The QueuedThreadPool uses an extra Thread to process queued messages
  * asynchronously.
  * 
  * @author Frank Fock
  * @version 3.5.18
  */
 class AGENTPP_DECL QueuedThreadPool : public ThreadPool, public Thread {

	List<Runnable>     queue;
	bool		   go;

 public:
	/**
	 * Create a ThreadPool with a given number of threads.
	 *
	 * @param size
	 *    the number of threads started for performing tasks.
	 *    The default value is 4 threads.
	 */
	QueuedThreadPool(int size = 4);

	
	/**
	 * Create a ThreadPool with a given number of threads and
	 * stack size.
	 *
	 * @param size
	 *    the number of threads started for performing tasks.
	 *    The default value is 4 threads.
	 * @param stackSize
	 *    the stack size for each thread.
	 */
	QueuedThreadPool(int size, int stackSize);

	/**
	 * Destructor will wait for termination of all threads.
	 */
	virtual ~QueuedThreadPool();

	/**
	 * Execute a task. The task will be deleted after call of
	 * its run() method.
	 */
	void	execute(Runnable*);

	/**
	 * Gets the current number of queued tasks.
	 *
	 * @return 
	 *    the number of tasks that are currently queued.
	 */
	unsigned int queue_length() { return queue.size(); }


	/**
	 * Runs the queue processing loop.
	 */
	void	run();

	/**
	 * Stop queue processing.
	 */
	void	stop()		{ go = FALSE; }

	/** 
	 * Notifies the thread pool about an idle thread.
	 */
	virtual void	idle_notification();

 private:
	void	assign(Runnable* task);
 };


 /**
  * The TaskManager class controls the execution of tasks on
  * a Thread of a ThreadPool.
  *
  * @author Frank Fock
  * @version 3.5.19
  */
 class AGENTPP_DECL TaskManager : public Synchronized, public Runnable {
 public:
	/**
	 * Create a TaskManager and insert the created thread
	 * into the given ThreadPool.
	 *
	 * @param threadPool
	 *    a pointer to a ThreadPool instance.
	 * @param stackSize
	 *    the stack size for the managed thread.
	 */
	TaskManager(ThreadPool*, 
		    int stackSize = AGENTPP_DEFAULT_STACKSIZE);

	/**
	 * Destructor will wait for thread to terminate.
	 */
	virtual ~TaskManager();

	
	/**
	 * Check whether this thread is idle or not.
	 *
	 * @return
	 *   TRUE if the thread managed by this TaskManager does
	 *   not currently execute any task; FALSE otherwise.
	 */
	bool is_idle()	{ return (!task); }

	/**
	 * Start thread execution.
	 */
	void	start()		{ thread.start(); }

	/**
	 * Stop thread execution after having finished current task.
	 */
	void	stop()		{ go = FALSE; }

	/**
	 * Set the next task for execution. This will block until
	 * current task has finished execution.
	 *
	 * @param task
	 *   a Runnable instance.
	 * @return
	 *   TRUE if the task could be assigned successfully and 
	 *   FALSE if another thread has assigned a task concurrently.
	 *   In the latter case, the task has not been assigned!
	 */
	bool  set_task(Runnable*);

	/**
	 * Clone this TaskManager.
	 */
	TaskManager*	clone() 
	    { return new TaskManager(
		new ThreadPool(threadPool->size(), threadPool->stack_size()));}

 protected:
	Thread		thread;
	ThreadPool*	threadPool;
	Runnable*      	task;
	void		run();
	bool		go;
 };

 #endif // AGENTPP_USE_THREAD_POOL

 /**
  * The MibTask class implements a task for processing SNMP requests
  * using AGENT++.
  *
  * @author Frank Fock
  * @version 3.5
  */ 
 class AGENTPP_DECL MibTask: public Runnable {
 public:
	MibTask(MibMethodCall* call) { task = call; }
	virtual ~MibTask() { delete task; }
	
	virtual void run();
 protected:
	MibMethodCall* task;
 };

#ifdef NO_FAST_MUTEXES 

 /**
  * The LockRequest class represents a lock or a unlock action
  * on a mutex. A LockRequest locks itself when it is created 
  * and releases that lock when it is deleted.
  *
  * @author Frank Fock
  * @version 3.5.3
  */
 class AGENTPP_DECL LockRequest: public Synchronized {
 public:
	/**
	 * Create a LockRequest to lock or unlock a Synchronized instance.
	 *
	 * @param mutex
	 *    a pointer to a Synchronized instance that should be locked or
	 *    unlocked according to the specified action.
	 */
	LockRequest(Synchronized*);
	~LockRequest();

	Synchronized*	target;
 };

 /**
  * The LockQueue class implements a thread that is responsible for locking
  * and unlocking mutexes. With this LockQueue mutexes can be unlocked by 
  * threads that do not own the mutex. In addition, mutexes acquired from a
  * thread are not automatically released when that thread ends. As a
  * consequence, a LockQueue object can be used to simulate FAST POSIX
  * mutex behavior on systems that do not support FAST mutexes.
  *
  * @author Frank Fock
  * @version 3.5.3
  */
#if !defined (AGENTPP_DECL_TEMPL_LIST_LOCKREQUEST)
#define AGENTPP_DECL_TEMPL_LIST_LOCKREQUEST
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL List<LockRequest>;
#endif

 class AGENTPP_DECL LockQueue: public Thread {
 public:
	LockQueue();
	virtual ~LockQueue();
	virtual void run();

	/**
	 * Lock a Synchronized instance.
	 *
	 * @param
	 *    a pointer to LockRequest. The pointer will not be deleted and
	 *    must remain valid memory until a LockQueue.wait() called 
	 *    hereafter returns.
	 */
	void acquire(LockRequest*);

	/**
	 * Unlock a Synchronized instance.
	 *
	 * @param
	 *    a pointer to LockRequest. The pointer will not be deleted and
	 *    must remain valid memory until a LockQueue.wait() called 
	 *    hereafter returns.
	 */
	void release(LockRequest*);

 protected:
	List<LockRequest> pendingLock;
	List<LockRequest> pendingRelease;
	bool		  go;
 };
#endif

#endif
 
/**
 * The ThreadManager class provides functionality to control the
 * execution of threads.
 *
 * @author Frank Fock
 * @version 3.5.3
 */ 

#ifdef _THREADS
class AGENTPP_DECL ThreadManager: public Synchronized {
#else
class AGENTPP_DECL ThreadManager {
#endif

public:

	/**
	 * Default constructor
	 */
	ThreadManager();

	/**
	 * Destructor
	 */
	virtual ~ThreadManager();
	
	/**
	 * Start synchronized execution.
	 */
	void		start_synch();
	/**
	 * End synchronized execution.
	 */
	void		end_synch();

	/**
	 * Start global synchronized execution.
	 */
	static	void	start_global_synch();
	/**
	 * End global synchronized execution.
	 */
	static  void	end_global_synch();

private:
#ifdef _THREADS
	static Synchronized global_lock;
#endif
};


class AGENTPP_DECL ThreadSynchronize {
 public:
	ThreadSynchronize(ThreadManager&);
	~ThreadSynchronize();
 protected:
	ThreadManager& s;
};

class AGENTPP_DECL SingleThreadObject: public ThreadManager 
{
public:
	SingleThreadObject();
	virtual ~SingleThreadObject();
};


#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
