#include <stdio.h>
#include <dlfcn.h>

namespace Interpose
{
    // locate the address for given function name at runtime
    void *loadFuncPtr(const char *name) {
        void *ptr = dlsym(RTLD_NEXT, name);
        const char *message = dlerror();
        if (message != NULL) {
            fprintf(stderr, "libexray.so: dlsym for %s failed: %s\n", name, message);
            fflush(stderr);
            return NULL;
        }
        return ptr;
    }
}
