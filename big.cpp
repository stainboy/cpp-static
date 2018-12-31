#include <iostream>
#include "core.h"
#include "big.h"

using namespace std;
class BigImpl : public Big {
public:
    int calculate(int a, Math* m) {
        return a * m->add(a, a+1);
    }
};

Activator_Register(Big);
