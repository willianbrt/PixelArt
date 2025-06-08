#include "ITile.h"

#include <emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(itile_module){
    class_<ITile>("ITile")
        .function("resize", &ITile::resize, pure_virtual())
        .function("move", &ITile::move, pure_virtual());
};