#include "BrushSettingsVM.h"


BrushSettingsVM::BrushSettingsVM(){
   _toolManager = AppContext::instance().getToolManager();
   _toolSettings = AppContext::instance().getToolSettings();
   unsigned int* ptr = (unsigned int*)malloc(1*sizeof(unsigned int));
    _toolSettings->brushContext.setShape(new CustomTip(ptr, 1,1));
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
void BrushSettingsVM::setPattern(uintptr_t ptr, int size, int factor){
    // Pattern* flagPattern = _toolSettings->brushContext.selectedPattern;
    // Pattern* pattern = new Pattern("teste", reinterpret_cast<unsigned int*>(ptr), size, factor);
    // _toolSettings->brushContext.selectedPattern = pattern;
    // delete flagPattern;
    
    int w = factor, h = size/factor;
    _toolSettings->brushContext.setShape(new CustomTip(reinterpret_cast<unsigned int*>(ptr), w,h));

}
void BrushSettingsVM::setCustomShape(string pattern){
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
    emscripten::val data = emscripten::val(emscripten::typed_memory_view(shape->getWidth()* shape->getHeight()*4, reinterpret_cast<uint8_t*>( shape->getBuffer())))
        .call<emscripten::val>("slice");
    // emscripten::val data = emscripten::val(emscripten::typed_memory_view(
    //     _toolSettings->brushContext.selectedPattern->getWidth()* _toolSettings->brushContext.selectedPattern->getHeight()*4, 
    //     reinterpret_cast<uint8_t*>( _toolSettings->brushContext.selectedPattern->getBuffer()))).call<emscripten::val>("slice");
    
    BrushDTO brushDTO;
    brushDTO.buffer = data;
    brushDTO.width = shape->getWidth();
    brushDTO.height = shape->getHeight();
    
    
    return brushDTO;
}
BrushDTO BrushSettingsVM::getShape(string pattern){
    DrawBuffer draw(_toolSettings->drawingContext.size, _toolSettings->drawingContext.size);
    Surface* surface = draw.getSurface();
    
    if(pattern == "circle"){
        CircleRasterize circle(
            {_toolSettings->drawingContext.size-1, _toolSettings->drawingContext.size-1},
            {((_toolSettings->drawingContext.size-1)>>1),((_toolSettings->drawingContext.size-1)>>1)}
        );
        bool isFilled = true;
        circle.filled(isFilled);
        circle.draw(draw);
    }
    if(pattern == "square"){
        std::fill(surface->getBuffer(), surface->getBuffer()+surface->getLength(), 0xFF);
    }
    if(pattern == "line"){
    }
    emscripten::val data = emscripten::val(emscripten::typed_memory_view(surface->getWidth()* surface->getHeight()*4, reinterpret_cast<uint8_t*>(surface->getBuffer()))).call<emscripten::val>("slice");
    delete surface;

    BrushDTO brushDTO;
    brushDTO.buffer = data;
    brushDTO.width = _toolSettings->drawingContext.size;
    brushDTO.height = _toolSettings->drawingContext.size;
    brushDTO.id = Guid::generateUUID().toString();
    
    
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
        .function("setCustomShape", &BrushSettingsVM::setCustomShape)
        .function("getShape", &BrushSettingsVM::getShape)
        .function("getBrush", &BrushSettingsVM::getBrush)
        ;
};