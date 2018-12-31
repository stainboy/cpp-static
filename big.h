#pragma once
#include "core.h"
#include "math.h"

class Big {
    Default_Destructor(Big)
public:
    virtual int calculate(int a, Math* m) = 0;
};
