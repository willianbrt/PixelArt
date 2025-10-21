#include "IGraphic.h"

#include <emscripten.h>
#include <emscripten/bind.h>


using namespace emscripten;
EMSCRIPTEN_BINDINGS(IGraphic_binding) {
    class_<IGraphic>("IGraphic");
}