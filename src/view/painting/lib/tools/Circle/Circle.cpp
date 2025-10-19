#include "Circle.h"

Circle::Circle(Layer& layer, 
                int from_x, int from_y, 
                int to_x, int to_y,
                unsigned int newColorHex) : _layer(layer){
    _fromPoint = Point(from_x,from_y);
    _toPoint = Point(to_x,to_y);
    _newColorHex = newColorHex;
}
void Circle::draw() {
    int cx = (_toPoint.x + _fromPoint.x) / 2;
    int cy = (_toPoint.y + _fromPoint.y) / 2;
    
    int dx = _toPoint.x - _fromPoint.x;
    int dy = _toPoint.y - _fromPoint.y;
    int r = static_cast<int>(sqrt(dx*dx + dy*dy) / 2.0);

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
        .constructor<Layer&, int, int, int, int, unsigned int>()
        .smart_ptr<std::shared_ptr<Circle>>("shared_ptr<Circle>")
        .function("draw", &Circle::draw);
};
