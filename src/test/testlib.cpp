#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

static __thread int threshold = 2;
static __thread struct drand48_data randbuffer;

void threadinit(const int t)
{
    struct timeval timestamp;
    gettimeofday(&timestamp, NULL);
    srand48_r(timestamp.tv_usec, &randbuffer);
    if (t > 0)
        threshold = t;
}

const char *baz(const char *id)
{
    long result;
    lrand48_r(&randbuffer, &result);
    if ((result % 100) < threshold)
        throw 999999999LL;
    return "baz";
}

static void bar(const char *id)
{
    long result;
    lrand48_r(&randbuffer, &result);
    if ((result % 100) < threshold)
        throw 12345;
    baz(id);
}

void foo(const char *id)
{
    long result;
    lrand48_r(&randbuffer, &result);
    if ((result % 100) < threshold)
        throw std::string("  foo error!");;
    bar(id);
}
