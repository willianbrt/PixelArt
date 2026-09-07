#include "BrushSettingsVM.h"


BrushSettingsVM::BrushSettingsVM(){
   _toolManager = AppContext::instance().getToolManager();
   _toolSettings = AppContext::instance().getToolSettings();
}
BrushSettingsVM::~BrushSettingsVM(){
}
void BrushSettingsVM::setSize(int size){
    if(size < 1 || size > 16) return;

    _toolSettings->drawingContext.size = size;
}
void BrushSettingsVM::setHardness(float hardness){
    if(hardness < 0.0f || hardness > 1.0f) return;

    _toolSettings->drawingContext.hardness = hardness;
    _toolSettings->drawingContext.color = (_toolSettings->drawingContext.color & 0xFFFFFF00) | static_cast<int>(hardness * 255.0f);
}
void BrushSettingsVM::setColor(unsigned int color){
    _toolSettings->drawingContext.color = color;
}
void BrushSettingsVM::setPattern(string pattern){
    if(pattern == "square"){
        _toolSettings->brushContext.getPattern("dot");
        return;
    }
    if(pattern == "circle"){
        _toolSettings->brushContext.getPattern("circle");
        return;
    }
    _toolSettings->brushContext.getPattern(pattern);
}

#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<BrushSettingsVM>("BrushSettingsVM")
        .constructor<>()
        .function("setSize", &BrushSettingsVM::setSize)
        .function("setHardness", &BrushSettingsVM::setHardness)
        .function("setColor", &BrushSettingsVM::setColor)
        .function("setPattern", &BrushSettingsVM::setPattern)
        ;
};