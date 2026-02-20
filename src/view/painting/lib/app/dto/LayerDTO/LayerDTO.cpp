#include "../LayerDTO/LayerDTO.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    value_object<LayerDTO>("LayerDTO")
        .field("id", &LayerDTO::id)
        .field("isLock", &LayerDTO::isLock)
        .field("isVisible", &LayerDTO::isVisible)
        .field("opacity", &LayerDTO::opacity)
        .field("buffer", &LayerDTO::buffer)
        ;
}