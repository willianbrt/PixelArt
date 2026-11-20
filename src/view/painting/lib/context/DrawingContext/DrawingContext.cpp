#include "DrawingContext.h"

DrawingContext::DrawingContext(){
    brushTip = new NoneTip();
}

void DrawingContext::setSize(int size){
    _size = size;
    brushTip->setSize(_size);
    _shape = brushTip->getMask(_size);
}
void DrawingContext::setColor(unsigned int color){ _color = color; }
void DrawingContext::setHardness(float hardness){ _hardness = hardness; }
void DrawingContext::setSpacing(int spacing){ _spacing = spacing; }
void DrawingContext::setStabilizer(float stabilizer){ _stabilizer = stabilizer; }
void DrawingContext::setPixelPerfect(bool isEnabled){ _pixelPerfect = isEnabled; }

int DrawingContext::getSize()  {return _size; }
float DrawingContext::getHardness(){ return _hardness; }
unsigned int DrawingContext::getColor(){ return _color; }
int DrawingContext::getSpacing() {return _spacing; }
float DrawingContext::getStabilizer(){ return _stabilizer; }
bool DrawingContext::getPixelPerfect(){ return _pixelPerfect; }

void DrawingContext::setShape(IBrushTip* tip){
    IBrushTip* _tip = brushTip;
    brushTip = tip;
    delete _tip;
    
    _shape = brushTip->getMask(_size);
    _size = brushTip->getSize();
}
Surface* DrawingContext::getShape()
{
    return _shape;
}

TYPE_BRUSH_TIP DrawingContext::getTypeBrushTip()
{
    return brushTip->getType();
}
void DrawingContext::setPattern(IBrushTip* tip){
    IBrushTip* _tip = brushPattern;
    brushPattern = tip;
    delete _tip;
    isDirtyPattern = true;
}
Surface* DrawingContext::getPattern()
{
    if(isDirtyPattern){
        _pattern = brushPattern->getMask(_size);
        isDirtyPattern = false;
    }

    return _pattern;
}

void DrawingContext::setFillMode(FILL_MODE fillMode){
    _fillMode = fillMode;
}
FILL_MODE DrawingContext::getFillMode()
{
    return _fillMode;
}

void DrawingContext::setAlignMode(ALIGN_MODE alignMode){
    _alignMode = alignMode;
}
ALIGN_MODE DrawingContext::getAlignMode()
{
    return _alignMode;
}

bool DrawingContext::isPatternEnable(){
    return _enablePattern;

}
void DrawingContext::enablePattern(bool enable){
    _enablePattern = enable;
}
void DrawingContext::setIsSolidColor(bool enable){
    _isSolidColor = enable;
}
bool DrawingContext::isSolidColor(){
    return _isSolidColor;
}

Transformation* DrawingContext::getTransformation(){
    return &transformation;
}
IBrushTip* DrawingContext::getBrushTip(){
    return brushTip;
}