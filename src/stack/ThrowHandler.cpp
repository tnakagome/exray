#include <stack/ThrowHandler.h>
#include <core/Options.h>
#include <core/Guardian.h>
#include <core/System.h>
#include <core/Logger.h>

#include <cxxabi.h>
#include <sys/time.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

using namespace exray;

ThrowHandler::ThrowHandler():
    ExceptionHandler(), exceptionThrown(false)
{
}

/**
 * Called from __cxa_throw() in the interposing code.
 * Captures stack frames and stores exception information
 * in thread local storage. Optionally prints the stack frames.
 */
void ThrowHandler::captureFrames(const std::type_info &tinfo, const char *origin)
{
    StackHandler::captureFrames();
    this->exceptionThrown = true;
    bcopy(tinfo.name(), this->exceptionType, EXCEPTION_TYPE_LENGTH);
    if (Options::dumpAll || exitCalled) {
        dumpFrames(origin);
    }
}

void ThrowHandler::dumpHeader(const char *origin)
{
    struct tm _tm;
    localtime_r(&this->timestamp.tv_sec, &_tm);

    Logger::printf("%s------------------------------------------------------------\n",
                   this->threadID);
    Logger::printf("%sOrigin of Dump: %s\n",
                   this->threadID, origin);
    Logger::printf("%sException Time: %d:%02d:%02d.%06d\n",
                   this->threadID, _tm.tm_hour, _tm.tm_min, _tm.tm_sec,
                   this->timestamp.tv_usec);
}

void ThrowHandler::dumpVerboseFrames(const char *origin, int frameCount, char *traceStrings[])
{
    Guardian g(dumpLock);

    dumpHeader(origin);
    if (traceStrings == NULL)
        return;

    char *exceptionString;
    bool success = demangleException(&exceptionString);
    if (success) {
        Logger::printf("%sException Type: %s\n",
                       this->threadID, exceptionString);
        free(exceptionString);
    }
    else {
        Logger::printf("%sException Type (Mangled): %s\n",
                       this->threadID, this->exceptionType);
    }
    StackHandler::dumpVerboseFrames(frameCount, traceStrings);
}

void ThrowHandler::dumpFrames(const char *origin) {
    if (this->exceptionThrown == false || filterExceptions() == true || active == false) {
        return;
    }
    dumpVerboseFrames(origin, this->frameCount, this->traceStrings);
    this->exceptionThrown = false;
}
