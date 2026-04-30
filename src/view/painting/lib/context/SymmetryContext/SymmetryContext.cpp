#include "SymmetryContext.h"


int SymmetryContext::pointMirrored(float point, float comprimento){
    return comprimento - point - 1; 
}
int SymmetryContext::pointMirrored(int point, int center, int comprimento){
    return comprimento - center - point - 1; 
}

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