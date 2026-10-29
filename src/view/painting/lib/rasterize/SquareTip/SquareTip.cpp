#include "./SquareTip.h"

SquareTip::SquareTip(){}
SquareTip::~SquareTip(){}


TYPE_BRUSH_TIP SquareTip::getType() {
    return TYPE_BRUSH_TIP::GEOMTRIC;
}
Surface* SquareTip::getMask(int& size){
    surface = new Surface(size,size);
    std::fill(surface->getBuffer(), surface->getBuffer()+surface->getLength(), 0xFF);
    return surface;
}
void  SquareTip::plot(const int& x, const int& y) {
    surface->putPixel(x,y,0xFF);
}