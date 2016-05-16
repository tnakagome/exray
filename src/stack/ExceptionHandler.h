#ifndef __EXCEPTION_HANDLER__
#define __EXCEPTION_HANDLER__

#include <stack/StackHandler.h>
#include <list>
#include <string>

#define EXCEPTION_TYPE_LENGTH 100

class ExceptionHandler : public StackHandler
{
public:
                             ExceptionHandler();
protected:
            bool             demangleException     (char **);
            bool             filterExceptions      ();

            char             exceptionType[EXCEPTION_TYPE_LENGTH];
};

#endif
