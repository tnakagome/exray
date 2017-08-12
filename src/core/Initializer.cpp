#include <core/System.h>
#include <core/Options.h>
#include <core/Logger.h>
#include <core/Initializer.h>

#include <stack/StackHandler.h>

#include <stdlib.h>

/**
 * Initializes various parts of the library when it is loaded into memory.
 */
__attribute__((constructor))
void exray::exray_init()
{
    System::init();
    OptionParser::parseOptions();

    if (Options::logFileName != NULL)
        Logger::init(Options::logFileName);
    else
        Logger::init();

    StackHandler::init();
    atexit(System::atexitHandler);
}

/**
 * Clean up the library when it is unloaded from memory.
 */
__attribute__((destructor))
void exray::exray_finish()
{
    StackHandler::finish();
    Logger::finish();
    System::finish();
}
