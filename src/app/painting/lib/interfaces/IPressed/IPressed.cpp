#include "IPressed.h"

#include <emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;
EMSCRIPTEN_BINDINGS(ipressed_module){
    class_<IPressed>("IPressed")
        .function("onPressed", &IPressed::onPressed)
        .function("onTracking", &IPressed::onTracking)
        .function("onRelease", &IPressed::onRelease);
}