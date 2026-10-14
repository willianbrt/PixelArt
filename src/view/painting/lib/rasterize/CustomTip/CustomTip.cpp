#include "./CustomTip.h"

CustomTip::~CustomTip(){}

CustomTip::CustomTip(unsigned int* buffer, int width, int height) {
    _buffer = buffer;
    _width = width;
    _height = height;
}

Surface* CustomTip::getMask(int& size){
    Surface* surface = new Surface(_buffer, _width, _height);
    return surface;
}