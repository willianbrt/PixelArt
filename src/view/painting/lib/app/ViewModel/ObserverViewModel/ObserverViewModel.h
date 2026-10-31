#ifndef OBSERVERVIEWMODEL_H
#define OBSERVERVIEWMODEL_H

#include <emscripten/val.h>
#include <unordered_map>
#include <string>

class ObserverViewModel {
private:
    std::unordered_map<std::string, emscripten::val> observable;

public:
    ObserverViewModel() = default;
    ~ObserverViewModel() = default;
    void registerEvent(std::string eventType, emscripten::val callback);
    void notify(std::string eventType);
};

#endif