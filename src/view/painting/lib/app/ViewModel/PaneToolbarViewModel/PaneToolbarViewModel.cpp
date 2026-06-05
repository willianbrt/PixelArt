#include "PaneToolbarViewModel.h"


PaneToolbarViewModel::PaneToolbarViewModel(){
   _toolManager = AppContext::instance().getToolManager();
   _toolSettings = AppContext::instance().getToolSettings();
}
PaneToolbarViewModel::~PaneToolbarViewModel(){
}
void PaneToolbarViewModel::setPressedTool(std::string tool){
    if(tool == "brush"){
        _toolManager->setRightToolPressed(new BrushStrategy(&_toolSettings->brushContext, &_toolSettings->drawingContext, &_toolSettings->symmetryContext));
        return;
    }
    if(tool == "select"){
        _toolManager->setRightToolPressed(new SelectStrategy(AppContext::instance().getEditorManager()->getActiveEditor()->getSelectContext(), &_toolSettings->symmetryContext));
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