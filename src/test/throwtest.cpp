#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>

#include <unistd.h>
#include <sys/syscall.h>

#include <test/testlib.h>

#include <iostream>
#include <sstream>

#ifndef PRELOAD
#include <core/Initializer.h>
#endif

// open()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

pthread_mutex_t testLock = PTHREAD_MUTEX_INITIALIZER;

static void log(const char *id, const std::ostringstream &message)
{
    pthread_mutex_lock(&testLock);
    std::ostringstream ss;
    ss << getpid() << "[" << syscall(SYS_gettid) << "] "
       << id << ": " << message.str() << std::endl;
    std::cerr << ss.str();
    std::cerr.flush();
    pthread_mutex_unlock(&testLock);
}

void *test_pthread_exit(void *args)
{
    static const char *id = "test_pthread_exit";
    int count;

    threadinit(2);

    try {
        for (count = 1; count <= 10; count++) {
            foo(id);
        }
    }
    catch (...) {
        std::ostringstream ss;
        ss << "caught exception after looping " << count << " times";
        log(id, ss);
        pthread_exit(NULL);
    }
    return (void *)"test_pthread_exit() no exceptions";
}

void *test_exit(void *args)
{
    static const char *id = "test_exit";
    int count;

    threadinit(2);

    try {
        for (count = 1; count <= 10; count++) {
            foo(id);
        }
    }
    catch (...) {
        std::ostringstream ss;
        ss << "caught exception after looping " << count << " times";
        log(id, ss);
        exit(-1);
    }
    return (void *)"test_exit() no exceptions";
}

void *test_thread_return(void *args)
{
    static const char *id = "test_thread_return";
    int count;

    threadinit(3);

    try {
        for (count = 1; count <= 10; count++) {
            foo(id);
        }
    }
    catch (...) {
        std::ostringstream ss;
        ss << "caught exception after looping " << count << " times";
        log(id, ss);
        return NULL;
    }
    return (void *)"test_thread_return() no exceptions";
}

void *test_pthread_cancel(void *args)
{
    static const char *id = "test_pthread_cancel";
    int count;

    threadinit(3);

    try {
        for (count = 1; count <= 10; count++) {
            foo(id);
        }
    }
    catch (...) {
        std::ostringstream ss;
        ss << "caught exception after looping " << count << " times";
        log(id, ss);
        pthread_cancel(pthread_self());
        return NULL;
    }
    return (void *)"test_pthread_cancel() no exceptions";
}

#define THREADS 3

int main(int argc, char **argv)
{
    static const char *id = "main";
    pthread_t thread[THREADS];

#ifndef PRELOAD
    exray::exray_init();
#endif

    int fd = open("/foo/bar/baz", O_RDONLY);
    if (fd != -1)
        close(fd);

    pthread_mutex_init(&testLock, NULL);

    pthread_create(&thread[0], NULL, test_thread_return, NULL);
    pthread_create(&thread[1], NULL, test_pthread_cancel, NULL);
    pthread_create(&thread[2], NULL, test_pthread_exit, NULL);
    // pthread_create(&thread[3], NULL, test_exit, NULL);

    threadinit(3);

    int count;

    try {
        for (count = 1; count <= 10; count++) {
            foo(id);
        }
        std::ostringstream ss;
        ss << "completed without throwing exceptions";
        log(id, ss);
    }
    catch (...) {
        std::ostringstream ss;
        ss << "caught exception after looping " << count << " times";
        log(id, ss);
    }

    for (int i = 0; i < THREADS; i++) {
        char *message;
        pthread_join(thread[i], (void **)&message);
        pid_t pid = getpid();
        std::ostringstream ss;
        ss << pid << " thread #" << i << " returned successfully" << std::endl;
        if (message != NULL && message != (void *)-1) {
            ss << pid << " thread #" << i << " " << message << std::endl;
        }
        std::cerr << ss.str();
    }

    pthread_mutex_destroy(&testLock);

#ifndef PRELOAD
    exray::exray_finish();
#endif

    return 0;
}
