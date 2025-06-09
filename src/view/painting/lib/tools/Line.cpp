#ifndef LINE 
#define LINE
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

#include "../interfaces/ITile/ITile.h"
#include "../interfaces/IGraphic/IGraphic.h"
#include "../graphics/GraphicsEngine/GraphicsEngine.cpp"
#include "../graphics/Pixel/Pixel.h"
#include "../objects/layer/Layers.h"

class Line {
public:
    Line(Layer& layer, Point to, Point from, unsigned int newColorHex) : _layer(layer){
        _to = to;
        _from = from;
        _newColorHex = newColorHex;
    }
    Line(Layer& layer, int toX, int toY, int fromX, int fromY, unsigned int newColorHex) : _layer(layer){
        _layer=layer;
        _to = Point(toX, toY);
        _from = Point(fromX, fromY);
        _newColorHex = newColorHex;
        printf("%u \n",_newColorHex);
    }
    // Line(Layer& layer, Point from, int width, int deg, unsigned int newColorHex){
    //     to = Point(abs((width + from.x) * tan(deg)), abs((width + from.y) * tan(deg)));;
    //     from = from;
    //     layer = layer;
    //     newColorHex = newColorHex;
    // }

    void draw(){
        vector<Pixel> modifiedPixels;
        
        if (std::abs(_to.x - _from.x) > std::abs(_to.y - _from.y)) {
            modifiedPixels = drawHorizontalLine();
        }
        else{
            modifiedPixels = drawVerticalLine();
        }

    }

private:
    vector<Pixel> drawHorizontalLine(){
        vector<Pixel> modifiedPixels;

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
            // printf("%i,%i \n", x, y);
            
            if (D >= 0){
                y+=dir;
                D -= 2*dx;
            } else
                D += 2*dy;
        }
    
        
        return modifiedPixels;
    }

    vector<Pixel> drawVerticalLine(){
        vector<Pixel> modifiedPixels;

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
    unsigned int _newColorHex;
    Layer _layer ;
    Point _to, _from;
};

using namespace emscripten;

EMSCRIPTEN_BINDINGS(line_module){
    register_vector<Pixel>("VectorPixel");
    class_<Line>("Line")
        .constructor<Layer&, int, int , int, int, unsigned int>()
        .smart_ptr<std::shared_ptr<Line>>("shared_ptr<Line>")
        .function("draw", &Line::draw);
        // .function("drawHorizontalLine", &Line::drawHorizontalLine)
        // .function("drawVerticalLine", &Line::drawVerticalLine);
};

#endif