#include <core/Logger.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

bool exray::Logger::initialized = false;
int  exray::Logger::logFD       = -1;

/**
 * Default logger that writes messages to stderr.
 */
bool exray::Logger::init()
{
    logFD = 2; // stderr
    initialized = true;
    return true;
}

/**
 * File logger.
 * Useful when the target application disables screen output by redirecting them to a blackhole.
 */
bool exray::Logger::init(const char *path)
{
    logFD = open(path, O_CREAT|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR);
    if (logFD == -1) {
        fprintf(stderr, "exray::Logger::init(): failed to open file: %s\n", strerror(errno));
        return false;
    }
    else {
        fprintf(stderr, "exray::Logger::init(): writing log to %s\n", path);
        initialized = true;
        return true;
    }
}

bool exray::Logger::printf(const char *format, ...)
{
    if (initialized == false)
        return false;

    va_list args;
    char buffer[512];

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    write(buffer);
}

bool exray::Logger::write(const char *message)
{
    if (initialized == false)
        return false;

    int messageLength = strlen(message);
    if (::write(logFD, message, messageLength) == messageLength)
        return true;
    else
        return false;
}

void exray::Logger::finish()
{
    close(logFD);
}
