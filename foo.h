#pragma once

class Foo {
private:
    bool flag;
public:
    bool getFlag() {
        return flag;
    }
    void setFlag(bool value) {
        flag = value;
    }
    void bar();
    void bar2();
};

inline bool makeDAG() {
    return false;
}
