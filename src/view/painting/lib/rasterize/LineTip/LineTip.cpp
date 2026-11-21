#include "./LineTip.h"

LineTip::LineTip(){
    int size = 1;
    setSize(size);
}
LineTip::~LineTip(){}

TYPE_BRUSH_TIP LineTip::getType() {
    return TYPE_BRUSH_TIP::GEOMTRIC;
}


Surface* LineTip::getMask(){
    return _surface;
}
void LineTip::plot(const int& x, const int& y) {
    _surface->putPixel(x,y,0xFF);
}

void LineTip::setSize(int& size){
    _surface = new Surface(size,size);
    LineRasterize line(
        {size-1,0},
        {0,size-1}
    );
    while(line.hasNext()){
        Point p = line.next();
        _surface->putPixel(p.x,p.y,0xFF);
    }

    _size = size;
}
void LineTip::setRotate(float& rad) {
    _rad = rad;
    _transform.setRad(rad);
}
int LineTip::getSize(){
    return _size;
}
float LineTip::getRotate(){
    return _rad;
}
Transformation LineTip::getTransformation(){
    return _transform;
}