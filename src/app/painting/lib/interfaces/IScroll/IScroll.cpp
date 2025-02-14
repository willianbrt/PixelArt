#include "IScroll.h"

#include <emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;
EMSCRIPTEN_BINDINGS(iscroll_module){
    class_<IScroll>("IScroll")
        .function("onScroll", &IScroll::onScroll);
}