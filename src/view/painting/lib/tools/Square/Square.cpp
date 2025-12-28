#include "Square.h"

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
    const int rx = ((_toPoint.x - _fromPoint.x) >> 1);
    const int ry = ((_toPoint.y - _fromPoint.y) >> 1);

    for(int x = 0; x <= rx; x++){
        for(int t = 0; t < _thickness; t++){
            layer.putPixel(_fromPoint.x + x, _fromPoint.y + t, _newColorHex);
            layer.putPixel(_fromPoint.x + x, _toPoint.y - t, _newColorHex);
            layer.putPixel(_toPoint.x - x, _fromPoint.y + t, _newColorHex);
            layer.putPixel(_toPoint.x - x, _toPoint.y - t, _newColorHex);
        }
    }

    for(int y = _thickness; y <= ry; y++){
        for(int t = 0; t < _thickness; t++){
            layer.putPixel(_fromPoint.x + t, _fromPoint.y + y, _newColorHex);
            layer.putPixel(_toPoint.x - t, _fromPoint.y + y, _newColorHex);

            layer.putPixel(_fromPoint.x + t, _toPoint.y - y, _newColorHex);
            layer.putPixel(_toPoint.x - t, _toPoint.y - y, _newColorHex);
        }
    }

    if(_isFilled){
        for(int y = _fromPoint.y + _thickness; y <= _toPoint.y - _thickness; y++){
            for(int x = _fromPoint.x+_thickness; x <= _toPoint.x - _thickness; x++){
                layer.putPixel(x, y, _newColorHex);
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
