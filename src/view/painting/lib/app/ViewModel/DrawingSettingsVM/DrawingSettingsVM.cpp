#include "DrawingSettingsVM.h"


DrawingSettingsVM::DrawingSettingsVM(){
   _toolManager = AppContext::instance().getToolManager();
   _toolSettings = AppContext::instance().getToolSettings();
}
DrawingSettingsVM::~DrawingSettingsVM(){
}
void DrawingSettingsVM::setSize(int size){
    if(size < 1 || size > 16) return;

    _toolSettings->drawingContext.size = size;
}
void DrawingSettingsVM::setHardness(float hardness){
    if(hardness < 0.0f || hardness > 1.0f) return;

    _toolSettings->drawingContext.hardness = (int)(hardness*255.0f);
}
void DrawingSettingsVM::setColor(unsigned int color){
    _toolSettings->drawingContext.color = color;
}

#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<DrawingSettingsVM>("DrawingSettingsVM")
        .constructor<>()
        .function("setSize", &DrawingSettingsVM::setSize)
        .function("setHardness", &DrawingSettingsVM::setHardness)
        .function("setColor", &DrawingSettingsVM::setColor)
        ;
};