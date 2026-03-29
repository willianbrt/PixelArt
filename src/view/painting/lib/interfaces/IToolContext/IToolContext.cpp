#include "IToolContext.h"


#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<IToolContext>("IToolContext")
        .constructor<>()
        .property("isMirrorX", &IToolContext::isMirrorX)
        .property("isMirrorY", &IToolContext::isMirrorY)
        .property("nTileX", &IToolContext::nTileX)
        .property("nTileY", &IToolContext::nTileY)
        ;
};