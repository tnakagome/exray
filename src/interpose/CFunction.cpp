//
// C function wrappers
//

#include <core/Interpose.h>
#include <core/System.h>
#include <stack/StackHandler.h>
#include <stack/ThrowHandler.h>
#include <stack/CatchHandler.h>
#include <stack/CallStackHandler.h>

using namespace exray;

typedef void  (*ExitFuncPtr)(int);
typedef int   (*OpenFuncPtr)(const char *, int, int);
typedef void* (*DlopenFuncPtr)(const char *, int);
typedef int   (*DlcloseFuncPtr)(void *);

namespace exray {
    namespace CFunc {
        ExitFuncPtr     exit __attribute__ ((__noreturn__)) = NULL;
        OpenFuncPtr     open      = NULL;
        DlopenFuncPtr   dlopen    = NULL;
        DlcloseFuncPtr  dlclose   = NULL;
    }
}

extern "C"
{
    void exit(int status)
    {
        if (CFunc::exit == NULL) {
            CFunc::exit = (ExitFuncPtr)Interpose::loadFuncPtr("exit");
        }

        StackHandler::setExitCalled();

        ThrowHandler *throwHandler = System::getThrowHandler();
        if (throwHandler != NULL)
            throwHandler->dumpFrames("exit() (Last exception thrown in the thread)");

        CatchHandler *catchHandler = System::getCatchHandler();
        if (catchHandler != NULL)
            catchHandler->dumpFrames("exit() (Last exception caught in the thread)");

        CFunc::exit(status);
    }

    // More examples on how you would add stack dump functionality to C functions.
#if 0
    int open(const char *pathname, int flags, int mode)
    {
        if (CFunc::open == NULL) {
            CFunc::open = (OpenFuncPtr)Interpose::loadFuncPtr("open");
        }
        CallStackHandler handler;
        handler.dumpFrames("open()", pathname);
        return CFunc::open(pathname, flags, mode);
    }

    void *dlopen(const char *filename, int flag)
    {
        if (CFunc::dlopen == NULL) {
            CFunc::dlopen = (DlopenFuncPtr)Interpose::loadFuncPtr("dlopen");
        }
        CallStackHandler handler;
        handler.dumpFrames("dlopen()", filename);
        return CFunc::dlopen(filename, flag);
    }

    int dlclose(void *handle)
    {
        if (CFunc::dlclose == NULL) {
            CFunc::dlclose = (DlcloseFuncPtr)Interpose::loadFuncPtr("dlclose");
        }
        CallStackHandler handler;
        handler.dumpFrames("dlclose()", handle);
    }
#endif
}
