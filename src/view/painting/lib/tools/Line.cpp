#include "Line.h"

Line::Line(Layer& layer, Point to, Point from, unsigned int newColorHex) : _layer(layer){
    _to = to;
    _from = from;
    _newColorHex = newColorHex;
}
Line::Line(Layer& layer, int toX, int toY, int fromX, int fromY, unsigned int newColorHex) : _layer(layer){
    _layer=layer;
    _to = Point(toX, toY);
    _from = Point(fromX, fromY);
    _newColorHex = newColorHex;
}
// Line::Line(Layer& layer, Point from, int width, int deg, unsigned int newColorHex) : _layer(layer){
//     _to = Point(abs((width + from.x) * tan(deg)), abs((width + from.y) * tan(deg)));;
//     _from = from;
//     _layer = layer;
//     _newColorHex = newColorHex;
// }

void Line::draw(){
    if (std::abs(_to.x - _from.x) > std::abs(_to.y - _from.y)) {
        modifiedPixels = drawHorizontalLine();
    }
    else{
        modifiedPixels = drawVerticalLine();
    }
}
void Line::addPoint(int toX, int toY){
    if (modifiedPixels.empty()) return;

    // Último ponto desenhado
    Point last = modifiedPixels.back().point;

    // Atualiza os pontos da linha
    _from = last;
    _to = Point(toX, toY);

    // Número de pontos intermediários
    int steps = std::max(std::abs(_to.x - _from.x), std::abs(_to.y - _from.y));
    float stepX = float(_to.x - _from.x) / steps;
    float stepY = float(_to.y - _from.y) / steps;

    // Suavização: Adicionando os pontos intermediários
    vector<Pixel> newPixels;
    for (int i = 1; i <= steps; i++) {
        int x = std::round(_from.x + stepX * i);
        int y = std::round(_from.y + stepY * i);

        unsigned int oldColor = _layer.getPixel(x, y);
        newPixels.emplace_back(Point(x, y), oldColor);
        _layer.putPixel(x, y, _newColorHex);
    }

    // Acumula os novos pixels desenhados
    modifiedPixels.insert(modifiedPixels.end(), newPixels.begin(), newPixels.end());
}



vector<Pixel> Line::drawHorizontalLine(){
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
        unsigned int oldColor = _layer.getPixel(x, y);
        modifiedPixels.emplace_back(Point(x, y), oldColor);
        _layer.putPixel(x, y, _newColorHex);

        if (D >= 0){
            y+=dir;
            D -= 2*dx;
        } else
            D += 2*dy;
    }

    
    return modifiedPixels;
}
vector<Pixel> Line::drawVerticalLine(){
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
        unsigned int oldColor = _layer.getPixel(x, y);
        modifiedPixels.emplace_back(Point(x, y), oldColor);
        _layer.putPixel(x, y, _newColorHex);
        
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
    register_vector<Pixel>("VectorPixel");
    class_<Line>("Line")
        .constructor<Layer&, int, int , int, int, unsigned int>()
        .smart_ptr<std::shared_ptr<Line>>("shared_ptr<Line>")
        .function("draw", &Line::draw)
        .function("addPoint", &Line::addPoint);
        // .function("drawHorizontalLine", &Line::drawHorizontalLine)
        // .function("drawVerticalLine", &Line::drawVerticalLine);
};
