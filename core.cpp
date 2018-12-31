#include <iostream>
#include <unordered_map>
#include <type_traits>
#include "core.h"

using namespace std;

unordered_map<string, constructor> &types()
{
    static unordered_map<string, constructor> v;
    return v;
}

bool Activator::add(const string &type, constructor ctor)
{
    // static unordered_map<string, constructor> cc;
    // cout << "adding type -> " << type << endl;
    auto &v = types();
    v[type] = ctor;
    // cout << "finishing added type -> " << type << endl;
    return true;
}

// template <typename T>
void *Activator::create(const string &type)
{
    // cout << "creating type -> " << type << endl;
    auto &v = types();
    // static_assert(is_base_of<Disposable, T>::value, "T must inherit from `Disposable`");
    return reinterpret_cast<void *>(v[type]());
}