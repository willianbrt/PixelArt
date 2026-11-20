#include "DrawingSettingsVM.h"


DrawingSettingsVM::DrawingSettingsVM(){
   _toolManager = AppContext::instance().getToolManager();
   _toolSettings = AppContext::instance().getToolSettings();
   
   IBrushTip* brushTip = new SquareTip();
    _toolSettings->drawingContext.setShape(brushTip);
    _toolSettings->drawingContext.setSize(1);
}
DrawingSettingsVM::~DrawingSettingsVM(){
}

int DrawingSettingsVM::getSize(){ return _toolSettings->drawingContext.getSize(); }
float DrawingSettingsVM::getHardness(){ return _toolSettings->drawingContext.getHardness(); }
unsigned int DrawingSettingsVM::getColor(){ return _toolSettings->drawingContext.getColor(); }
int DrawingSettingsVM::getSpacing() {return _toolSettings->drawingContext.getSpacing(); }
float DrawingSettingsVM::getStabilizer(){ return _toolSettings->drawingContext.getStabilizer(); }
bool DrawingSettingsVM::getPixelPerfect(){ return _toolSettings->drawingContext.getPixelPerfect(); }
bool DrawingSettingsVM::isPatternEnable(){ return _toolSettings->drawingContext.isPatternEnable(); }
bool DrawingSettingsVM::isSolidColor(){ return _toolSettings->drawingContext.isSolidColor(); }

void DrawingSettingsVM::setSize(int size){
    if(size < 1 || size > 16) return;
 
    _toolSettings->drawingContext.setSize(size);
    notify("brush");
}
void DrawingSettingsVM::setHardness(float hardness){
    if(hardness < 0.0f || hardness > 1.0f) return;

    _toolSettings->drawingContext.setHardness(hardness);
    // _toolSettings->drawingContext.setColor((_toolSettings->drawingContext.getColor() & 0xFFFFFF00) | static_cast<int>(hardness * 255.0f));
    notify("brush");
}
void DrawingSettingsVM::setColor(int r, int g, int b){
    _toolSettings->drawingContext.setColor(r << 24 | g << 16 | b << 8 |  255);
    notify("brush");
}
void DrawingSettingsVM::setSpacing(int spacing){
    if(spacing < 0) return;
    _toolSettings->drawingContext.setSpacing(spacing);  
    notify("brush");
}
void DrawingSettingsVM::setStabilizer(float stabilizer){
    if(stabilizer < 0.0f || stabilizer > 1.0f) return;
    _toolSettings->drawingContext.setStabilizer(stabilizer);  
    notify("brush");     
}
void DrawingSettingsVM::setPixelPerfect(bool isEnabled){
    _toolSettings->drawingContext.setPixelPerfect(isEnabled);  
    notify("brush");     
}



void DrawingSettingsVM::setAlignMode(string alignMode){
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

    _toolSettings->drawingContext.setAlignMode(align); 
    notify("brush");
}

void DrawingSettingsVM::setFillMode(string fillMode){
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

    _toolSettings->drawingContext.setFillMode(fill); 
    notify("brush");
}
void DrawingSettingsVM::setPattern(uintptr_t ptr, int size, int factor){
    int w = factor, h = size/factor;
    _toolSettings->drawingContext.setPattern(new CustomTip(reinterpret_cast<unsigned int*>(ptr), w,h));

    notify("brush");
}
void DrawingSettingsVM::setShape(uintptr_t ptr, int size, int factor){
    int w = factor, h = size/factor;
    
    _toolSettings->drawingContext.setShape(new CustomTip(reinterpret_cast<unsigned int*>(ptr), w,h));
    notify("brush");
}
void DrawingSettingsVM::setShape(string pattern){
    IBrushTip* brushTip = nullptr;
    if(pattern == "square"){
        brushTip = new SquareTip();
    }
    if(pattern == "circle"){
        brushTip = new CircleTip();
    }
    if(pattern == "line"){
        brushTip = new LineTip();
    }
    
    _toolSettings->drawingContext.setShape(brushTip);

    notify("brush");
}

