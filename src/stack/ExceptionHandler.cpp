#include <stack/ExceptionHandler.h>
#include <core/Options.h>
#include <cxxabi.h>
#include <stdlib.h>

#include <list>
#include <string>

using namespace exray;

/**
 * Base class for ThrowHandler and CatchHandler
 */
ExceptionHandler::ExceptionHandler():
    StackHandler()
{
    exceptionType[0] = 0;
}

/**
 * @return true if exception names can be demangled successfully.
 * In that case, caller must free *destination.
 * If demangle failed, then false will be returned.
 */
bool ExceptionHandler::demangleException(char **destination)
{
    int status;
    *destination = abi::__cxa_demangle(this->exceptionType, NULL, NULL, &status);
    if (status == 0) {
        return true;
    }
    else {
        *destination = (char *)this->exceptionType;
        return false;
    }
}

/**
 * Determines whether current exception should be filtered out.
 *
 * @return true if current exception matches one of the output filter entries.
 * In this case, the caller should not dump current stack frames.
 */
bool ExceptionHandler::filterExceptions()
{
    if (! Options::outputFilters().empty()) {
        std::string currentException = this->exceptionType;

        char *exceptionString;
        bool success = demangleException(&exceptionString);
        if (success) {
            currentException = exceptionString;
            free(exceptionString);
        }

        StringList::iterator it = Options::outputFilters().begin();
        while (it != Options::outputFilters().end()) {
            const std::string &filterString = *it;
            if (currentException.find(filterString) != std::string::npos) {
                return true;
            }
            it++;
        }
    }
    return false;
}
