#include <core/System.h>
#include <core/Interpose.h>
#include <core/Initializer.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>
#include <sys/syscall.h>

namespace exray {

pthread_once_t             System::keyInitializer   = PTHREAD_ONCE_INIT;
pthread_key_t              System::throwHandlerKey  = 0;
pthread_key_t              System::catchHandlerKey  = 0;
pthread_key_t              System::backtraceKey     = 0;
pthread_key_t              System::unwindingKey     = 0;
bool                       System::initialized      = false;

PthreadMutexLockFuncPtr    System::mutexLockQuiet   = NULL;
PthreadMutexUnlockFuncPtr  System::mutexUnlockQuiet = NULL;

/**
 * Initializer
 */
void System::init()
{
    pthread_once(&keyInitializer, createDestructionKey);
    initialized = true;
}

/**
 * Finalizer
 */
void System::finish()
{
    if (throwHandlerKey != 0) {
        pthread_key_delete(throwHandlerKey);
        throwHandlerKey = 0;
    }
    if (catchHandlerKey != 0) {
        pthread_key_delete(catchHandlerKey);
        catchHandlerKey = 0;
    }
    if (backtraceKey != 0) {
        pthread_key_delete(backtraceKey);
        backtraceKey = 0;
    }
    if (unwindingKey != 0) {
        pthread_key_delete(unwindingKey);
        unwindingKey = 0;
    }
}

/**
 * @return the function pointer to pthread_mutex_lock() provided by the OS.
 * Simply writing pthread_mutex_lock(...) may invoke the interposing code
 * depending on the build configuration, but it may not be ideal.
 * Call this function to directly invoke the OS implementation.
 */
PthreadMutexLockFuncPtr System::getMutexLockFunc()
{
    if (mutexLockQuiet == NULL)
        mutexLockQuiet = (PthreadMutexLockFuncPtr)Interpose::loadFuncPtr("pthread_mutex_lock");
    return mutexLockQuiet;
}

/**
 * @return the function pointer to pthread_mutex_unlock() provided by the OS.
 * Simply writing pthread_mutex_unlock(...) may invoke the interposing code
 * depending on the build configuration, but it may not be ideal.
 * Call this function to directly invoke the OS implementation.
 */
PthreadMutexUnlockFuncPtr System::getMutexUnlockFunc()
{
    if (mutexUnlockQuiet == NULL)
        mutexUnlockQuiet = (PthreadMutexLockFuncPtr)Interpose::loadFuncPtr("pthread_mutex_unlock");
    return mutexUnlockQuiet;
}

/**
 * Called when a thread returns.
 * Dumps the stack frames for the last exception thrown in the thread.
 * if 'exitonly' option is set.
 */
void System::throwHandlerDestructor(void *data)
{
    ThrowHandler *throwHandler = (ThrowHandler *)data;
    if (throwHandler != NULL) {
        throwHandler->dumpFrames("Thread Destructor (Last exception thrown in the thread)");
        delete throwHandler;
    }
}

/**
 * Called when a thread returns.
 * Dumps the stack frames for the last exception caught in the thread.
 * if 'exitonly' option is set.
 */
void System::catchHandlerDestructor(void *data)
{
    CatchHandler *catchHandler = (CatchHandler *)data;
    if (catchHandler != NULL) {
        catchHandler->dumpFrames("Thread Destructor (Last exception caught in the thread)");
        delete catchHandler;
    }
}

void System::backtraceKeyDestructor(void *data)
{
    bool *backtraceKey = (bool *)data;
    if (backtraceKey != NULL) {
        delete backtraceKey;
    }
}

void System::unwindingKeyDestructor(void *data)
{
    bool *unwindingKey = (bool *)data;
    if (unwindingKey != NULL) {
        delete unwindingKey;
    }
}

/**
 * Initialize thread local data and respective destructors.
 *
 * Destructors are the functions specified as the second argument to pthread_key_create().
 * Each destructor is called when a thread returns.
 *
 * throwHandlerDestructor() and catchHandlerDestructor() dumps
 * stack frames for the last exception generated in the thread
 * if 'exitonly' option is set.
 */
void System::createDestructionKey()
{
    if (pthread_key_create(&throwHandlerKey, throwHandlerDestructor) != 0) {
        fprintf(stderr, "libexray.so: pthread_key_create() failed: %s\n", strerror(errno));
    }
    if (pthread_key_create(&catchHandlerKey, catchHandlerDestructor) != 0) {
        fprintf(stderr, "libexray.so: pthread_key_create() failed: %s\n", strerror(errno));
    }
    if (pthread_key_create(&backtraceKey, backtraceKeyDestructor) != 0) {
        fprintf(stderr, "libexray.so: pthread_key_create() failed: %s\n", strerror(errno));
    }
    if (pthread_key_create(&unwindingKey, unwindingKeyDestructor) != 0) {
        fprintf(stderr, "libexray.so: pthread_key_create() failed: %s\n", strerror(errno));
    }
}

/**
 * @return ThrowHandler instance stored in the thread local
 * storage area for the calling thread.
 */
ThrowHandler *System::getThrowHandler()
{
    if (isInitialized() == false)
        exray::exray_init();

    ThrowHandler *throwHandler =
        (ThrowHandler *)pthread_getspecific(throwHandlerKey);

    if (throwHandler == NULL) {
        throwHandler = new ThrowHandler();
        pthread_setspecific(throwHandlerKey, throwHandler);
    }
    return throwHandler;
}

/**
 * @return CatchHandler instance stored in the thread local
 * storage area for the calling thread.
 */
CatchHandler *System::getCatchHandler()
{
    if (isInitialized() == false)
        exray::exray_init();

    CatchHandler *catchHandler =
        (CatchHandler *)pthread_getspecific(catchHandlerKey);

    if (catchHandler == NULL) {
        catchHandler = new CatchHandler();
        pthread_setspecific(catchHandlerKey, catchHandler);
    }
    return catchHandler;
}

/**
 * Dump stack frames for the last exception thrown and caught in the main thread
 * if 'exitonly' option is set.
 */
void System::atexitHandler()
{
    if (isInitialized() == false)
        return;

    ThrowHandler *throwHandler =
        (ThrowHandler *)pthread_getspecific(throwHandlerKey);
    if (throwHandler != NULL) {
        throwHandler->dumpFrames("atexit() (Last exception thrown in the thread)");
    }

    CatchHandler *catchHandler =
        (CatchHandler *)pthread_getspecific(catchHandlerKey);
    if (catchHandler != NULL) {
        catchHandler->dumpFrames("atexit() (Last exception caught in the thread)");
    }
}

pid_t System::gettid()
{
    return syscall(SYS_gettid); // Linux
}

/**
 * Whether the current thread is unwinding due to an exception.
 */
bool System::isThreadUnwinding()
{
    bool *threadUnwinding = getThreadUnwinding();
    return *threadUnwinding;
}

void System::setThreadUnwinding()
{
    bool *threadUnwinding = getThreadUnwinding();
    *threadUnwinding = true;
}

void System::clearThreadUnwinding()
{
    bool *threadUnwinding = getThreadUnwinding();
    *threadUnwinding = false;
}

bool *System::getThreadUnwinding()
{
    bool *threadUnwinding = (bool *)pthread_getspecific(unwindingKey);
    if (threadUnwinding == NULL) {
        threadUnwinding = new bool;
        *threadUnwinding = false;
        pthread_setspecific(unwindingKey, threadUnwinding);
    }
    return threadUnwinding;
}

/**
 *  Whether current call stack has backtrace() or backtrace_symbols().
 */
bool System::threadInBacktrace()
{
    bool *threadInBacktrace = getBacktraceFlag();
    return *threadInBacktrace;
}

void System::setBacktraceFlag()
{
    bool *threadInBacktrace = getBacktraceFlag();
    *threadInBacktrace = true;
}

void System::clearBacktraceFlag()
{
    bool *threadInBacktrace = getBacktraceFlag();
    *threadInBacktrace = false;
}

bool *System::getBacktraceFlag()
{
    bool *threadInBacktrace =
        (bool *)pthread_getspecific(backtraceKey);
    if (threadInBacktrace == NULL) {
        threadInBacktrace = new bool; 
        *threadInBacktrace = false;
       pthread_setspecific(backtraceKey, threadInBacktrace);
    }
    return threadInBacktrace;
}

} // namespace exray
