#ifndef __OPTIONS__
#define __OPTIONS__

#include <list>
#include <string>

typedef std::list<std::string> StringList;

struct Options
{
    static bool        dumpAll;
    static bool        pthreadDump;
    static int         maxFrames;
    static char       *logFileName;

    static StringList  outputFilters;
};

namespace OptionParser
{
    void parseOptions();
    void parseLogFilename();
    void finish();
};

#endif // __OPTIONS__
