#include <stack/CatchHandler.h>
#include <core/Options.h>
#include <core/Guardian.h>
#include <core/Logger.h>

#include <stdlib.h>

using namespace exray;

CatchHandler::CatchHandler():
    ExceptionHandler(), exceptionCaught(false)
{
}

/**
 * Called from __cxa_begin_catch() in the interposing code.
 * Captures stack frames and stores exception information
 * in thread local storage. Optionally prints the stack frames.
 */
void CatchHandler::captureFrames(const char *origin)
{
    StackHandler::captureFrames();
    this->exceptionCaught = true;
    if (Options::dumpAll || exitCalled) {
        dumpFrames(origin);
    }
}

void CatchHandler::dumpHeader(const char *origin)
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

void CatchHandler::dumpVerboseFrames(const char *origin)
{
    Guardian g(dumpLock);

    if (traceStrings == NULL)
        return;

    dumpHeader(origin);

    if (this->exceptionType[0] != '\0') {
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
    }
    StackHandler::dumpVerboseFrames();
}

void CatchHandler::dumpFrames(const char *origin)
{
    if (this->exceptionCaught == false || filterExceptions() == true || active == false) {
        return;
    }
    dumpVerboseFrames(origin);
    this->exceptionCaught = false;
}
