#include "PaneToolbarViewModel.h"


PaneToolbarViewModel::PaneToolbarViewModel(){
   _toolManager = AppContext::instance().getToolManager();
brushContext = new BrushContext();
}
PaneToolbarViewModel::~PaneToolbarViewModel(){
}
void PaneToolbarViewModel::setPressedTool(std::string tool){
    if("brush"){
        _toolManager->setRightToolPressed(new BrushStrategy(brushContext, &drawingContext, &symmetryContext));
        return;
    }
    if("select"){
        _toolManager->setRightToolPressed(new SelectStrategy(&symmetryContext));
        return;
    }
}
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<PaneToolbarViewModel>("PaneToolbarViewModel")
        .constructor<>()
        .function("setPressedTool", &PaneToolbarViewModel::setPressedTool)
        ;
};