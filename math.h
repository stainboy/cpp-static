#pragma once
#include "core.h"

class Math {
    Default_Destructor(Math)
public:
    virtual int add(int a, int b) = 0;
    virtual int multiple(int a, int b) = 0;
};
