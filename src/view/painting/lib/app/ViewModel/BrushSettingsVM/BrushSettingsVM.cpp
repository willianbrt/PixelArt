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

    _toolSettings->brushContext.setHardness(hardness);
    _toolSettings->brushContext.setColor((_toolSettings->brushContext.getColor() & 0xFFFFFF00) | static_cast<int>(hardness * 255.0f));
}
void BrushSettingsVM::setColor(int r, int g, int b){
    _toolSettings->brushContext.setColor(r << 24 | g << 16 | b << 8 | static_cast<int>(_toolSettings->brushContext.getHardness() * 255.0f));
}
void BrushSettingsVM::setSpacing(int spacing){
    if(spacing < 0) return;
    _toolSettings->brushContext.setSpacing(spacing);  
}
void BrushSettingsVM::setStabilizer(float stabilizer){
    if(stabilizer < 0.0f || stabilizer > 1.0f) return;
    _toolSettings->brushContext.setStabilizer(stabilizer);       
}
void BrushSettingsVM::setPixelPerfect(bool isEnabled){
    _toolSettings->brushContext.setPixelPerfect(isEnabled);       
}

int BrushSettingsVM::getSize(){
    return _toolSettings->brushContext.getSize();
}
float BrushSettingsVM::getHardness(){
    return _toolSettings->brushContext.getHardness();
}
unsigned int BrushSettingsVM::getColor(){
    return _toolSettings->brushContext.getColor();
}
int BrushSettingsVM::getSpacing() {return _toolSettings->brushContext.getSpacing(); }
float BrushSettingsVM::getStabilizer(){ return _toolSettings->brushContext.getStabilizer(); }
bool BrushSettingsVM::getPixelPerfect(){ return _toolSettings->brushContext.getPixelPerfect(); }


void BrushSettingsVM::setAlignMode(string alignMode){
    ALIGN_MODE align;
    if(alignMode == "brushing"){
        align = ALIGN_MODE::BRUSHING;
    }
    if(alignMode == "local"){
        align = ALIGN_MODE::LOCAL;
    }
    if(alignMode == "source"){
        align = ALIGN_MODE::SOURCE;
    }

    _toolSettings->brushContext.setAlignMode(align); 
}

void BrushSettingsVM::setFillMode(string fillMode){
    FILL_MODE fill;
    if(fillMode == "fill"){
        fill = FILL_MODE::FILL;
    }
    if(fillMode == "stamp"){
        fill = FILL_MODE::STAMP;
    }
    if(fillMode == "mask"){
        fill = FILL_MODE::MASK;
    }

    _toolSettings->brushContext.setFillMode(fill); 
}
void BrushSettingsVM::setPattern(uintptr_t ptr, int size, int factor){
    int w = factor, h = size/factor;
    _toolSettings->brushContext.setPattern(new CustomTip(reinterpret_cast<unsigned int*>(ptr), w,h));

}
void BrushSettingsVM::setShape(uintptr_t ptr, int size, int factor){
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
        .function("getPixelPerfect", &BrushSettingsVM::getPixelPerfect)
        .function("setPixelPerfect", &BrushSettingsVM::setPixelPerfect)
        .function("setHardness", &BrushSettingsVM::setHardness)
        .function("setColor", &BrushSettingsVM::setColor)        
        .function("getColor", &BrushSettingsVM::getColor)
        .function("getHardness", &BrushSettingsVM::getHardness)
        .function("getSize", &BrushSettingsVM::getSize)
        .function("setSpacing", &BrushSettingsVM::setSpacing)
        .function("getSpacing", &BrushSettingsVM::getSpacing)
        .function("setStabilizer", &BrushSettingsVM::setStabilizer)
        .function("getStabilizer", &BrushSettingsVM::getStabilizer)
        .function("setAlignMode", &BrushSettingsVM::setAlignMode)
        .function("setFillMode", &BrushSettingsVM::setFillMode)
        .function("setPattern", &BrushSettingsVM::setPattern)
        .function("setShape", select_overload<void(string)>(&BrushSettingsVM::setShape))
        .function("setCustomShape", select_overload<void(uintptr_t, int, int)>(&BrushSettingsVM::setShape))
        .function("getBrush", &BrushSettingsVM::getBrush)
        ;
};