void DrawingSettingsVM::setEnablePattern(bool enable){
    _toolSettings->drawingContext.enablePattern(enable);
    notify("brush");
}
void DrawingSettingsVM::setSolidColor(bool enable){
    _toolSettings->drawingContext.setIsSolidColor(enable);
    notify("brush");
}
BrushDTO DrawingSettingsVM::getBrush(){
    ISurface* pattern = _toolSettings->drawingContext.getPattern();
    ISurface* shape = _toolSettings->drawingContext.getShape();


    SurfaceDTO shapeDTO;
    if(shape->getBuffer() != nullptr){
        shapeDTO.buffer = emscripten::val(emscripten::typed_memory_view(shape->getWidth()* shape->getHeight()*4, reinterpret_cast<uint8_t*>(shape->getBuffer())));
        shapeDTO.width = shape->getWidth();
        shapeDTO.height = shape->getHeight();
    }
    SurfaceDTO patternDTO;
    if(pattern != nullptr){
        patternDTO.buffer = emscripten::val(emscripten::typed_memory_view(pattern->getWidth()* pattern->getHeight()*4, reinterpret_cast<uint8_t*>(pattern->getBuffer())));
        patternDTO.width = pattern->getWidth();
        patternDTO.height = pattern->getHeight();
    }

    BrushDTO brushDTO;
    brushDTO.pattern = patternDTO;
    brushDTO.shape = shapeDTO;
    brushDTO.color =  _toolSettings->drawingContext.getColor();
    brushDTO.fillMode = (int)_toolSettings->drawingContext.getFillMode();
    brushDTO.hardness = _toolSettings->drawingContext.getHardness()*100;
    brushDTO.spacing = _toolSettings->drawingContext.getSpacing();
    brushDTO.size = _toolSettings->drawingContext.getBrushTip()->getSize();
    brushDTO.alignMode = (int)_toolSettings->drawingContext.getAlignMode();
    brushDTO.stabilizer = _toolSettings->drawingContext.getStabilizer()*100;
    brushDTO.pixelPerfect = _toolSettings->drawingContext.getPixelPerfect();
    brushDTO.isPatternEnable = _toolSettings->drawingContext.isPatternEnable();
    brushDTO.isSolidColor = _toolSettings->drawingContext.isSolidColor();
    brushDTO.typeBrushTip = _toolSettings->drawingContext.getTypeBrushTip();
    
    
    return brushDTO;
}
void DrawingSettingsVM::registerEvent(std::string eventType, emscripten::val action){
    observable.registerEvent(eventType, action);
}
void DrawingSettingsVM::notify(std::string eventType){
    observable.notify(eventType);
}

#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<DrawingSettingsVM>("DrawingSettingsVM")
        .constructor<>()
        .function("setSize", &DrawingSettingsVM::setSize)
        .function("getPixelPerfect", &DrawingSettingsVM::getPixelPerfect)
        .function("setPixelPerfect", &DrawingSettingsVM::setPixelPerfect)
        .function("setHardness", &DrawingSettingsVM::setHardness)
        .function("setColor", &DrawingSettingsVM::setColor)        
        .function("getColor", &DrawingSettingsVM::getColor)
        .function("getHardness", &DrawingSettingsVM::getHardness)
        .function("getSize", &DrawingSettingsVM::getSize)
        .function("setSpacing", &DrawingSettingsVM::setSpacing)
        .function("getSpacing", &DrawingSettingsVM::getSpacing)
        .function("setStabilizer", &DrawingSettingsVM::setStabilizer)
        .function("getStabilizer", &DrawingSettingsVM::getStabilizer)
        .function("setAlignMode", &DrawingSettingsVM::setAlignMode)
        .function("setFillMode", &DrawingSettingsVM::setFillMode)
        .function("setPattern", &DrawingSettingsVM::setPattern)
        .function("setShape", select_overload<void(string)>(&DrawingSettingsVM::setShape))
        .function("setCustomShape", select_overload<void(uintptr_t, int, int)>(&DrawingSettingsVM::setShape))
        .function("getBrush", &DrawingSettingsVM::getBrush)
        .function("setEnablePattern", &DrawingSettingsVM::setEnablePattern)
        .function("setSolidColor", &DrawingSettingsVM::setSolidColor)
        .function("isSolidColor", &DrawingSettingsVM::isSolidColor)
        .function("isPatternEnable", &DrawingSettingsVM::isPatternEnable)
        .function("registerEvent", &DrawingSettingsVM::registerEvent)
        ;
};