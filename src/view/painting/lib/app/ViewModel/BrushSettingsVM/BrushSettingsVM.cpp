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
void BrushSettingsVM::setPattern(uintptr_t ptr, int size, int factor){
    Pattern* flagPattern = _toolSettings->brushContext.selectedPattern;
    Pattern* pattern = new Pattern("teste", reinterpret_cast<unsigned int*>(ptr), size, factor);
    _toolSettings->brushContext.selectedPattern = pattern;
    delete flagPattern;

}
void BrushSettingsVM::setShape(string pattern){
    if(pattern == "square"){
        _toolSettings->brushContext.setShape("square");
        return;
    }
    if(pattern == "circle"){
        _toolSettings->brushContext.setShape("circle");
        return;
    }
    _toolSettings->brushContext.setShape(pattern);
}
BrushDTO BrushSettingsVM::getBrush(){
    emscripten::val data = emscripten::val(emscripten::typed_memory_view(
        _toolSettings->brushContext.selectedPattern->getWidth()* _toolSettings->brushContext.selectedPattern->getHeight()*4, 
        reinterpret_cast<uint8_t*>( _toolSettings->brushContext.selectedPattern->getBuffer()))).call<emscripten::val>("slice");
    
    BrushDTO brushDTO;
    brushDTO.buffer = data;
    brushDTO.width = _toolSettings->brushContext.selectedPattern->getWidth();
    brushDTO.height = _toolSettings->brushContext.selectedPattern->getHeight();
    
    
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
        LineRasterize line(
            {_toolSettings->drawingContext.size-1,0},
            {0,_toolSettings->drawingContext.size-1}
        );
        while(line.hasNext()){
            Point p = line.next();
            surface->putPixel(p.x,p.y,0xFF);
        }
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
        .function("setShape", &BrushSettingsVM::setShape)
        .function("getShape", &BrushSettingsVM::getShape)
        .function("getBrush", &BrushSettingsVM::getBrush)
        ;
};