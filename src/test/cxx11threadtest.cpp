// C++11 std::thread compatibility test

#include <thread>
#include <iostream>
#include <exception>

class Exception : public std::exception {
public:
    Exception(const char *reason) {
        this->reason = reason;
    }

    const char *what() {
        return reason;
    }

private:
    const char *reason;
};

void func() throw(Exception)
{
    std::cout << "func() called" << std::endl;
    throw Exception("C++11 thread test");
}

void ThreadEntry()
{
    try {
        std::cout << "thread func started" << std::endl;
        func();
    }
    catch (Exception &e) {
        std::cout << "Caught an exception: reason: " << e.what() << std::endl;
    }
}

int main()
{
    std::thread t1(ThreadEntry);
    t1.join();
}
