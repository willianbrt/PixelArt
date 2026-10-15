#include "BrushSettingsVM.h"


BrushSettingsVM::BrushSettingsVM(){
   _toolManager = AppContext::instance().getToolManager();
   _toolSettings = AppContext::instance().getToolSettings();
   
    _toolSettings->brushContext.setShape(new SquareTip());
}
BrushSettingsVM::~BrushSettingsVM(){
}
void BrushSettingsVM::setSize(int size){
    if(size < 1 || size > 16) return;

    _toolSettings->brushContext.setSize(size);
}
void BrushSettingsVM::setHardness(float hardness){
    if(hardness < 0.0f || hardness > 1.0f) return;

    _toolSettings->drawingContext.hardness = hardness;
    _toolSettings->drawingContext.color = (_toolSettings->drawingContext.color & 0xFFFFFF00) | static_cast<int>(hardness * 255.0f);
}
void BrushSettingsVM::setColor(unsigned int color){
    _toolSettings->drawingContext.color = color;        
}
void BrushSettingsVM::setPattern(uintptr_t ptr, int size, int factor){
    int w = factor, h = size/factor;
    _toolSettings->brushContext.setShape(new CustomTip(reinterpret_cast<unsigned int*>(ptr), w,h));

}
void BrushSettingsVM::setShape(string pattern){
    IBrushTip* brushTip = new SquareTip();
    if(pattern == "square"){
        brushTip = new SquareTip();
    }
    if(pattern == "circle"){
        brushTip = new CircleTip();
    }
    if(pattern == "line"){
        brushTip = new LineTip();
    }


    _toolSettings->brushContext.setShape(brushTip);
}
BrushDTO BrushSettingsVM::getBrush(){
    ISurface* shape = _toolSettings->brushContext.getShape();
    emscripten::val data = emscripten::val(emscripten::typed_memory_view(shape->getWidth()* shape->getHeight()*4, reinterpret_cast<uint8_t*>( shape->getBuffer())));
        // .call<emscripten::val>("slice");
    // emscripten::val data = emscripten::val(emscripten::typed_memory_view(
    //     _toolSettings->brushContext.selectedPattern->getWidth()* _toolSettings->brushContext.selectedPattern->getHeight()*4, 
    //     reinterpret_cast<uint8_t*>( _toolSettings->brushContext.selectedPattern->getBuffer()))).call<emscripten::val>("slice");
    
    BrushDTO brushDTO;
    brushDTO.buffer = data;
    brushDTO.width = shape->getWidth();
    brushDTO.height = shape->getHeight();
    
    
    return brushDTO;
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
        .function("setShape", &BrushSettingsVM::setShape)
        .function("getBrush", &BrushSettingsVM::getBrush)
        ;
};