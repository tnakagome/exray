//
// C++ exception wrappers
//
//   These will allow you to detect where an exception is thrown,
//   and where it is caught.
//

#include <core/System.h>
#include <core/Interpose.h>
#include <stack/ThrowHandler.h>
#include <stack/CatchHandler.h>

#include <cxxabi.h>
#include <exception>

using namespace exray;

typedef void  (*CxaThrowFuncPtr)(void *, std::type_info *, void(*)(void *));
typedef void* (*CxaBeginCatchFuncPtr)(void *);

namespace exray {
    namespace CXX {
        CxaThrowFuncPtr      throwFunc = NULL;
        CxaBeginCatchFuncPtr catchFunc = NULL;
    }
}

extern "C"
{
    void __cxa_throw(void *thrownException,
                     std::type_info *tinfo,
                     void (*dest) (void *))
    {
        if (exray::CXX::throwFunc == NULL) {
            exray::CXX::throwFunc = (CxaThrowFuncPtr)exray::Interpose::loadFuncPtr("__cxa_throw");
        }
        if (System::isThreadUnwinding() == false) {
            ThrowHandler *handler = System::getThrowHandler();
            handler->captureFrames(*tinfo, "__cxa_throw");
            System::setThreadUnwinding();
        }
        exray::CXX::throwFunc(thrownException, tinfo, dest);
    }

    void *__cxa_begin_catch(void *exceptionObject) throw()
    {
        if (exray::CXX::catchFunc == NULL) {
            exray::CXX::catchFunc = (CxaBeginCatchFuncPtr)exray::Interpose::loadFuncPtr("__cxa_begin_catch");
        }
        System::clearThreadUnwinding();
        CatchHandler *handler = System::getCatchHandler();
        handler->captureFrames("__cxa_begin_catch");
        return exray::CXX::catchFunc(exceptionObject);
    }
}

#if defined(CPPMODE) && (CPPMODE == CPP11)
typedef void  (*CxaRethrowExceptionFuncPtr)(std::__exception_ptr::exception_ptr);

namespace exray {
    namespace CXX {
        CxaRethrowExceptionFuncPtr rethrowExceptionFunc __attribute__ ((__noreturn__)) = NULL;
    }
}

namespace std
{
    static const char *RETHROW_FUNC_MANGLED = "_ZSt17rethrow_exceptionNSt15__exception_ptr13exception_ptrE";

    void rethrow_exception(__exception_ptr::exception_ptr p)
    {
        if (CXX::rethrowExceptionFunc == NULL) {
            CXX::rethrowExceptionFunc =
                (CxaRethrowExceptionFuncPtr)exray::Interpose::loadFuncPtr(RETHROW_FUNC_MANGLED);
        }
        if (System::isThreadUnwinding() == false) {
            ThrowHandler *handler = System::getThrowHandler();
            handler->captureFrames(*p.__cxa_exception_type(), "std::rethrow_exception()");
            System::setThreadUnwinding();
        }
        CXX::rethrowExceptionFunc(p);
    }
}
#endif
