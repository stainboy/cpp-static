#include <iostream>
#include "core.h"
#include "math.h"
#include "big.h"
#include "foo.h"
using namespace std;


int main(int c, char** args) {
    auto p = Activator_Create(Math);
    auto b = Activator_Create(Big);
    cout << "math -> " << p->add(2,3) << endl;
    cout << "big -> " << b->calculate(5, p.get()) << endl;

    Foo f;
    f.bar();
    f.bar2();
    return 0;
}
