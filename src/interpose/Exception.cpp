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

typedef void  (*CxaThrowFuncPtr)(void *, std::type_info *, void(*)(void *));
typedef void* (*CxaBeginCatchFuncPtr)(void *);

namespace CXX
{
    CxaThrowFuncPtr      throwFunc = NULL;
    CxaBeginCatchFuncPtr catchFunc = NULL;
}

extern "C"
{
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

    void *__cxa_begin_catch(void *exceptionObject) throw()
    {
        if (CXX::catchFunc == NULL) {
            CXX::catchFunc = (CxaBeginCatchFuncPtr)Interpose::loadFuncPtr("__cxa_begin_catch");
        }
        System::clearThreadUnwinding();
        CatchHandler *handler = System::getCatchHandler();
        handler->captureFrames("__cxa_begin_catch");
        CXX::catchFunc(exceptionObject);
    }
}
