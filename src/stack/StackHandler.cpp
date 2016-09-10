//
// Use 'addr2line -e program ADDR' in order to decode addresses in stack frames.
// Use 'c++filt -t SYMBOL' in order to decode C++ mangled symbols like "Ss".
//

#include <stack/StackHandler.h>
#include <core/Options.h>
#include <core/System.h>
#include <core/Initializer.h>
#include <core/Logger.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <strings.h>
#include <execinfo.h>

#define THREAD_ID_LENGTH 20

using namespace exray;

pthread_mutex_t     StackHandler::dumpLock   = PTHREAD_MUTEX_INITIALIZER;
pthread_mutexattr_t StackHandler::lockAttr;
bool                StackHandler::active     = false;
bool                StackHandler::exitCalled = false;

void StackHandler::init()
{
    pthread_mutexattr_init(&lockAttr);
    pthread_mutexattr_settype(&lockAttr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&dumpLock, &lockAttr);
    StackHandler::active  = true;
}

void StackHandler::finish()
{
    active = false;
    pthread_mutex_destroy(&dumpLock);
}

void StackHandler::setExitCalled()
{
    exitCalled = true;
}

StackHandler::StackHandler()
{
    gettimeofday(&timestamp, NULL);
    traceStrings = NULL;
    snprintf(threadID, THREAD_ID_LENGTH, "%d[%d] ", getpid(), System::gettid());
}

StackHandler::~StackHandler()
{
}

// Ignore the first 2 frames that belong to this library
#define SKIP_FRAMES 2

void StackHandler::dumpVerboseFrames(int depth, char *traceStrings[])
{
    if (depth <= SKIP_FRAMES || traceStrings == NULL)
        return;

    int currentFrame = SKIP_FRAMES;
    depth -= SKIP_FRAMES;

    Logger::printf("%sStack Frames\n", threadID);

    int loop = (Options::maxFrames < depth) ? Options::maxFrames : depth;
    for (int i = 1; i <= loop; i++, currentFrame++) {
        Logger::printf("%s#%d: %s\n", threadID, i, traceStrings[currentFrame]);
    }
}

void StackHandler::captureFrames()
{
    // avoid deadlock/infinite loop
    if (System::threadInBacktrace() || System::isThreadUnwinding())
        return;

    System::setBacktraceFlag();

    gettimeofday(&this->timestamp, NULL);
    this->frameCount = backtrace(this->frames, STACKSIZE);
    if (this->traceStrings != NULL)
        free(this->traceStrings);
    this->traceStrings = backtrace_symbols(this->frames, this->frameCount);

    System::clearBacktraceFlag();
}
