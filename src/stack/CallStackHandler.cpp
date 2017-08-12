#include <stack/CallStackHandler.h>
#include <core/Options.h>
#include <core/Guardian.h>
#include <core/Logger.h>

using namespace exray;

/**
 * A utility class that enables stack dumps from arbitrary C and C++ functions.
 * See interpose/CFunction.cpp and interpose/PThread.cpp for example use of this class.
 */
CallStackHandler::CallStackHandler():
    StackHandler()
{
}

void CallStackHandler::dumpHeader(const char *origin)
{
    struct tm _tm;
    localtime_r(&this->timestamp.tv_sec, &_tm);

    Logger::printf("%s------------------------------------------------------------\n",
                   threadID);
    Logger::printf("%sOrigin of Dump: %s\n",
                   threadID, origin);
    Logger::printf("%sTime of Dump  : %d:%02d:%02d.%06d\n",
                   threadID ,_tm.tm_hour, _tm.tm_min, _tm.tm_sec, this->timestamp.tv_usec);
}

/**
 * Print stack frames. If the second argument is not NULL,
 * first 32 bytes of the object will be dumped.
 *
 * @param origin indicates the location of dump.
 * @param object is an optional pointer to an object.
 */
void CallStackHandler::dumpFrames(const char *origin, void *object)
{
    static const char *OBJECT_DUMP_FORMAT =
        "%sObject Content: %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x\n";

    if (active == false || (Options::dumpAll == false && exitCalled == false))
        return;

    captureFrames();

    Guardian g(dumpLock);
    dumpHeader(origin);

    if (object != NULL) {
        const unsigned char *ptr = (const unsigned char *)object;
        Logger::printf("%sObject Address: %08x\n", this->threadID, object);
        Logger::printf(OBJECT_DUMP_FORMAT,
                       this->threadID,
                       ptr[0],  ptr[1],  ptr[2],  ptr[3],  ptr[4],  ptr[5],  ptr[6],  ptr[7],
                       ptr[8],  ptr[9],  ptr[10], ptr[11], ptr[12], ptr[13], ptr[14], ptr[15]);
        Logger::printf(OBJECT_DUMP_FORMAT,
                       this->threadID,
                       ptr[16], ptr[17], ptr[18], ptr[19], ptr[20], ptr[21], ptr[22], ptr[23],
                       ptr[24], ptr[25], ptr[26], ptr[27], ptr[28], ptr[29], ptr[30], ptr[31]);
    }

    dumpVerboseFrames();
}

/**
 * Print stack frames. If the second argument is not NULL,
 * it is printed as a string.
 *
 * @param origin indicates the location of dump.
 * @param str is an optional pointer to a C string.
 */
void CallStackHandler::dumpFrames(const char *origin, const char *str)
{
    if (active == false || (Options::dumpAll == false && exitCalled == false))
        return;

    captureFrames();

    Guardian g(dumpLock);
    dumpHeader(origin);

    if (str != NULL) {
        Logger::printf("%sString Address: %08x\n", this->threadID, str);
        Logger::printf("%sString Value  : %s\n",   this->threadID, str);
    }

    dumpVerboseFrames();
}
