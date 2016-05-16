#include <core/Logger.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

bool Logger::initialized = false;
int  Logger::logFD       = -1;

/**
 * Default logger that writes messages to stderr.
 */
bool Logger::init()
{
    logFD = 2; // stderr
    initialized = true;
}

/**
 * File logger.
 * Useful when the target application disables screen output by redirecting them to a blackhole.
 */
bool Logger::init(const char *path)
{
    logFD = open(path, O_CREAT|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR);
    if (logFD == -1) {
        fprintf(stderr, "Logger::init(): failed to open file: %s\n", strerror(errno));
        return false;
    }
    else {
        fprintf(stderr, "Logger::init(): writing log to %s\n", path);
        initialized = true;
        return true;
    }
}

bool Logger::printf(const char *format, ...)
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

bool Logger::write(const char *message)
{
    if (initialized == false)
        return false;

    int messageLength = strlen(message);
    if (::write(logFD, message, messageLength) == messageLength)
        return true;
    else
        return false;
}

void Logger::finish()
{
    close(logFD);
}
