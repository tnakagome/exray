//
// pthread function wrappers
//

#include <core/System.h>
#include <core/Interpose.h>
#include <core/Options.h>
#include <stack/ThrowHandler.h>
#include <stack/CatchHandler.h>
#include <stack/CallStackHandler.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

using namespace exray;

namespace PThread
{
    // pthread function pointer types
    typedef int (*CancelFuncPtr)(pthread_t);
    typedef int (*CreateFuncPtr)(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *);

    // funcion pointers for OS pthread functions
    CancelFuncPtr cancel = NULL;
    CreateFuncPtr create = NULL;

    namespace Mutex
    {
        // mutex function pointer types
        typedef int (*InitFuncPtr)(pthread_mutex_t *, const pthread_mutexattr_t*);
        typedef int (*LockFuncPtr)(pthread_mutex_t *);
        typedef int (*TryLockFuncPtr)(pthread_mutex_t *);
        typedef int (*UnlockFuncPtr)(pthread_mutex_t *);
        typedef int (*DestroyFuncPtr)(pthread_mutex_t *);

        // function pointers for OS mutex functions
        InitFuncPtr    init    = NULL;
        LockFuncPtr    lock    = NULL;
        TryLockFuncPtr trylock = NULL;
        UnlockFuncPtr  unlock  = NULL;
        DestroyFuncPtr destroy = NULL;
    }
}

//
// Special data structure used with pthread_create()
// This does not do anything by default. Customize if necessary.
//
namespace thunk
{
    struct parameters {
        void *(*userFunc)(void *);
        void *userArgs;
    };

    static void *entry(void *args)
    {
        struct parameters *param = (struct parameters *)args;

        //
        // Do things here in order to add features to pthread_create()
        //

        void *result = param->userFunc(param->userArgs);
        free(param);
        return result;
    }
}

extern "C"
{
    int pthread_create(pthread_t *tid,
                       const pthread_attr_t *attr,
                       void *(*startRoutine)(void*),
                       void *args)
    {
        struct thunk::parameters *param;
        if (PThread::create == NULL) {
            PThread::create = (PThread::CreateFuncPtr)
                Interpose::loadFuncPtr("pthread_create");
        }

        param = (struct thunk::parameters *)malloc(sizeof(struct thunk::parameters));
        if (param == NULL) {
            return EAGAIN;
        }
        param->userFunc = startRoutine;
        param->userArgs = args;
        return PThread::create(tid, attr, thunk::entry, param);
    }

    int pthread_cancel(pthread_t tid)
    {
        if (PThread::cancel == NULL) {
            PThread::cancel = (PThread::CancelFuncPtr)
                Interpose::loadFuncPtr("pthread_cancel");
        }

        if (Options::pthreadDump) {
            ThrowHandler *throwHandler = System::getThrowHandler();
            throwHandler->dumpFrames("exit() (Last exception thrown in the thread)");

            CatchHandler *catchHandler = System::getCatchHandler();
            catchHandler->dumpFrames("exit() (Last exception caught in the thread)");
        }

        return PThread::cancel(tid);
    }

    int pthread_mutex_init(pthread_mutex_t *__restrict mutex,
                           const pthread_mutexattr_t *__restrict attr)
    {
        if (PThread::Mutex::init == NULL) {
            PThread::Mutex::init = (PThread::Mutex::InitFuncPtr)
                Interpose::loadFuncPtr("pthread_mutex_init");
        }
        int result = PThread::Mutex::init(mutex, attr);

        if (Options::pthreadDump && System::isThreadUnwinding() == false) {
            CallStackHandler handler;
            handler.dumpFrames("pthread_mutex_init()", (void *)mutex);
        }

        return result;
    }

    int pthread_mutex_lock(pthread_mutex_t *mutex)
    {
        if (PThread::Mutex::lock == NULL) {
            PThread::Mutex::lock = (PThread::Mutex::LockFuncPtr)
                Interpose::loadFuncPtr("pthread_mutex_lock");
        }
        int result = PThread::Mutex::lock(mutex);

        if (Options::pthreadDump && System::isThreadUnwinding() == false) {
            CallStackHandler handler;
            handler.dumpFrames("pthread_mutex_lock()", (void *)mutex);
        }
        return result;
    }

    int pthread_mutex_trylock(pthread_mutex_t *mutex)
    {
        if (PThread::Mutex::trylock == NULL) {
            PThread::Mutex::trylock = (PThread::Mutex::LockFuncPtr)
                Interpose::loadFuncPtr("pthread_mutex_trylock");
        }
        int result = PThread::Mutex::trylock(mutex);

        if (Options::pthreadDump && System::isThreadUnwinding() == false) {
            CallStackHandler handler;
            handler.dumpFrames("pthread_mutex_trylock()", (void *)mutex);
        }
        return result;
    }

    int pthread_mutex_unlock(pthread_mutex_t *mutex)
    {
        if (PThread::Mutex::unlock == NULL) {
            PThread::Mutex::unlock = (PThread::Mutex::UnlockFuncPtr)
                Interpose::loadFuncPtr("pthread_mutex_unlock");
        }
        int result = PThread::Mutex::unlock(mutex);

        if (Options::pthreadDump && System::isThreadUnwinding() == false) {
            CallStackHandler handler;
            handler.dumpFrames("pthread_mutex_unlock()", (void *)mutex);
        }
        return result;
    }

    int pthread_mutex_destroy(pthread_mutex_t *mutex)
    {
        if (PThread::Mutex::destroy == NULL) {
            PThread::Mutex::destroy = (PThread::Mutex::DestroyFuncPtr)
                Interpose::loadFuncPtr("pthread_mutex_destroy");
        }
        int result = PThread::Mutex::destroy(mutex);

        if (Options::pthreadDump && System::isThreadUnwinding() == false) {
            CallStackHandler handler;
            handler.dumpFrames("pthread_mutex_destroy()", (void *)mutex);
        }
        return result;
    }
}
