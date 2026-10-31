#ifndef ITOOLOBSERVER_H
#define ITOOLOBSERVER_H

#include <string>
#include <emscripten/val.h>

template <class T>
class IToolObserver{
private:
    virtual void notify(std::string eventType) = 0;
public:
    virtual void registerEvent(std::string eventType, emscripten::val action) = 0;
};

#endif