#include "Brush.h"

Brush::Brush(
    int toX, int toY, 
    int fromX, int fromY, 
    const vector<vector<float>> pattern,
    unsigned int newColorHex,
    int size,
    bool isMirrorX, bool isMirrorY, 
    int nRows, int nCols) : IGraphic(isMirrorX, isMirrorY, nRows, nCols)
{
    _to = Point(toX, toY);
    _from = Point(fromX, fromY);
    _pattern = pattern;
    _newColorHex = newColorHex;
    _size = size;
}
void Brush::draw(Layer& layer){
    const int screenWidth = layer.getWidth()*_nRows;
    const int screenHeight = layer.getHeight()*_nCols;

    if (std::abs(_to.x - _from.x) > std::abs(_to.y - _from.y)) {
        drawHorizontalBrush(layer, _from, _to, screenWidth, screenHeight);
    }
    else{
        drawVerticalBrush(layer, _from, _to, screenWidth, screenHeight);
    }
}

void Brush::drawHorizontalBrush(Layer& layer, Point to, Point from, int screenWidth, int screenHeight){
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
        stampPixel(Point(x, y), layer, screenWidth, screenHeight);
        
        if (D >= 0){
            y+=dir;
            D -= 2*dx;
        }
        D += 2*dy;
    }
}
void Brush::drawVerticalBrush(Layer& layer, Point to, Point from, int screenWidth, int screenHeight){
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
        stampPixel(Point(x, y), layer,  screenWidth, screenHeight);

        if (D > 0){
            x+=dir;
            D -= 2*dy;
        }
        D += 2*dx;
    }
}

void Brush::stampPixel(Point pixel, Layer& layer, int screenWidth, int screenHeight){
    int heightPattern = (int)_pattern.size()*_size;
    int widthPattern = (int)_pattern[0].size()*_size;

    Point startPixel(pixel.x - (widthPattern >> 1), pixel.y - (heightPattern >> 1));
    
    if(startPixel.x >= screenWidth || startPixel.y >= screenHeight) return;
    if(startPixel.x < -widthPattern || startPixel.y < -heightPattern) return;

    Bounding boundingStamp;
    boundingStamp.start.x = startPixel.x < 0 ? 0 : startPixel.x;
    boundingStamp.start.y = startPixel.y < 0 ? 0 : startPixel.y;
    
    boundingStamp.end.x = startPixel.x + widthPattern >= screenWidth ? screenWidth : startPixel.x + widthPattern;  
    boundingStamp.end.y = startPixel.y + heightPattern >= screenHeight ? screenHeight : startPixel.y + heightPattern;

    int startSrcX = startPixel.x < 0 ? -startPixel.x  / _size : 0;
    int startErrX = startPixel.x < 0 ? startPixel.x % widthPattern : 0;
    
    int srcY =  startPixel.y < 0 ? -startPixel.y  / _size : 0;
    int errY = startPixel.y < 0 ? startPixel.y % heightPattern : 0;
    
    for(int y = boundingStamp.start.y; y < boundingStamp.end.y; y ++){
        int srcX = startSrcX;
        int errX = startErrX;
        for(int x =  boundingStamp.start.x; x <  boundingStamp.end.x; x ++){
            float alphaSrc = _pattern[srcY][srcX];
            unsigned int topColor = static_cast<int>(alphaSrc * (_newColorHex >> 24 & 0xFF)) << 24 | (_newColorHex & 0x00FFFFFF);

            Point p;
            p.x = GraphicsEngine::clampedTilePoint(x, layer.getWidth());
            p.y = GraphicsEngine::clampedTilePoint(y, layer.getHeight());
            putPixel(layer, p.x, p.y, topColor);

            errX +=(int) _pattern[0].size();
            if(errX >= widthPattern){
                srcX++;
                errX-=widthPattern;
            }
        }

        errY+= (int)_pattern.size();
        if(errY >= heightPattern){
            srcY++;
            errY-=heightPattern;
        }
    }
}
void Brush::putPixel(Layer& layer, int x, int y, unsigned int color){
    layer.putPixel(x, y,  GraphicsEngine::blendColors(layer.getPixel(x, y), color));
    
    int pointMirrorX = GraphicsEngine::pointMirrored(x, layer.getWidth());
    int pointMirrorY = GraphicsEngine::pointMirrored(y, layer.getHeight());

    if(_isMirrorX){
        layer.putPixel(pointMirrorX, y, GraphicsEngine::blendColors(layer.getPixel(pointMirrorX, y), color));
    }            
    if(_isMirrorY){
        layer.putPixel(x, pointMirrorY, GraphicsEngine::blendColors(layer.getPixel(x, pointMirrorY), color));
    }
    if(_isMirrorX && _isMirrorY){
        layer.putPixel(pointMirrorX, pointMirrorY, GraphicsEngine::blendColors(layer.getPixel(pointMirrorX, pointMirrorY), color));
    }
}

using namespace emscripten;

EMSCRIPTEN_BINDINGS(brush_module){
    register_vector<Pixel>("VectorPixel");
    emscripten::register_vector<float>("VectorFloat");
    emscripten::register_vector<std::vector<float>>("VectorVectorFloat");

    class_<Brush, base<IGraphic>>("Brush")
        .constructor<int, int , int, int, const vector<vector<float>>, unsigned int, int, bool,bool, int, int>()
        .smart_ptr<std::shared_ptr<Brush>>("shared_ptr<Brush>")
        .function("draw", &Brush::draw);
};
