#include "./CircleTip.h"

CircleTip::CircleTip(){
    int size = 1;
    setSize(size);
}
CircleTip::~CircleTip(){}

TYPE_BRUSH_TIP CircleTip::getType() {
    return TYPE_BRUSH_TIP::GEOMTRIC;
}

Surface* CircleTip::getMask(){
    return _surface;
}

void  CircleTip::plot(const int& x, const int& y) {
    _surface->putPixel(x,y,0xFF);
}

void CircleTip::setSize(int& size){
    _surface = new Surface(size,size);
    CircleRasterize circle(
        {size-1, size-1},
        {((size-1)>>1),((size-1)>>1)}
    );

    bool isFilled = true;
    circle.filled(isFilled);
    circle.draw(*this);
 
    _size = size;
}
void CircleTip::setRotate(float& rad) {
    _rad = rad;
    _transform.setRad(rad);
}
int CircleTip::getSize(){
    return _size;
}
float CircleTip::getRotate(){
    return _rad;
}
Transformation CircleTip::getTransformation(){
    return _transform;
}