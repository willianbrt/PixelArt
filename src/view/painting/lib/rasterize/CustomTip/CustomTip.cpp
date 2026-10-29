#include "./CustomTip.h"

CustomTip::~CustomTip(){}

CustomTip::CustomTip(unsigned int* buffer, int width, int height) {
    _buffer = buffer;
    _width = width;
    _height = height;
}

TYPE_BRUSH_TIP CustomTip::getType() {
    return TYPE_BRUSH_TIP::IMAGE;
}

Surface* CustomTip::getMask(int& size){
    Surface* surface = new Surface(_buffer, _width, _height);
    return surface;
}