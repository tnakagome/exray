//
// Use 'addr2line -e program ADDR' in order to decode addresses in stack frames.
// Use 'c++filt -t SYMBOL' in order to decode C++ mangled symbols like "Ss".
//

#include <stack/StackHandler.h>
#include <core/Options.h>
#include <core/System.h>
#include <core/Logger.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <execinfo.h>
#include <cxxabi.h>

#include <string>

#define THREAD_ID_LENGTH 20

using namespace exray;

pthread_mutex_t     StackHandler::dumpLock      = PTHREAD_MUTEX_INITIALIZER;
pthread_mutexattr_t StackHandler::lockAttr;
bool                StackHandler::active        = false;
bool                StackHandler::exitCalled    = false;
regex_t             StackHandler::framePattern;

/*
 * Regex pattern for a stack frame from backtrace_symbols()
 * used to extract the function name from a frame.
 *
 * Example frame string:
 * /usr/lib64/libreoffice/program/libucbhelper.so(_ZN9ucbhelper7Content16getPropertyValueERKN3rtl8OUStringE+0xa6) [0x7feb400bedf6]
 *
 * Match groups
 * 0 1         2        3
 * ^([^(]+)\\(([^\\+]*)(\\+.*)$
 *
 * 0: Entire string
 * 1: Shared library or program name
 * 2: Function name (before the + sign), which may or may not be mangled
 * 3: The rest
 */
static const char *FRAME_PATTERN_REGEX = "^([^(]+)\\(([^\\+]*)(\\+.*)$";

void StackHandler::init()
{
    pthread_mutexattr_init(&lockAttr);
    pthread_mutexattr_settype(&lockAttr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&dumpLock, &lockAttr);
    regcomp(&StackHandler::framePattern, FRAME_PATTERN_REGEX, REG_EXTENDED);
    StackHandler::active = true;
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
    snprintf(threadID, THREAD_ID_LENGTH, "%d[%d] ", getpid(), System::gettid());
    this->traceStrings = NULL;
    this->frameCount   = 0;
}

StackHandler::~StackHandler()
{
}

// Ignore the first 2 frames that belong to this library
#define SKIP_FRAMES 2

void StackHandler::dumpMangled(int loop)
{
    int currentFrame = SKIP_FRAMES;
    for (int i = 1; i <= loop; i++, currentFrame++) {
        Logger::printf("%s#%d: %s\n", threadID, i, this->traceStrings[currentFrame]);
    }
}

/**
 * Convert a mangled C++ function name into human readable string.
 *
 * @return string containing demangled frame string.
 */
std::string StackHandler::demangleFrame(char *frame)
{
    std::string result;

    regmatch_t pmatch[REGEX_BUFFER];
    int ret = regexec(&framePattern, frame, REGEX_BUFFER, pmatch, 0);
    if (ret != 0) {
        // probably not a mangled name
        return result;
    }

    int begin  = pmatch[2].rm_so;
    int end    = pmatch[2].rm_eo;
    int length = end - begin + 1;
    if (length <= 1) {
        // there may be no function name in the frame
        return result;
    }

    char *mangled = new char[length];
    int destIndex = 0, srcIndex = 0;
    for (srcIndex = begin; srcIndex < end; srcIndex++) {
        mangled[destIndex++] = frame[srcIndex];
    }
    mangled[destIndex] = '\0';

    int status;
    char *demangled = abi::__cxa_demangle(mangled, NULL, NULL, &status);
    delete[] mangled;
    if (status != 0) { // demangle failed
        return result;
    }

    // copy library or program name
    result.append(frame+pmatch[1].rm_so, (pmatch[1].rm_eo - pmatch[1].rm_so + 1));
    // copy demangled function
    result.append(demangled);
    // copy the rest
    result.append(frame+pmatch[3].rm_so, (pmatch[3].rm_eo - pmatch[3].rm_so + 1));
    free(demangled);
    return result;
}

void StackHandler::dumpDemangled(int loop)
{
    int currentFrame = SKIP_FRAMES;
    for (int i = 1; i <= loop; i++, currentFrame++) {
        std::string demangled = demangleFrame(this->traceStrings[currentFrame]);
        if (demangled.length() == 0) {
            Logger::printf("%s#%d: %s\n", threadID, i, this->traceStrings[currentFrame]);
        }
        else {
            Logger::printf("%s#%d: %s\n", threadID, i, demangled.c_str());
        }
    }
}

void StackHandler::dumpVerboseFrames() {
    if (frameCount <= SKIP_FRAMES || traceStrings == NULL)
        return;

    int depth = frameCount - SKIP_FRAMES;

    Logger::printf("%sStack Frames\n", threadID);

    int loop = (Options::maxFrames < depth) ? Options::maxFrames : depth;
    if (Options::demangleFunction) {
        dumpDemangled(loop);
    }
    else {
        dumpMangled(loop);
    }
    free(this->traceStrings);
    this->traceStrings = NULL;
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
