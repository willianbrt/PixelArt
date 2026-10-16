#include "./CircleTip.h"

CircleTip::CircleTip(){}
CircleTip::~CircleTip(){}

Surface* CircleTip::getMask(int& size){
    surface = new Surface(size,size);
    CircleRasterize circle(
        {size-1, size-1},
        {((size-1)>>1),((size-1)>>1)}
    );
    bool isFilled = true;
    circle.filled(isFilled);
    circle.draw(*this);
    
    return surface;
}

void  CircleTip::plot(const int& x, const int& y) {
    surface->putPixel(x,y,0xFF);
}