#include "DrawingContext.h"


#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<DrawingContext>("DrawingContext")
        .constructor<>()
        .property("size", &DrawingContext::size)
        .property("color", &DrawingContext::color)
        .property("hardness", &DrawingContext::hardness)
        ;
};