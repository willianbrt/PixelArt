#include "./SquareTip.h"

SquareTip::SquareTip(){
    _surface = new Surface(1,1);
    std::fill(_surface->getBuffer(), _surface->getBuffer()+_surface->getLength(), 0xFF);

    int size = 1;
    setSize(size);
}
SquareTip::~SquareTip(){}


TYPE_BRUSH_TIP SquareTip::getType() {
    return TYPE_BRUSH_TIP::GEOMTRIC;
}
Surface* SquareTip::getMask(){
    return _surface;
}
void  SquareTip::plot(const int& x, const int& y) {
    _surface->putPixel(x,y,0xFF);
}

void SquareTip::setSize(int& size){
    _size = size;

    PointF scale;
    if(_surface->getWidth() > _surface->getHeight()){
        scale.x = (float)_size / _surface->getWidth();
        scale.y = std::max(1.0f / _surface->getHeight(), scale.x);
    } else {
        scale.y = (float)_size / _surface->getHeight();
        scale.x = std::max(1.0f / _surface->getWidth(), scale.y);
    }
    _transform.setScale(scale);
}
void SquareTip::setRotate(float& rad) {
    _rad = rad;
    _transform.setRad(rad);
}
int SquareTip::getSize(){
    return _size;
}
float SquareTip::getRotate(){
    return _rad;
}
Transformation SquareTip::getTransformation(){
    return _transform;
}