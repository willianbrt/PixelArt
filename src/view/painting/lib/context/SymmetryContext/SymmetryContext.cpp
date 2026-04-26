#include "SymmetryContext.h"


#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<SymmetryContext>("SymmetryContext")
        .constructor<>()
        .property("isMirrorX", &SymmetryContext::isMirrorX)
        .property("isMirrorY", &SymmetryContext::isMirrorY)
        .property("nTileX", &SymmetryContext::nTileX)
        .property("nTileY", &SymmetryContext::nTileY)
        ;
};