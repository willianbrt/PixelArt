#include "Square.h"

Square::Square(
    int from_x, int from_y,
    int to_x, int to_y,
    bool isFilled, int thickness,
    unsigned int newColorHex,
    bool isMirrorX, bool isMirrorY, 
    int nRows, int nCols) : IGraphic(isMirrorX, isMirrorY, nRows, nCols)
{
    _fromPoint = Point(from_x,from_y);
    _toPoint = Point(to_x,to_y);

    if(_toPoint.x < _fromPoint.x) std::swap(_fromPoint.x, _toPoint.x);
    if(_toPoint.y < _fromPoint.y) std::swap(_fromPoint.y, _toPoint.y);

    _newColorHex = newColorHex;
    _thickness = thickness;
    _isFilled = isFilled;
}
void Square::draw(Layer& layer) {
    const int screenWidth = layer.getWidth()*_nRows;
    const int screenHeight = layer.getHeight()*_nCols;
    
    const int rx = ((_toPoint.x - _fromPoint.x) >> 1);
    const int ry = ((_toPoint.y - _fromPoint.y) >> 1);

    for(int x = 0; x <= rx; x++){
        for(int t = 0; t < _thickness; t++){
            putPixel(layer, _fromPoint.x + x, _fromPoint.y + t, _newColorHex, screenWidth, screenHeight);
            putPixel(layer, _fromPoint.x + x, _toPoint.y - t, _newColorHex, screenWidth, screenHeight);
            putPixel(layer, _toPoint.x - x, _fromPoint.y + t, _newColorHex, screenWidth, screenHeight);
            putPixel(layer, _toPoint.x - x, _toPoint.y - t, _newColorHex, screenWidth, screenHeight);
        }
    }

    for(int y = _thickness; y <= ry; y++){
        for(int t = 0; t < _thickness; t++){
            putPixel(layer, _fromPoint.x + t, _fromPoint.y + y, _newColorHex, screenWidth, screenHeight);
            putPixel(layer, _fromPoint.x + t, _toPoint.y - y, _newColorHex, screenWidth, screenHeight);
            putPixel(layer, _toPoint.x - t, _fromPoint.y + y, _newColorHex, screenWidth, screenHeight);
            putPixel(layer, _toPoint.x - t, _toPoint.y - y, _newColorHex, screenWidth, screenHeight);
        }
    }
    
    if(_isFilled){
        for(int y = _fromPoint.y + _thickness; y <= _toPoint.y - _thickness; y++){
            for(int x = _fromPoint.x+_thickness; x <= _toPoint.x - _thickness; x++){
                putPixel(layer, x, y, _newColorHex, screenWidth, screenHeight);
            }
        }
    }
}

void Square::putPixel(Layer& layer, int x, int y, unsigned int color, int screenWidth, int screenHeight){
    if(x >= screenWidth || y >= screenHeight || x < 0 || y < 0) return;

    Point p;
    p.x = GraphicsEngine::clampedTilePoint(x, layer.getWidth());
    p.y = GraphicsEngine::clampedTilePoint(y, layer.getHeight());

    layer.putPixel(p.x, p.y, color);
    
    int pointMirrorX = GraphicsEngine::pointMirrored(p.x, layer.getWidth());
    int pointMirrorY = GraphicsEngine::pointMirrored(p.y, layer.getHeight());

    if(_isMirrorX){
        layer.putPixel(pointMirrorX, p.y, color);
    }            
    if(_isMirrorY){
        layer.putPixel(p.x, pointMirrorY, color);
    }
    if(_isMirrorX && _isMirrorY){
        layer.putPixel(pointMirrorX, pointMirrorY, color);
    }
}

using namespace emscripten;

EMSCRIPTEN_BINDINGS(square_module){
    class_<Square, base<IGraphic>>("Square")
        .constructor<int, int, int, int, bool, int, unsigned int, bool, bool, int, int>()
        .smart_ptr<std::shared_ptr<Square>>("shared_ptr<Square>")
        .function("draw", &Square::draw);
};
