#include "ITool.h"

#include <emscripten.h>
#include <emscripten/bind.h>


using namespace emscripten;
EMSCRIPTEN_BINDINGS(itool_module) {
    class_<ITool>("ITool");
}