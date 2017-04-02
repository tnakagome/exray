#include <core/Logger.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

bool exray::Logger::initialized = false;
int  exray::Logger::logFD       = -1;
bool exray::Logger::writeToFile = false;

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
        writeToFile = true;
        return true;
    }
}

#define LOG_BUFFER_SIZE 512

bool exray::Logger::printf(const char *format, ...)
{
    if (initialized == false)
        return false;

    va_list args;
    char buffer[LOG_BUFFER_SIZE];

    va_start(args, format);
    int result = vsnprintf(buffer, LOG_BUFFER_SIZE, format, args);
    va_end(args);
    if (result < 0) {
        return false;
    }
    else {
        write(buffer);
        return true;
    }
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
    if (writeToFile)
        close(logFD);
}
