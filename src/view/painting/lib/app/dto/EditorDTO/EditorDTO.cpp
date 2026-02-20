#include "../EditorDTO/EditorDTO.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    register_vector<FrameDTO>("VectorFrameDTO");

    value_object<EditorDTO>("EditorDTO")
        .field("activeFrame", &EditorDTO::activeFrame)
        .field("frames", &EditorDTO::frames)
        .field("height", &EditorDTO::height)
        .field("width", &EditorDTO::width)
        .field("n_tiles_x", &EditorDTO::n_tiles_x)
        .field("n_tiles_y", &EditorDTO::n_tiles_y)
        ;
}