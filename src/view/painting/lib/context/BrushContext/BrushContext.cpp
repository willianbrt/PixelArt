#include "BrushContext.h"

BrushContext::BrushContext(){}

void BrushContext::setSize(int size){
    _size = size;
    isDirtyTip = true;
}

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