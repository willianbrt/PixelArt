#include "ToolRuntimeContext.h"



void ToolRuntimeContext::clampBounding(Bounding& bounding){
    bounding.start.x = std::max(bounding.start.x,0);
    bounding.start.y = std::max(bounding.start.y,0);
    bounding.end.x = std::min(bounding.end.x, drawingAreaSize.x);
    bounding.end.y = std::min(bounding.end.y, drawingAreaSize.y);
}


#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<ToolRuntimeContext>("ToolRuntimeContext")
        .constructor<>();
};