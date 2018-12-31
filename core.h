#pragma once
#include <string>
#include <memory>
#include <functional>

using std::string;
using std::unique_ptr;
using std::function;

#define Activator_Create(type) unique_ptr<type>(reinterpret_cast<type*>(Activator::create(#type)))
#define Activator_Register(type) static bool _ = Activator::add(#type, []{return reinterpret_cast<void*>(new type##Impl());})
#define Default_Destructor(type) public: virtual ~type() {}

typedef function<void*()> constructor;

class Activator {
public:
    static bool add(const string& type, constructor ctor);
    static void* create(const string& type);
};
