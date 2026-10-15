#include "./LineTip.h"

LineTip::LineTip(){}
LineTip::~LineTip(){}
Surface* LineTip::getMask(int& size){
    surface = new Surface(size,size);
    LineRasterize line(
        {size-1,0},
        {0,size-1}
    );
    while(line.hasNext()){
        Point p = line.next();
        surface->putPixel(p.x,p.y,0xFF);
    }
    return surface;
}
void LineTip::plot(const int& x, const int& y) {
    surface->putPixel(x,y,0xFF);
}