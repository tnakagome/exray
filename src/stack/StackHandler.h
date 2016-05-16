#ifndef __STACK_HANDLER__
#define __STACK_HANDLER__

#include <sys/types.h>
#include <pthread.h>

#define STACKSIZE 100
#define THREAD_ID_LENGTH 20

/**
 * Base construct for call stack handling.
 */
class StackHandler
{
public:
    static  void             init                  ();
    static  void             finish                ();
    static  void             setExitCalled         ();

            void             getTimestamp          (struct timeval &);

protected:
    static  pthread_mutex_t     dumpLock;
    static  pthread_mutexattr_t lockAttr;
    static  bool             active;
    static  bool             exitCalled;

                             StackHandler          ();
                            ~StackHandler          ();

            void             captureFrames         ();
            void             dumpVerboseFrames     (int frameCount, char *traceString[]);

            struct timeval   timestamp;
            char             threadID[THREAD_ID_LENGTH];
            void            *frames[STACKSIZE];
            char           **traceStrings;
            int              frameCount;
};

inline void
StackHandler::getTimestamp(struct timeval &timestamp)
{
    timestamp = this->timestamp;
}
#endif
