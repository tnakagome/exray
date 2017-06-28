#ifndef __STACK_HANDLER__
#define __STACK_HANDLER__

#include <sys/types.h>
#include <pthread.h>
#include <regex.h>

#include <string>

#define STACKSIZE 100
#define THREAD_ID_LENGTH 20
#define REGEX_BUFFER 4

namespace exray {

/**
 * Base construct for call stack handling.
 */
class StackHandler
{
public:
    static  void                  init                  ();
    static  void                  finish                ();
    static  void                  setExitCalled         ();

            void                  getTimestamp          (struct timeval &);

protected:
    static  pthread_mutex_t       dumpLock;
    static  pthread_mutexattr_t   lockAttr;
    static  bool                  active;
    static  bool                  exitCalled;

                                  StackHandler          ();
                                 ~StackHandler          ();

            void                  captureFrames         ();
            void                  dumpVerboseFrames     ();

            struct timeval        timestamp;
            char                  threadID[THREAD_ID_LENGTH];
            void                 *frames[STACKSIZE];
            char                **traceStrings;
            int                   frameCount;

private:
    static  regex_t               framePattern;

            void                  dumpMangled           (int frameCount);
            void                  dumpDemangled         (int frameCount);
            std::string           demangleFrame         (char *frame);

            regmatch_t            pmatch[REGEX_BUFFER]; // frame pattern is split into 3 parts + entire match.
};

inline void
StackHandler::getTimestamp(struct timeval &timestamp)
{
    timestamp = this->timestamp;
}

} // namespace exray

#endif
