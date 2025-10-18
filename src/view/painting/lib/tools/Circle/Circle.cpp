#include "Circle.h"

Circle::Circle(Layer& layer, unsigned int x, unsigned int y, unsigned int newColorHex) : _layer(layer){
    _point = Point(x,y);
    _newColorHex = newColorHex;
}
void Circle::draw(unsigned int cx_u, unsigned int cy_u, unsigned int r_u) {
    
    int cx = static_cast<int>(cx_u);
    int cy = static_cast<int>(cy_u);
    int r  = static_cast<int>(r_u);

    int x = 0;
    int y = -r;
    int p = -r;

    const unsigned int _width = 32;
    const unsigned int _height = 32;
    while(x < -y) {

        if(p > 0){
            y+=1;
            p+=2*(x+y)+1;
        }else{
            p+=2*x+1;
        }

        _layer.putPixel(cx + _point.x, cy + _point.y, _newColorHex);
        _layer.putPixel(cx - _point.x, cy + _point.y, _newColorHex);
        _layer.putPixel(cx + _point.x, cy - _point.y, _newColorHex);
        _layer.putPixel(cx - _point.x, cy - _point.y, _newColorHex);
        _layer.putPixel(cx + _point.y, cy + _point.x, _newColorHex);
        _layer.putPixel(cx + _point.y, cy - _point.x, _newColorHex);
        _layer.putPixel(cx - _point.y, cy + _point.x, _newColorHex);
        _layer.putPixel(cx - _point.y, cy - _point.x, _newColorHex);
        
        x+=1;
    }
}


using namespace emscripten;

EMSCRIPTEN_BINDINGS(circle_module){
    class_<Circle>("Circle")
        .constructor<Layer&, unsigned int,unsigned int,unsigned int>()
        .smart_ptr<std::shared_ptr<Circle>>("shared_ptr<Circle>")
        .function("draw", &Circle::draw);
};
