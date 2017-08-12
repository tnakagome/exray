#ifndef __SYSTEM__
#define __SYSTEM__

#include <stack/ThrowHandler.h>
#include <stack/CatchHandler.h>
#include <pthread.h>

namespace exray {

typedef int (*PthreadMutexLockFuncPtr)(pthread_mutex_t *);
typedef int (*PthreadMutexUnlockFuncPtr)(pthread_mutex_t *);

/**
 * - Sets up and handles thread local storage.
 *
 * - Provides interface to pthread_mutex_lock and unlock
 *   in order to avoid infinite loop with interposing functions.
 *
 * - Manages thread unwinding state and thread backtrace state.
 */
class System
{
public:
     static void                       init();
     static void                       finish();
     static void                       atexitHandler();
     static pid_t                      gettid();

     static ThrowHandler              *getThrowHandler();
     static CatchHandler              *getCatchHandler();

     static PthreadMutexLockFuncPtr    getMutexLockFunc();
     static PthreadMutexUnlockFuncPtr  getMutexUnlockFunc();

     static bool                       threadInBacktrace();
     static void                       setBacktraceFlag();
     static void                       clearBacktraceFlag();

     static bool                       isThreadUnwinding();
     static void                       setThreadUnwinding();
     static void                       clearThreadUnwinding();

     static inline bool                isInitialized() { return initialized; }

private:
     static void                       throwHandlerDestructor(void *);
     static void                       catchHandlerDestructor(void *);
     static void                       backtraceKeyDestructor(void *);
     static void                       unwindingKeyDestructor(void *);
     static void                       createDestructionKey();
     static bool                      *getBacktraceFlag();
     static bool                      *getThreadUnwinding();

     static pthread_once_t             keyInitializer;
     static pthread_key_t              throwHandlerKey;
     static pthread_key_t              catchHandlerKey;
     static pthread_key_t              backtraceKey;
     static pthread_key_t              unwindingKey;
     static bool                       initialized;

     // function pointers for mutex locker/unlocker
     static PthreadMutexLockFuncPtr    mutexLockQuiet;
     static PthreadMutexUnlockFuncPtr  mutexUnlockQuiet;

};

} // namespace exray

#endif // __SYSTEM__
