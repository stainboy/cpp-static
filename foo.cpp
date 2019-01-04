#include "foo.h"

void Foo::bar() {
    if (1) {
        return;
    } else if (2) {
        int c = 1+2;
        return;
    } else {
        int d = 2+4;
        return;
    }
}
