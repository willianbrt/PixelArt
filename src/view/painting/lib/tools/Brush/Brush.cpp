#include "Brush.h"

Brush::Brush(
    int toX, int toY, 
    int fromX, int fromY, 
    const vector<vector<float>> pattern,
    unsigned int newColorHex,
    unsigned int size){
    _to = Point(toX, toY);
    _from = Point(fromX, fromY);
    _pattern = pattern;
    _newColorHex = newColorHex;
    _size = size;
}

void Brush::draw(Layer& layer){
    if (std::abs(_to.x - _from.x) > std::abs(_to.y - _from.y)) {
        modifiedPixels = drawHorizontalBrush(layer);
    }
    else{
        modifiedPixels = drawVerticalBrush(layer);
    }
}

vector<Pixel> Brush::drawHorizontalBrush(Layer& layer){
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

        stampPixel(Point(x, y), layer);
        
        if (D >= 0){
            y+=dir;
            D -= 2*dx;
        }
        D += 2*dy;
    }

    
    return modifiedPixels;
}
vector<Pixel> Brush::drawVerticalBrush(Layer& layer){
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
        stampPixel(Point(x, y), layer);

        if (D > 0){
            x+=dir;
            D -= 2*dy;
        }
        D += 2*dx;
    }
    
    return modifiedPixels;
}

void Brush::stampPixel(Point pixel, Layer& layer){
    const unsigned int heightPattern = _pattern.size()*_size;
    const unsigned int widthPattern = _pattern[0].size()*_size;

    Point startPixel = Point(
        std::round(pixel.x - widthPattern / 2),
        std::round(pixel.y - heightPattern / 2)
    );

    Bounding boundingStamp;
    if(!GraphicsEngine::computeVisibleShape(startPixel.x, widthPattern, layer.getHeight(), boundingStamp.start.x, boundingStamp.end.x) || 
       !GraphicsEngine::computeVisibleShape(startPixel.y, heightPattern, layer.getWidth(), boundingStamp.start.y, boundingStamp.end.y)){
        return;
    }
    
    for(int patternY = 0, y = startPixel.y; patternY < heightPattern; patternY++, y += _size){
        for(int patternX = 0, x = startPixel.x; patternX < widthPattern; patternX++, x += _size){
            float alphaSrc = _pattern[patternY][patternX];
            unsigned int topColor = (_newColorHex & 0xFFFFFF00) | static_cast<int>(alphaSrc * (_newColorHex & 0xFF));

            for(int sy = std::max(0, boundingStamp.start.y - y); sy < std::min<int>(_size, boundingStamp.end.y - y); sy++){
                for(int sx = std::max(0, boundingStamp.start.x - x); sx < std::min<int>(_size, boundingStamp.end.x - x); sx++){
                    Point p = Point(x + sx, y + sy);

                    unsigned int oldColor = layer.getPixel(p.x, p.y);
                    unsigned int color = GraphicsEngine::blendColors(oldColor, topColor);
                    layer.putPixel(p.x, p.y, color);
                    modifiedPixels.emplace_back(p, oldColor);
                }
            }
        }
    }
}

using namespace emscripten;

EMSCRIPTEN_BINDINGS(brush_module){
    register_vector<Pixel>("VectorPixel");
    emscripten::register_vector<float>("VectorFloat");
    emscripten::register_vector<std::vector<float>>("VectorVectorFloat");

    class_<Brush, base<IGraphic>>("Brush")
        .constructor<int, int , int, int, const vector<vector<float>>, unsigned int, unsigned int>()
        .smart_ptr<std::shared_ptr<Brush>>("shared_ptr<Brush>")
        .function("draw", &Brush::draw);
};
