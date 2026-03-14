#include "BrushContext.h"

#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    register_vector<BrushContext::Pattern>("VectorPattern");
    // register_vector<float>("VectorFloat");
    // class_<DrawingContext>("BrushContext")
    // .field("name", &DrawingContext::Pattern)

    
    value_object<BrushContext::Pattern>("Pattern")
        .field("name", &BrushContext::Pattern::name)
        .field("buffer", &BrushContext::Pattern::buffer)
        ;

    class_<BrushContext>("BrushContext")
        .constructor<>()
        .property("selectedPattern", &BrushContext::selectedPattern)
        .property("pattern", &BrushContext::pattern)
        ;
};