#include "Line.h"

Line::Line(int toX, int toY, 
    int fromX, int fromY, 
    unsigned int newColorHex, unsigned int size,
    bool isMirrorX, bool isMirrorY, 
    int nRows, int nCols) : IGraphic(isMirrorX, isMirrorY, nRows, nCols)
{
    _to = Point(toX, toY);
    _from = Point(fromX, fromY);
    _newColorHex = newColorHex;
    _size = size;
}

void Line::draw(Layer& layer){
    const int screenWidth = layer.getWidth()*_nRows;
    const int screenHeight = layer.getHeight()*_nCols;
    
    if (std::abs(_to.x - _from.x) > std::abs(_to.y - _from.y)) {
        modifiedPixels = drawHorizontalLine(layer, screenWidth, screenHeight);
    }
    else{
        modifiedPixels = drawVerticalLine(layer, screenWidth, screenHeight);
    }
}

vector<Pixel> Line::drawHorizontalLine(Layer& layer, int screenWidth, int screenHeight){
    if(_to.x < _from.x){
        std::swap(_to, _from);
    }
    
    int dx = _to.x - _from.x;
    int dy = _to.y - _from.y;

    int dir = (dy < 0) ? -1 : 1;
    dy = std::abs(dy); 

    int D = 2*dy - dx;
    int y = _from.y;
    
    for(int x = _from.x; x <= _to.x; x++){
        if(x >= screenWidth || y >= screenHeight || x < 0 || y < 0) continue;
        
        Point clampedPoint;
        clampedPoint.x = GraphicsEngine::clampedTilePoint(x, layer.getWidth());
        clampedPoint.y = GraphicsEngine::clampedTilePoint(y, layer.getHeight());

        putPixel(layer, clampedPoint.x, clampedPoint.y, _newColorHex);
        
        if (D >= 0){
            y+=dir;
            D -= 2*dx;
        }
        D += 2*dy;
    }

    return modifiedPixels;
}


vector<Pixel> Line::drawVerticalLine(Layer& layer, int screenWidth, int screenHeight){
    if(_to.y < _from.y){
        std::swap(_to, _from);
    }
    
    int dx = _to.x - _from.x;
    int dy = _to.y - _from.y;

    int dir = (dx < 0) ? -1 : 1;
    dx = std::abs(dx); 

    int D = 2*dx - dy;
    int x = _from.x;
    
    for(int y = _from.y; y <= _to.y; y++){
        if(x >= screenWidth || y >= screenHeight || x < 0 || y < 0) continue;

        Point clampedPoint;
        clampedPoint.x = GraphicsEngine::clampedTilePoint(x, layer.getWidth());
        clampedPoint.y = GraphicsEngine::clampedTilePoint(y, layer.getHeight());

        putPixel(layer, clampedPoint.x, clampedPoint.y, _newColorHex);

        if (D > 0){
            x+=dir;
            D -= 2*dy;
        }
        D += 2*dx;
    }
    
    return modifiedPixels;
}

void Line::putPixel(Layer& layer, int x, int y, unsigned int color){

    layer.putPixel(x, y, color);
    
    int pointMirrorX = GraphicsEngine::pointMirrored(x, layer.getWidth());
    int pointMirrorY = GraphicsEngine::pointMirrored(y, layer.getHeight());

    if(_isMirrorX){
        layer.putPixel(pointMirrorX, y, color);
    }            
    if(_isMirrorY){
        layer.putPixel(x, pointMirrorY, color);
    }
    if(_isMirrorX && _isMirrorY){
        layer.putPixel(pointMirrorX, pointMirrorY, color);
    }
}

using namespace emscripten;

EMSCRIPTEN_BINDINGS(line_module){
    // register_vector<Pixel>("VectorPixel");
    // emscripten::register_vector<float>("VectorFloat");
    // emscripten::register_vector<std::vector<float>>("VectorVectorFloat");

    class_<Line, base<IGraphic>>("Line")
        .constructor<int, int, int, int, unsigned int, unsigned int, bool, bool, int, int>()
        .smart_ptr<std::shared_ptr<Line>>("shared_ptr<Line>")
        .function("draw", &Line::draw);
};
