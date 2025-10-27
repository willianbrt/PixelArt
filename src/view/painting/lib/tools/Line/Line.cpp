#include "Line.h"

Line::Line(int toX, int toY, int fromX, int fromY, unsigned int newColorHex, unsigned int size){
    _to = Point(toX, toY);
    _from = Point(fromX, fromY);
    _newColorHex = newColorHex;
    _size = size;
}

void Line::draw(Layer& layer){
    if (std::abs(_to.x - _from.x) > std::abs(_to.y - _from.y)) {
        modifiedPixels = drawHorizontalLine(layer);
    }
    else{
        modifiedPixels = drawVerticalLine(layer);
    }
}

vector<Pixel> Line::drawHorizontalLine(Layer& layer){
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
        layer.putPixel(x, y, _newColorHex);
        
        if (D >= 0){
            y+=dir;
            D -= 2*dx;
        } else
            D += 2*dy;
    }

    
    return modifiedPixels;
}
vector<Pixel> Line::drawVerticalLine(Layer& layer){
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
        layer.putPixel(x, y, _newColorHex);

        if (D > 0){
            x+=dir;
            D -= 2*dy;
        } else
            D += 2*dx;
    }
    
    return modifiedPixels;
}

using namespace emscripten;

EMSCRIPTEN_BINDINGS(line_module){
    // register_vector<Pixel>("VectorPixel");
    // emscripten::register_vector<float>("VectorFloat");
    // emscripten::register_vector<std::vector<float>>("VectorVectorFloat");

    class_<Line, base<IGraphic>>("Line")
        .constructor<int, int, int, int, unsigned int, unsigned int>()
        .smart_ptr<std::shared_ptr<Line>>("shared_ptr<Line>")
        .function("draw", &Line::draw);
};
