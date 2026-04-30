#include "ToolRuntimeContext.h"



#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<ToolRuntimeContext>("ToolRuntimeContext")
        .constructor<>()
        ;
};