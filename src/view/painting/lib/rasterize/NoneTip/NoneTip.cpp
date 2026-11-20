#include "./NoneTip.h"

NoneTip::~NoneTip(){
}

NoneTip::NoneTip() {
    _surface = new Surface(1, 1);
    _surface->putPixel(0,0);
    int size = 1;
    setSize(size);
}

TYPE_BRUSH_TIP NoneTip::getType() {
    return TYPE_BRUSH_TIP::NONE;
}

Surface* NoneTip::getMask(int& size){
    return _surface;
}

void NoneTip::setSize(int& size){
    _size = size;
}
void NoneTip::setRotate(float& rad) {
    _rad = rad;
    _transform.setRad(rad);
}
int NoneTip::getSize(){
    return _size;
}
float NoneTip::getRotate(){
    return _rad;
}
Transformation NoneTip::getTransformation(){
    return _transform;
}