#include "./CircleTip.h"

CircleTip::CircleTip(){}
CircleTip::~CircleTip(){}

Surface* CircleTip::getMask(int& size){
    Surface* surface = new Surface(size,size);
    CircleRasterize circle(
        {size-1, size-1},
        {((size-1)>>1),((size-1)>>1)}
    );
    bool isFilled = true;
    circle.filled(isFilled);
    // circle.draw(draw);
    return surface;
}
