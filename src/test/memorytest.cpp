// C++ operator new should throw and libexray will capture the throw/catch
// from memory allocation failure.

#include <list>
#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>

// setrlimit
#include <sys/time.h>
#include <sys/resource.h>

using namespace std;

typedef unsigned long Unimatrix;
typedef unsigned long Adjunct;

class Designation {
public:
    Designation(const unsigned long _x, const unsigned long _y): x(_x), y(_y) {
    }

    string toString() const {
        stringstream s;
        s << y << " of " << x;
        return s.str();
    }

private:
    const unsigned long x;
    const unsigned long y;
};

class Drone {
public:
    Drone(const Unimatrix *_u, const Adjunct *_a, const Designation *_d): u(_u), a(_a), d(_d) {
    }

    const Unimatrix    *getUnimatrix()   { return u; }
    const Adjunct      *getAdjunct()     { return a; }
    const Designation  *getDesignation() { return d; }

    string toString() const {
        stringstream s;
        s << d->toString() << " of Adjunct " << *a << " of Unimatrix " << *u;
        return s.str();
    }

private:
    const Unimatrix   *u;
    const Adjunct     *a;
    const Designation *d;

    char buffer[1000000];
};

Drone *assimilate() {
    Unimatrix   *unimatrix   = new Unimatrix(rand() % 100);
    Adjunct     *adjunct     = new Adjunct(rand() % 100);
    Designation *designation = new Designation(rand() % 1000, rand() % 1000);
    Drone       *drone       = new Drone(unimatrix, adjunct, designation);

    return drone;
}

list<Drone*> ship;

void borg() {
    unsigned long count = 1;

    while (1) {
        Drone *d = assimilate();
        cout << count++ << ": " << d->toString() << endl;
        ship.push_back(d);
    }
}

// Configure the upper limit of address space that this process can use to 100MB.
// This will allow earlier core dump without causing swapping to disks,
// which can easily hang your computer.
void init() {
    struct rlimit r;
    getrlimit(RLIMIT_AS, &r);
    r.rlim_cur = 100000000;
    setrlimit(RLIMIT_AS, (const struct rlimit *)&r);
}

int main(int argc, char **argv) {
    try {
        init();
        borg();
    } catch (...) {
        cout << "unhandled exception caught in main" << endl;
    }
    return 0;
}
