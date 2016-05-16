#include <pthread.h>
#include <core/System.h>

class Guardian
{
public:
           Guardian(pthread_mutex_t &);
          ~Guardian();

private:
           pthread_mutex_t &lock;
};

inline Guardian::Guardian(pthread_mutex_t &that):
lock(that)
{
    (System::getMutexLockFunc())(&lock);
}

inline Guardian::~Guardian()
{
    (System::getMutexUnlockFunc())(&lock);
}
