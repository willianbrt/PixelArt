#include "Circle.h"

Circle::Circle(Layer& layer, unsigned int x, unsigned int y, unsigned int newColorHex) : _layer(layer){
    _point = Point(x,y);
    _newColorHex = newColorHex;
}
void Circle::draw(int cx, int cy, int r) {
    int x = 0;
    int y = -r;
    int p = -r;

    while(x < -y) {

        if(p > 0){
            y+=1;
            p+=2*(x+y)+1;
        }else{
            p+=2*x+1;
        }

        _layer.putPixel(cx + x, cy + y, _newColorHex);
        _layer.putPixel(cx - x, cy + y, _newColorHex);
        _layer.putPixel(cx + x, cy - y, _newColorHex);
        _layer.putPixel(cx - x, cy - y, _newColorHex);
        _layer.putPixel(cx + y, cy + x, _newColorHex);
        _layer.putPixel(cx + y, cy - x, _newColorHex);
        _layer.putPixel(cx - y, cy + x, _newColorHex);
        _layer.putPixel(cx - y, cy - x, _newColorHex);
        
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
