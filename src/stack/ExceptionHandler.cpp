#include <stack/ExceptionHandler.h>
#include <core/Options.h>
#include <cxxabi.h>
#include <stdlib.h>

using namespace exray;

ExceptionHandler::ExceptionHandler():
    StackHandler()
{
    exceptionType[0] = 0;
}

/**
 * @return true if exception names can be demangled succeessfully.
 * In that case, caller must free *destination.
 * If demangled faied, then false will be returned.
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

bool ExceptionHandler::filterExceptions()
{
    if (! Options::outputFilters.empty()) {
        std::string currentException = this->exceptionType;

        char *exceptionString;
        bool success = demangleException(&exceptionString);
        if (success) {
            currentException = exceptionString;
            free(exceptionString);
        }

        StringList::iterator it = Options::outputFilters.begin();
        while (it != Options::outputFilters.end()) {
            const std::string &filterString = *it;
            if (currentException.find(filterString) != std::string::npos) {
                return true;
            }
            it++;
        }
    }
    return false;
}
