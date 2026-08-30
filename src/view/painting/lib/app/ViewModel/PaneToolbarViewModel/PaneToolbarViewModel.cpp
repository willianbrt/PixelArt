#include "PaneToolbarViewModel.h"


PaneToolbarViewModel::PaneToolbarViewModel(){
   _toolManager = AppContext::instance().getToolManager();
   _toolSettings = AppContext::instance().getToolSettings();
}
PaneToolbarViewModel::~PaneToolbarViewModel(){
}
void PaneToolbarViewModel::setPressedTool(std::string tool){
    if(tool == "brush"){
        _toolManager->setRightToolPressed(new BrushStrategy(&_toolSettings->brushContext, &_toolSettings->drawingContext));
        return;
    }
    if(tool == "select"){
        _toolManager->setRightToolPressed(new SelectStrategy(AppContext::instance().getEditorManager()->getActiveEditor()->getSelectContext()));
        return;
    }
    if(tool == "line"){
        _toolManager->setRightToolPressed(new LineStrategy(&_toolSettings->lineContext, &_toolSettings->drawingContext));
        return;
    }
    if(tool == "eraser"){
        _toolManager->setRightToolPressed(new EraserStrategy(&_toolSettings->eraserContext, &_toolSettings->drawingContext));
        return;
    }
    if(tool == "bucket"){
        _toolManager->setRightToolPressed(new BucketStrategy(&_toolSettings->bucketContext, &_toolSettings->drawingContext));
        return;
    }
    if(tool == "circle"){
        _toolManager->setRightToolPressed(new CircleStrategy(&_toolSettings->circleContext, &_toolSettings->drawingContext));
        return;
    }
    if(tool == "square"){
        _toolManager->setRightToolPressed(new SquareStrategy(&_toolSettings->squareContext, &_toolSettings->drawingContext));
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