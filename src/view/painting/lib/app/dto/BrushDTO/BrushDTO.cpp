#include "./BrushDTO.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    value_object<BrushDTO>("BrushDTO")
        .field("id", &BrushDTO::id)
        .field("buffer", &BrushDTO::buffer)
        .field("width", &BrushDTO::width)
        .field("height", &BrushDTO::height)
        ;
}