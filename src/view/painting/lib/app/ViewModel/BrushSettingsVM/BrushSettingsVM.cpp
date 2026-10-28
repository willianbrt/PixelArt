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

void BrushSettingsVM::enablePattern(bool enable){
    _toolSettings->brushContext.enablePattern(enable);
}
BrushDTO BrushSettingsVM::getBrush(){
    ISurface* pattern = _toolSettings->brushContext.getPattern();
    ISurface* shape = _toolSettings->brushContext.getShape();

    // PointF center = {shape->getWidth()*0.5f, shape->getHeight()*0.5f};
    // float w = shape->getWidth();
    // float h = shape->getHeight();

    // ISurface* brush = new Surface((int)w, (int)h);
    
    // StampRasterize stamp(center,
    //     {shape->getWidth(), shape->getHeight()},
    //     {brush->getWidth(), brush->getHeight()},
    //     _toolSettings->brushContext.transformation);

    // while(stamp.hasNext()){
    //     Point it = stamp.next();
    //     Point src = stamp.getSrcPoint();

    //     if(src.x < 0 || src.y < 0 || src.x >= shape->getWidth() || src.y >= shape->getHeight()) continue;
            
        
    //     unsigned int color;
    //     float mask;
    //     if(_toolSettings->brushContext.getFillMode() == FILL_MODE::MASK){
    //         Point srcPattern = {
    //             GraphicsEngine::clampedTilePoint(it.x % pattern->getWidth(), pattern->getWidth()),
    //             GraphicsEngine::clampedTilePoint(it.y % pattern->getHeight(), pattern->getHeight())
    //         };
    //         float maskShape = (shape->getPixel(src.x, src.y) & 0xFF) / 255.0f;
    //         mask = ((pattern->getPixel(srcPattern.x, srcPattern.y) & 0xFF) / 255.0f)*maskShape;
    //         color = _toolSettings->brushContext.getColor();
    //     } else if(_toolSettings->brushContext.getFillMode() == FILL_MODE::FILL){
    //         mask = (shape->getPixel(src.x, src.y) & 0xFF) / 255.0f;
    //         color = _toolSettings->brushContext.getColor();
    //     } else if(_toolSettings->brushContext.getFillMode() == FILL_MODE::STAMP){
    //         color = shape->getPixel(src.x, src.y);
    //         mask = (color & 0xFF) / 255.0f;
    //     }
        
    //     GraphicsEngine::setOpacity(color, mask);
    //     brush->putPixel(it.x, it.y, color);
    // }
    


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
    brushDTO.color =  _toolSettings->brushContext.getColor();
    brushDTO.fillMode = (int)_toolSettings->brushContext.getFillMode();
    brushDTO.hardness = _toolSettings->brushContext.getHardness()*100;
    brushDTO.spacing = _toolSettings->brushContext.getSpacing();
    brushDTO.size = _toolSettings->brushContext.getSize();
    brushDTO.alignMode = (int)_toolSettings->brushContext.getAlignMode();
    brushDTO.stabilizer = _toolSettings->brushContext.getStabilizer()*100;
    brushDTO.pixelPerfect = _toolSettings->brushContext.getPixelPerfect();
    brushDTO.isPatternEnable = _toolSettings->brushContext.isPatternEnable();
    
    
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
        .function("enablePattern", &BrushSettingsVM::enablePattern)
        ;
};