//
// C function wrappers
//

#include <core/Interpose.h>
#include <core/System.h>
#include <core/Options.h>
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
        if (exray::CFunc::exit == NULL) {
            exray::CFunc::exit = (ExitFuncPtr)exray::Interpose::loadFuncPtr("exit");
        }

        StackHandler::setExitCalled();

        ThrowHandler *throwHandler = System::getThrowHandler();
        if (throwHandler != NULL)
            throwHandler->dumpFrames("exit() (Last exception thrown in the thread)");

        CatchHandler *catchHandler = System::getCatchHandler();
        if (catchHandler != NULL)
            catchHandler->dumpFrames("exit() (Last exception caught in the thread)");

        exray::CFunc::exit(status);
    }

#if 0
    int open(const char *pathname, int flags, int mode)
    {
        if (exray::CFunc::open == NULL) {
            exray::CFunc::open = (OpenFuncPtr)exray::Interpose::loadFuncPtr("open");
        }
        CallStackHandler handler;
        handler.dumpFrames("open()", pathname);
        return exray::CFunc::open(pathname, flags, mode);
    }

    void *dlopen(const char *filename, int flag)
    {
        if (exray::CFunc::dlopen == NULL) {
            exray::CFunc::dlopen = (DlopenFuncPtr)exray::Interpose::loadFuncPtr("dlopen");
        }
        CallStackHandler handler;
        handler.dumpFrames("dlopen()", filename);
        return exray::CFunc::dlopen(filename, flag);
    }

    int dlclose(void *handle)
    {
        if (exray::CFunc::dlclose == NULL) {
            exray::CFunc::dlclose = (DlcloseFuncPtr)exray::Interpose::loadFuncPtr("dlclose");
        }
        CallStackHandler handler;
        handler.dumpFrames("dlclose()", handle);
        return exray::CFunc::dlclose(handle);
    }
#endif
}
