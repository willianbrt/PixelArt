#include "./SquareTip.h"

SquareTip::SquareTip(){}
SquareTip::~SquareTip(){}

Surface* SquareTip::getMask(int& size){
    Surface* surface = new Surface(size,size);
    std::fill(surface->getBuffer(), surface->getBuffer()+surface->getLength(), 0xFF);
    return surface;
}
