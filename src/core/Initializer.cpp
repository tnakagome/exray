#include <core/System.h>
#include <core/Options.h>
#include <core/Guardian.h>
#include <core/Logger.h>

#include <stack/StackHandler.h>

#include <pthread.h>

#include <stdlib.h>

__attribute__((constructor))
void exray_init()
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

__attribute__((destructor))
void exray_finish()
{
    StackHandler::finish();
    Logger::finish();
    System::finish();
}
