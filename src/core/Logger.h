#ifndef __LOGGER__
#define __LOGGER__

#include <stdarg.h>

namespace exray {

    class Logger {
    public:
        static bool    init    ();
        static bool    init    (const char *);
        static void    finish  ();
        
        static bool    printf  (const char *, ...);
        static bool    write   (const char *);
        
    private:
        static bool    initialized;
        static int     logFD;
    };

} // namespace exray

#endif
