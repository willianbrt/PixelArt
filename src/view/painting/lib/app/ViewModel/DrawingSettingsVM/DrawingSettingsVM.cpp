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

    _toolSettings->drawingContext.hardness = hardness;
    _toolSettings->drawingContext.color = (_toolSettings->drawingContext.color & 0xFFFFFF00) | static_cast<int>(hardness * 255.0f);
}
void DrawingSettingsVM::setColor(int r, int g, int b){
    _toolSettings->drawingContext.color = r << 24 | g << 16 | b << 8 | static_cast<int>(_toolSettings->drawingContext.hardness * 255.0f);
}
int DrawingSettingsVM::getSize(){
    return _toolSettings->drawingContext.size;
}
float DrawingSettingsVM::getHardness(){
    return _toolSettings->drawingContext.hardness;
}
unsigned int DrawingSettingsVM::getColor(){
    return _toolSettings->drawingContext.color;
}

#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<DrawingSettingsVM>("DrawingSettingsVM")
        .constructor<>()
        .function("setSize", &DrawingSettingsVM::setSize)
        .function("setHardness", &DrawingSettingsVM::setHardness)
        .function("setColor", &DrawingSettingsVM::setColor)
        .function("getColor", &DrawingSettingsVM::getColor)
        .function("getHardness", &DrawingSettingsVM::getHardness)
        .function("getSize", &DrawingSettingsVM::getSize)
        ;
};