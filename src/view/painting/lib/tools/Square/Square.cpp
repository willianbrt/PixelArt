#include "Square.h"

inline void traceFilled(Layer& layer, int cx, int cy, int x, int y, unsigned int color) {
    if(x<=0) return;
    for (int xi = cx - x + 1 ; xi < cx + x; xi++) {
        layer.putPixel(xi, cy + y, color);
        layer.putPixel(xi, cy - y, color);
    }
}

Square::Square(int from_x, int from_y,
                int to_x, int to_y,
                bool isFilled, int thickness,
                unsigned int newColorHex){
    _fromPoint = Point(from_x,from_y);
    _toPoint = Point(to_x,to_y);

    if(_toPoint.x < _fromPoint.x) std::swap(_fromPoint.x, _toPoint.x);
    if(_toPoint.y < _fromPoint.y) std::swap(_fromPoint.y, _toPoint.y);

    _newColorHex = newColorHex;
    _thickness = thickness;
    _isFilled = isFilled;

    cx = (_toPoint.x + _fromPoint.x) >> 1;
    cy = (_toPoint.y + _fromPoint.y) >> 1;

}
void Square::draw(Layer& layer) {
// EXTERNO
    const int rx = ((_toPoint.x - _fromPoint.x) >> 1);
    const int ry = ((_toPoint.y - _fromPoint.y) >> 1);

    for(int x = 0; x <= rx; x++){
        for(int t = 0; t < _thickness; t++){
            layer.putPixel(_fromPoint.x + x, _fromPoint.y + t, 0xFFFF00FF);
            layer.putPixel(_fromPoint.x + x, _toPoint.y - t, 0x00FF00FF);
            layer.putPixel(_toPoint.x - x, _fromPoint.y + t, 0xFFFF00FF);
            layer.putPixel(_toPoint.x - x, _toPoint.y - t, 0x00FF00FF);
        }
    }

    for(int y = _thickness; y <= ry; y++){
        for(int t = 0; t < _thickness; t++){
            layer.putPixel(_fromPoint.x + t, _fromPoint.y + y, 0xFF00FFFF);
            layer.putPixel(_toPoint.x - t, _fromPoint.y + y, 0x0000FFFF);

            layer.putPixel(_fromPoint.x + t, _toPoint.y - y, 0xFF00FFFF);
            layer.putPixel(_toPoint.x - t, _toPoint.y - y, 0x0000FFFF);
        }
    }

    if(_isFilled){
        for(int y = _fromPoint.y + _thickness; y <= _toPoint.y - _thickness; y++){
            for(int x = _fromPoint.x+_thickness; x <= _toPoint.x - _thickness; x++){
                layer.putPixel(x, y, 0xFFFFFFFF);
            }
        }
    }
}

using namespace emscripten;

EMSCRIPTEN_BINDINGS(square_module){
    class_<Square, base<IGraphic>>("Square")
        .constructor<int, int, int, int, bool, int, unsigned int>()
        .smart_ptr<std::shared_ptr<Square>>("shared_ptr<Square>")
        .function("draw", &Square::draw);
};
