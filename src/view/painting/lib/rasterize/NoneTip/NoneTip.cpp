#include "./NoneTip.h"

NoneTip::~NoneTip(){
}

NoneTip::NoneTip() {
    surface = new Surface(1, 1);
    surface->putPixel(0,0);
}

TYPE_BRUSH_TIP NoneTip::getType() {
    return TYPE_BRUSH_TIP::NONE;
}

Surface* NoneTip::getMask(int& size){
    return surface;
}