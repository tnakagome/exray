#ifndef __EXCEPTION_HANDLER__
#define __EXCEPTION_HANDLER__

#include <stack/StackHandler.h>

#define EXCEPTION_TYPE_LENGTH 100

namespace exray {

class ExceptionHandler : public StackHandler
{
public:
                             ExceptionHandler();
protected:
            bool             demangleException     (char **);
            bool             filterExceptions      ();

            char             exceptionType[EXCEPTION_TYPE_LENGTH];
};

} // namespace exray

#endif
