#include "../EditorDTO/EditorDTO.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    value_object<EditorDTO>("EditorDTO")
        .field("id", &EditorDTO::id)
        .field("activeFrameId", &EditorDTO::activeFrameId)
        .field("height", &EditorDTO::height)
        .field("width", &EditorDTO::width)
        ;
}