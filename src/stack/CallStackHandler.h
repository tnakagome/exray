#ifndef __CALL_STACK_HANDLER__
#define __CALL_STACK_HANDLER__

#include <stack/StackHandler.h>

namespace exray {

/**
 * Dump stack frames from an arbitrary location.
 */
class CallStackHandler : public StackHandler {
public:
                             CallStackHandler      ();
            void             dumpHeader            (const char *origin);
            void             dumpFrames            (const char *origin, void *object);
            void             dumpFrames            (const char *origin, const char *string);
};

} // namespace exray

#endif
