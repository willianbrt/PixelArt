#include "BrushContext.h"

BrushContext::BrushContext(){}

void BrushContext::setSize(int size){
    _size = size;
    isDirtyTip = true;
}
void BrushContext::setColor(unsigned int color){ _color = color; }
void BrushContext::setHardness(float hardness){ _hardness = hardness; }
void BrushContext::setSpacing(int spacing){ _spacing = spacing; }
void BrushContext::setStabilizer(float stabilizer){ _stabilizer = stabilizer; }
void BrushContext::setPixelPerfect(bool isEnabled){ _pixelPerfect = isEnabled; }

int BrushContext::getSize() {return _size; }
float BrushContext::getHardness(){ return _hardness; }
unsigned int BrushContext::getColor(){ return _color; }
int BrushContext::getSpacing() {return _spacing; }
float BrushContext::getStabilizer(){ return _stabilizer; }
bool BrushContext::getPixelPerfect(){ return _pixelPerfect; }

void BrushContext::setShape(IBrushTip* tip){
    IBrushTip* _tip = brushTip;
    brushTip = tip;
    delete _tip;
    isDirtyTip = true;
}
Surface* BrushContext::getShape()
{
    if(isDirtyTip){
        _shape = brushTip->getMask(_size);
        isDirtyTip = false;
    }

    return _shape;
}

TYPE_BRUSH_TIP BrushContext::getTypeBrushTip()
{
    return brushTip->getType();
}
void BrushContext::setPattern(IBrushTip* tip){
    IBrushTip* _tip = brushPattern;
    brushPattern = tip;
    delete _tip;
    isDirtyPattern = true;
}
Surface* BrushContext::getPattern()
{
    if(isDirtyPattern){
        _pattern = brushPattern->getMask(_size);
        isDirtyPattern = false;
    }

    return _pattern;
}

void BrushContext::setFillMode(FILL_MODE fillMode){
    _fillMode = fillMode;
}
FILL_MODE BrushContext::getFillMode()
{
    return _fillMode;
}

void BrushContext::setAlignMode(ALIGN_MODE alignMode){
    _alignMode = alignMode;
}
ALIGN_MODE BrushContext::getAlignMode()
{
    return _alignMode;
}

bool BrushContext::isPatternEnable(){
    return _enablePattern;

}
void BrushContext::enablePattern(bool enable){
    _enablePattern = enable;
}
void BrushContext::setIsSolidColor(bool enable){
    _isSolidColor = enable;
}
bool BrushContext::isSolidColor(){
    return _isSolidColor;
}