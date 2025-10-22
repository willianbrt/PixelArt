#include "Circle.h"

Circle::Circle(int from_x, int from_y, 
                int to_x, int to_y,
                bool isFilled, int thickness,
                unsigned int newColorHex){
    _fromPoint = Point(from_x,from_y);
    _toPoint = Point(to_x,to_y);
    _newColorHex = newColorHex;
    _thickness = thickness;
    _isFilled = isFilled;

    cx = (_toPoint.x + _fromPoint.x) / 2;
    cy = (_toPoint.y + _fromPoint.y) / 2;

    rx = (_toPoint.x - _fromPoint.x) / 2;
    ry = (_toPoint.y - _fromPoint.y) / 2;
    
    rx2 = rx * rx;
    ry2 = ry * ry;

}
void Circle::draw(Layer& layer) {
    if(_isFilled){
        trace = [&](int x, int y){
            for (int xi = cx - x; xi <= cx + x; xi++) {
                layer.putPixel(xi, cy + y, _newColorHex);
                layer.putPixel(xi, cy - y, _newColorHex);
            }
        };
    }else{
        trace = [&](int x, int y){
            for (int t = 0; t < _thickness; t++) {
                layer.putPixel(cx + x + t, cy + y, _newColorHex);
                layer.putPixel(cx - x - t, cy + y, _newColorHex);
                layer.putPixel(cx + x + t, cy - y, _newColorHex);
                layer.putPixel(cx - x - t, cy - y, _newColorHex);

                layer.putPixel(cx + x, cy + y + t, _newColorHex);
                layer.putPixel(cx - x, cy + y + t, _newColorHex);
                layer.putPixel(cx + x, cy - y - t, _newColorHex);
                layer.putPixel(cx - x, cy - y - t, _newColorHex);
            }
        };
    }


    int x = 0;
    int y = ry;
    
    int p = ry2 - (rx2 * ry) + (rx2 / 4);

    int px = 0;
    int py = 2 * rx2 * y;

    while(px < py) {
        trace(x, y);
        
        x++;
        px += 2 * ry2;

        if (p < 0) {
            p += ry2 + px;
        } else {
            y--;
            py -= 2 * rx2;
            p += ry2 + px - py;
        }
    }
    p = (int)(ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2 * ry2);

    while (y >= 0) {
        trace(x, y);
        
        y--;
        py -= 2 * rx2;

        if (p > 0) {
            p += rx2 - py;
        } else {
            x++;
            px += 2 * ry2;
            p += rx2 - py + px;
        }
    }
}


using namespace emscripten;

EMSCRIPTEN_BINDINGS(circle_module){
    class_<Circle, base<IGraphic>>("Circle")
        .constructor<int, int, int, int, bool,int, unsigned int>()
        .smart_ptr<std::shared_ptr<Circle>>("shared_ptr<Circle>")
        .function("draw", &Circle::draw);
};
