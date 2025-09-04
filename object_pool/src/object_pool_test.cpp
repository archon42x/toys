#include <iostream>

#include "object_pool.h"


#ifdef DEBUG
template<typename... Args>
void log(Args... args)
{
    ((std::cout << args << " "), ...) << std::endl;
}
#else
template<typename... Args>
void log(Args... args) {}
#endif


struct MyObject {
    int x, y;
    MyObject()
    {
        log("MyObject() Constructor");
    }
    MyObject(int x, int y)
        : x(x), y(y)
    {
        log("MyObject(int, int) Constructor");
    }
    ~MyObject()
    {
        log("MyObject Destructor");
    }
};

int main()
{
    auto pool = ObjectPool<MyObject>(2);
    log("Pool size:", pool.available());
    {
        auto obj1 = pool.allocate(1, 2);
        log("Pool size:", pool.available());
    }
    log("Pool size:", pool.available());
    auto obj2 = pool.allocate(3, 4);
    log("Pool size:", pool.available());
    auto obj3 = pool.allocate(5, 6);
    log("Pool size:", pool.available());
    auto obj4 = pool.allocate(5, 6);
    log("Pool size:", pool.available());
}