#include "../FrameDTO/FrameDTO.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    register_vector<LayerDTO>("VectorLayerDTO");

    value_object<FrameDTO>("FrameDTO")
        .field("timeDuration", &FrameDTO::timeDuration)
        .field("isActive", &FrameDTO::isActive)
        .field("layers", &FrameDTO::layers)
        .field("id", &FrameDTO::id)
        .field("buffer", &FrameDTO::buffer)
        .field("width", &FrameDTO::width)
        .field("height", &FrameDTO::height)
        ;
}