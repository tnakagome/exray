#ifndef __CATCH_HANDLER__
#define __CATCH_HANDLER__

#include <stack/ExceptionHandler.h>

namespace exray {

/**
 * Handles exception stack frames when it is caught.
 * Should be allocated in thread local storage area for each thread.
 * Timing of stack dump is controlled at runtime by an environment variable.
 * It may immediately dump stack, or keeps the latest stack frames
 * until the thread exits then dumps at the end of the thread.
 */
class CatchHandler : public ExceptionHandler {
public:
                             CatchHandler          ();
            void             captureFrames         (const char *);
            void             dumpFrames            (const char *);
            void             dumpVerboseFrames     (const char *);
private:
            void             dumpHeader            (const char *);

            bool             exceptionCaught;
};

} // namespace exray

#endif
