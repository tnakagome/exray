//
// C++ exception wrappers
//
// Reference for __cxa_throw() and __cxa_begin_catch()
//
//   https://itanium-cxx-abi.github.io/cxx-abi/abi-eh.html
//
// Interposing these functions will allow you to detect
// where an exception is thrown and where it is caught,
// and to dump stack traces from respective functions.
//

#include <core/System.h>
#include <core/Interpose.h>
#include <stack/ThrowHandler.h>
#include <stack/CatchHandler.h>

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
    /**
     * The compiler will translate "throw" statement into this function call.
     * libexray.so interposes this function, adds stack dump functionality to the function,
     * and calls the original __cxa_throw() defined in the OS library.
     */
    void __cxa_throw(void *thrownException,
                     std::type_info *tinfo,
                     void (*dest) (void *))
    {
        if (CXX::throwFunc == NULL) {
            CXX::throwFunc = (CxaThrowFuncPtr)Interpose::loadFuncPtr("__cxa_throw");
        }
        if (System::isThreadUnwinding() == false) {
            ThrowHandler *handler = System::getThrowHandler();
            handler->captureFrames(*tinfo, "__cxa_throw");
            System::setThreadUnwinding();
        }
        CXX::throwFunc(thrownException, tinfo, dest);
    }

    /**
     * The compiler will translate "catch()" statement into __cxa_begin_catch() and
     * __cxa_end_catch().
     * libexray.so interposes __cxa_begin_catch() function, adds stack dump functionality
     * to the function, and calls the original __cxa_begin_catch() defined in the OS library.
     */
    void *__cxa_begin_catch(void *exceptionObject) throw()
    {
        if (CXX::catchFunc == NULL) {
            CXX::catchFunc = (CxaBeginCatchFuncPtr)Interpose::loadFuncPtr("__cxa_begin_catch");
        }
        System::clearThreadUnwinding();
        CatchHandler *handler = System::getCatchHandler();
        handler->captureFrames("__cxa_begin_catch");
        return CXX::catchFunc(exceptionObject);
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

    /**
     * Adds stack dump functionality to std::rethrow_exception()
     */
    void rethrow_exception(__exception_ptr::exception_ptr p)
    {
        if (CXX::rethrowExceptionFunc == NULL) {
            CXX::rethrowExceptionFunc =
                (CxaRethrowExceptionFuncPtr)Interpose::loadFuncPtr(RETHROW_FUNC_MANGLED);
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
