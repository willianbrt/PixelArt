#include "./CustomTip.h"

CustomTip::~CustomTip(){}

CustomTip::CustomTip(unsigned int* buffer, int width, int height) {
    _surface = new Surface(buffer, width, height);

    int size = std::max(width, height);
    setSize(size);
}

TYPE_BRUSH_TIP CustomTip::getType() {
    return TYPE_BRUSH_TIP::IMAGE;
}

Surface* CustomTip::getMask(){
    return _surface;
}

void CustomTip::setSize(int& size){
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
void CustomTip::setRotate(float& rad) {
    _rad = rad;
    _transform.setRad(rad);
}
int CustomTip::getSize(){
    return _size;
}
float CustomTip::getRotate(){
    return _rad;
}
Transformation CustomTip::getTransformation(){
    return _transform;
}