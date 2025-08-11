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

    Point startPixel = Point(
        std::round(pixel.x - ((_pattern.size()) / 2 * _size)),
        std::round(pixel.y - ((_pattern[0].size()) / 2* _size))
    );

    unsigned int heightPattern = _pattern.size()*_size;
    unsigned int widthPattern = _pattern[0].size()*_size;

    Bounding boundingStamp;
    if(!GraphicsEngine::computeVisibleShape(startPixel.x, widthPattern, 32, boundingStamp.start.x, boundingStamp.end.x) || 
       !GraphicsEngine::computeVisibleShape(startPixel.y, heightPattern, 32, boundingStamp.start.y, boundingStamp.end.y)){
        return;
    }

    Point flagStartPixel = boundingStamp.start;

    int y = boundingStamp.start.y;
    for(int i = 0; i < _pattern.size(); i++){
        auto patternLine = _pattern[i];
        
        int x = flagStartPixel.x;

        for(int j = 0; j < _pattern[0].size(); j++){
            float alphaSrc = patternLine[j];
            unsigned int topColor = (_newColorHex & 0xFFFFFF00) | static_cast<int>(alphaSrc * (_newColorHex & 0xFF) );
            unsigned int bottomColor = _layer.getPixel(x, y);
            unsigned int color = GraphicsEngine::blendColors(bottomColor, topColor);

            // unsigned int color =(((_newColorHex >> 24) & 0xFF) << 24) |
            //                     (((_newColorHex >> 16) & 0xFF) << 16) |
            //                     (((_newColorHex >> 8) & 0xFF) << 8) |
            //                    (static_cast<int>(alphaSrc * (_newColorHex & 0xFF)));
            // unsigned int color = _newColorHex;
            // GraphicsEngine::blendColor(color, alphaSrcBrush);
            
            
            unsigned int oldColor = _layer.getPixel(x, y);
            for(int sy = 0; sy < _size; sy++){
                for(int sx = 0; sx < _size; sx++){
                    _layer.putPixel(x+sx, y+sy, color);
                    modifiedPixels.emplace_back(Point(x+sx, y+sy), oldColor);
                }
            }

            x++;
        }
        y++;
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
