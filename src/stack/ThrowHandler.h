#ifndef __THROW_HANDLER__
#define __THROW_HANDLER__

#include <stack/ExceptionHandler.h>
#include <typeinfo>

namespace exray {

/**
 * Handles exception stack frames when it is thrown.
 * Should be allocated in thread local storage area for each thread.
 * Timing of stack dump is controlled at runtime by an environment variable.
 * It may immediately dump stack, or keeps the latest stack frames
 * until the thread exits then dumps at the end of the thread.
 */
class ThrowHandler : public ExceptionHandler {
public:
                             ThrowHandler          ();
            void             captureFrames         (const std::type_info &, const char *);
            void             dumpFrames            (const char *);
            void             dumpVerboseFrames     (const char *);
private:
            void             dumpHeader            (const char *);

            bool             exceptionThrown;
};

} // namespace exray

#endif
