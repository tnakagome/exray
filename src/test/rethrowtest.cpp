// Test interposition of std::rethrow_exception() in libexray.so
// Run as follows:
//
//   $ LD_PRELOAD=./libexray.so ./rethrowtest
//

#include <iostream>
#include <exception>
#include <stdexcept>

using namespace std;

exception_ptr p;

int main(int argc, char **argv) {

    try {
        throw invalid_argument("whatever");
    }
    catch(const exception &e) {
        p = current_exception();
    }

    try {
        rethrow_exception(p);
    }
    catch (const exception &e) {
        cout << "exception caught: " << e.what() << endl;
    }
    return 0;
}
