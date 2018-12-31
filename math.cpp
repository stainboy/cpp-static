#include <iostream>
#include "core.h"
#include "math.h"

using namespace std;
class MathImpl : public Math {
public:
    int add(int a, int b) {
        return a + b;
    }
    int multiple(int a, int b) {
        return a * b;
    }
};

Activator_Register(Math);
