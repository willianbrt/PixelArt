#include "Line.h"

Line::Line(Layer& layer, 
    Point to, 
    Point from, 
    const vector<vector<float>> pattern,
    unsigned int newColorHex, unsigned int size) : _layer(layer){
    _to = to;
    _from = from;
    _pattern = pattern;
    _newColorHex = newColorHex;
    _size = size;
}
Line::Line(Layer& layer, 
    int toX, int toY, 
    int fromX, int fromY, 
    const vector<vector<float>> pattern,
    unsigned int newColorHex,
    unsigned int size) : _layer(layer){
    _layer=layer;
    _to = Point(toX, toY);
    _from = Point(fromX, fromY);
    _pattern = pattern;
    _newColorHex = newColorHex;
    _size = size;
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
        // unsigned int oldColor = _layer.getPixel(x, y);
        // modifiedPixels.emplace_back(Point(x, y), oldColor);
        // _layer.putPixel(x, y, 
        // );

        stampPixel(Point(x, y));
        
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
        // unsigned int oldColor = _layer.getPixel(x, y);
        // modifiedPixels.emplace_back(Point(x, y), oldColor);
        // _layer.putPixel(x, y, _newColorHex);
        stampPixel(Point(x, y));

        if (D > 0){
            x+=dir;
            D -= 2*dy;
        } else
            D += 2*dx;
    }
    
    return modifiedPixels;
}

void Line::stampPixel(Point pixel){
    const unsigned int heightPattern = _pattern.size()*_size;
    const unsigned int widthPattern = _pattern[0].size()*_size;

    Point startPixel = Point(
        std::round(pixel.x - widthPattern / 2),
        std::round(pixel.y - heightPattern / 2)
    );

    Bounding boundingStamp;
    if(!GraphicsEngine::computeVisibleShape(startPixel.x, widthPattern, _layer.getHeight(), boundingStamp.start.x, boundingStamp.end.x) || 
       !GraphicsEngine::computeVisibleShape(startPixel.y, heightPattern, _layer.getWidth(), boundingStamp.start.y, boundingStamp.end.y)){
        return;
    }
    
    for(int patternY = 0, y = startPixel.y; patternY < heightPattern; patternY++, y += _size){
        for(int patternX = 0, x = startPixel.x; patternX < widthPattern; patternX++, x += _size){
            float alphaSrc = _pattern[patternY][patternX];
            unsigned int topColor = (_newColorHex & 0xFFFFFF00) | static_cast<int>(alphaSrc * (_newColorHex & 0xFF));

            for(int sy = std::max(0, boundingStamp.start.y - y); sy < std::min<int>(_size, boundingStamp.end.y - y); sy++){
                for(int sx = std::max(0, boundingStamp.start.x - x); sx < std::min<int>(_size, boundingStamp.end.x - x); sx++){
                    Point p = Point(x + sx, y + sy);

                    unsigned int oldColor = _layer.getPixel(p.x, p.y);
                    unsigned int color = GraphicsEngine::blendColors(oldColor, topColor);
                    _layer.putPixel(p.x, p.y, color);
                    modifiedPixels.emplace_back(p, oldColor);
                }
            }
        }
    }
}

using namespace emscripten;

EMSCRIPTEN_BINDINGS(line_module){
    register_vector<Pixel>("VectorPixel");
    emscripten::register_vector<float>("VectorFloat");
    emscripten::register_vector<std::vector<float>>("VectorVectorFloat");

    class_<Line>("Line")
        .constructor<Layer&, int, int , int, int, const vector<vector<float>>, unsigned int, unsigned int>()
        .smart_ptr<std::shared_ptr<Line>>("shared_ptr<Line>")
        .function("draw", &Line::draw);
        // .function("drawHorizontalLine", &Line::drawHorizontalLine)
        // .function("drawVerticalLine", &Line::drawVerticalLine);
};
