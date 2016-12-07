#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <errno.h>

#include <core/System.h>
#include <core/Options.h>
#include <stack/ThrowHandler.h>

namespace exray {

bool          Options::dumpAll        = true;
bool          Options::pthreadDump    = false;
int           Options::maxFrames      = 100;
char         *Options::logFileName    = NULL;

StringList    Options::outputFilters;

namespace OptionParser {
    /**
     * Parse exception type names separated by pipe "|",
     * and stores them in an internal list.
     */
    static void parseOutputFilters(char *filterString)
    {
        char *saveptr, *token;
        static const char *FILTER_DELIMITER = "|";

        token = strtok_r(filterString, FILTER_DELIMITER, &saveptr);
        while (token != NULL) {
            Options::outputFilters.push_back(token);
            token = strtok_r(NULL, FILTER_DELIMITER, &saveptr);
        }
    }

    static void parseLogFilename(char *value)
    {
        Options::logFileName = strdup(value);
    }

    /**
     * Parse options for the library.
     *
     * Options:
     *  exitonly  : Print stack frame information from the last exception thrown
     *              when the target program and its threads exit. Without
     *              this option, all exception information will be written
     *              as exception is thrown.
     *
     *  logfile   : Output will be written to this file when specified.
     *              By default, stack dumps are written to stderr.
     *              This option redirects the output to file.
     *
     *  maxframes : Limit the number of frames per stack dump.
     *              Defaults to 100. Minimum is 3.
     *
     *  outputfilter : Exception names to be filtered, separated by '|'
     *              (the pipe char). This will be a partial match.
     *              If you write "long", then long, unsigned long, long long,
     *              and unsigned long long will be filtered.
     *
     *  pthread   : Turn on capturing of stack frames in some pthread functions.
     *              src/interpose/PThred.cpp must be included in the build
     *              if you want to use this option.
     */
    void parseOptions()
    {
        static const char *OPTION_DELIMITER = ",";
        static const char *OUTPUTFILTER_OPT = "outputfilter=";
        static const char *MAXFRAMES_OPT    = "maxframes=";
        static const char *LOGFILE_OPT      = "logfile=";

        char *options = getenv("LIBEXRAY");

        if (options != NULL) {
            char *saveptr, *value;

            value = strtok_r(options, OPTION_DELIMITER, &saveptr);
            while (value != NULL) {
                if (strcmp(value, "exitonly") == 0) {
                    Options::dumpAll = false;
                }
                else if (strstr(value, MAXFRAMES_OPT) != NULL) {
                    int frames = atoi(value + strlen(MAXFRAMES_OPT));
                    if (frames >= 3)
                        Options::maxFrames = frames;
                }
                else if (strcmp(value, "pthread") == 0) {
                    Options::pthreadDump = true;
                }
                else if (strstr(value, OUTPUTFILTER_OPT) != NULL) {
                    parseOutputFilters(value + strlen(OUTPUTFILTER_OPT));
                }
                else if (strstr(value, LOGFILE_OPT) != NULL) {
                    parseLogFilename(value + strlen(LOGFILE_OPT));
                }
                value = strtok_r(NULL, OPTION_DELIMITER, &saveptr);
            }
        }
    }
} // namespace OptionParser

} // namespace exray
