#include "BrushContext.h"

BrushContext::BrushContext(){}

void BrushContext::setSize(int size){
    _size = size;
    isDirty = true;
}

void BrushContext::setShape(IBrushTip* tip){
    IBrushTip* _tip = brushTip;
    brushTip = tip;
    delete _tip;
    isDirty = true;
}
Surface* BrushContext::getShape()
{
    if(isDirty){
        _shape = brushTip->getMask(_size);
    }

    return _shape;
}