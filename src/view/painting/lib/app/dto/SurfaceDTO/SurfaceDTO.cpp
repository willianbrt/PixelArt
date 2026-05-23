#include "./SurfaceDTO.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    value_object<SurfaceDTO>("SurfaceDTO")
        .field("width", &SurfaceDTO::width)
        .field("height", &SurfaceDTO::height)
        .field("buffer", &SurfaceDTO::buffer)
        ;
}