#include "./ObserverViewModel.h"

void ObserverViewModel::registerEvent(std::string eventType, emscripten::val callback){
    observable[eventType] = callback;
}
void ObserverViewModel::notify(std::string eventType){
    
    auto it = observable.find(eventType);
    if (it != observable.end()) {
        it->second();
    }
}
