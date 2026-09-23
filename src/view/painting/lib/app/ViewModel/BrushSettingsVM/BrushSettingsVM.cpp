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
emscripten::val BrushSettingsVM::getShape(string pattern){
    DrawBuffer draw(_toolSettings->drawingContext.size, _toolSettings->drawingContext.size);
    Surface* surface = draw.getSurface();
    
    if(pattern == "circle"){
        CircleRasterize circle(
            {_toolSettings->drawingContext.size-1, _toolSettings->drawingContext.size-1},
            {(_toolSettings->drawingContext.size>>1),(_toolSettings->drawingContext.size>>1)}
        );
        bool isFilled = true;
        circle.filled(isFilled);
        circle.draw(draw);
    }
    if(pattern == "square"){
        std::fill(surface->getBuffer(), surface->getBuffer()+surface->getLength(), 0xFF);
        // std::memset(surface->getBuffer(), 0xFF, surface->getLength()*sizeof(unsigned int));
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

    return emscripten::val(emscripten::typed_memory_view(surface->getWidth()* surface->getHeight()*4, reinterpret_cast<uint8_t*>(surface->getBuffer())));
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
        .function("getShape", &BrushSettingsVM::getShape)
        ;
};