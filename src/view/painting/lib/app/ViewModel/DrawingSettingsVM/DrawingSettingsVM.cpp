#include "DrawingSettingsVM.h"


DrawingSettingsVM::DrawingSettingsVM(){
   _toolManager = AppContext::instance().getToolManager();
   _toolSettings = AppContext::instance().getToolSettings();
   
    _toolSettings->drawingContext.shape = new SquareTip();
}
DrawingSettingsVM::~DrawingSettingsVM(){
}

int DrawingSettingsVM::getSize(){ return _toolSettings->drawingContext.shape->getSize(); }
float DrawingSettingsVM::getHardness(){ return _toolSettings->drawingContext.hardness; }
unsigned int DrawingSettingsVM::getColor(){
    return _toolSettings->drawingContext.color.hex();
}
int DrawingSettingsVM::getSpacing() {return _toolSettings->drawingContext.spacing; }
float DrawingSettingsVM::getStabilizer(){ return _toolSettings->drawingContext.stabilizer; }
bool DrawingSettingsVM::getPixelPerfect(){ return _toolSettings->drawingContext.pixelPerfect; }
bool DrawingSettingsVM::isPatternEnable(){ return _toolSettings->drawingContext.enablePattern; }
bool DrawingSettingsVM::isSolidColor(){ return _toolSettings->drawingContext.isSolidColor; }

void DrawingSettingsVM::setSize(int size){
    if(size < 1 || size > 16) return;
 
    _toolSettings->drawingContext.shape->setSize(size);
    notify("brush");
}
void DrawingSettingsVM::setHardness(float hardness){
    if(hardness < 0.0f || hardness > 1.0f) return;

    _toolSettings->drawingContext.hardness = hardness;
    notify("brush");
}
void DrawingSettingsVM::setColor(int r, int g, int b){
    _toolSettings->drawingContext.color.red = r / 255.0f;
    _toolSettings->drawingContext.color.green = g / 255.0f;
    _toolSettings->drawingContext.color.blue = b / 255.0f;
    _toolSettings->drawingContext.color.alpha = 1.0f;

    notify("brush");
}
void DrawingSettingsVM::setSpacing(int spacing){
    if(spacing < 0) return;
    _toolSettings->drawingContext.spacing = spacing;  
    notify("brush");
}
void DrawingSettingsVM::setStabilizer(float stabilizer){
    if(stabilizer < 0.0f || stabilizer > 1.0f) return;
    _toolSettings->drawingContext.stabilizer = (stabilizer);  
    notify("brush");     
}
void DrawingSettingsVM::setPixelPerfect(bool isEnabled){
    _toolSettings->drawingContext.pixelPerfect = (isEnabled);  
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

    _toolSettings->drawingContext.alignMode = (align); 
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

    _toolSettings->drawingContext.fillMode = (fill); 
    notify("brush");
}
void DrawingSettingsVM::setPattern(uintptr_t ptr, int size, int factor){
    int w = factor, h = size/factor;

    IBrushTip* pattern = _toolSettings->drawingContext.pattern;
    _toolSettings->drawingContext.pattern = new CustomTip(reinterpret_cast<unsigned int*>(ptr), w,h);
    delete pattern;

    notify("brush");
}
void DrawingSettingsVM::setShape(uintptr_t ptr, int size, int factor){
    int w = factor, h = size/factor;
    
    IBrushTip* shape = _toolSettings->drawingContext.shape;
    _toolSettings->drawingContext.shape = (new CustomTip(reinterpret_cast<unsigned int*>(ptr), w,h));
    delete shape;

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

    IBrushTip* shape = _toolSettings->drawingContext.shape;
    _toolSettings->drawingContext.shape = brushTip;
    delete shape;

    notify("brush");
}

void DrawingSettingsVM::setEnablePattern(bool enable){
    _toolSettings->drawingContext.enablePattern = (enable);
    notify("brush");
}
void DrawingSettingsVM::setSolidColor(bool enable){
    _toolSettings->drawingContext.isSolidColor = (enable);
    notify("brush");
}
BrushDTO DrawingSettingsVM::getBrush(){
    SurfaceDTO shapeDTO;
    if( _toolSettings->drawingContext.shape != nullptr){
        ISurface* shape = _toolSettings->drawingContext.shape->getMask();
        shapeDTO.buffer = emscripten::val(emscripten::typed_memory_view(shape->getWidth()* shape->getHeight()*4, reinterpret_cast<uint8_t*>(shape->getBuffer())));
        shapeDTO.width = shape->getWidth();
        shapeDTO.height = shape->getHeight();
    }
    SurfaceDTO patternDTO;

    if(_toolSettings->drawingContext.pattern != nullptr){
        ISurface* pattern = _toolSettings->drawingContext.pattern->getMask();
        patternDTO.buffer = emscripten::val(emscripten::typed_memory_view(pattern->getWidth()* pattern->getHeight()*4, reinterpret_cast<uint8_t*>(pattern->getBuffer())));
        patternDTO.width = pattern->getWidth();
        patternDTO.height = pattern->getHeight();
    }

    BrushDTO brushDTO;
    brushDTO.pattern = patternDTO;
    brushDTO.shape = shapeDTO;
    brushDTO.size = _toolSettings->drawingContext.shape->getSize();
    brushDTO.typeBrushTip = _toolSettings->drawingContext.shape->getType();
    brushDTO.color =  _toolSettings->drawingContext.color.hex();
    brushDTO.fillMode = (int)_toolSettings->drawingContext.fillMode;
    brushDTO.hardness = _toolSettings->drawingContext.hardness*100;
    brushDTO.spacing = _toolSettings->drawingContext.spacing;
    brushDTO.alignMode = (int)_toolSettings->drawingContext.alignMode;
    brushDTO.stabilizer = _toolSettings->drawingContext.stabilizer*100;
    brushDTO.pixelPerfect = _toolSettings->drawingContext.pixelPerfect;
    brushDTO.isPatternEnable = _toolSettings->drawingContext.enablePattern;
    brushDTO.isSolidColor = _toolSettings->drawingContext.isSolidColor;
    
    
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