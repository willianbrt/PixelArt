#include "Erase.h"

Erase::Erase(int toX, int toY, 
    int fromX, int fromY, 
    unsigned int size,
    float strength
) {
    _to = Point(toX, toY);
    _from = Point(fromX, fromY);
    _size = size;
    _strength = strength * 255.0f;
}

void Erase::draw(Layer& layer){
    if (std::abs(_to.x - _from.x) > std::abs(_to.y - _from.y)) {
        modifiedPixels = drawHorizontalErase(layer);
    }
    else{
        modifiedPixels = drawVerticalErase(layer);
    }
}

vector<Pixel> Erase::drawHorizontalErase(Layer& layer){
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

        for(int tx = x; tx < x+_size; tx++){
            for(int ty = y; ty < y+_size; ty++){
                unsigned int oldColor = layer.getPixel(tx, ty);
                float alphaSrc = oldColor & 0xFF;
                unsigned int alphaDst = static_cast<unsigned int>(std::clamp(alphaSrc - _strength, 0.0f, 255.0f));
                unsigned int color = (oldColor & 0xFFFFFF00) | alphaDst;

                layer.putPixel(tx, ty, color);
            }
        }
        
        if (D >= 0){
            y+=dir;
            D -= 2*dx;
        } else
            D += 2*dy;
    }

    
    return modifiedPixels;
}
vector<Pixel> Erase::drawVerticalErase(Layer& layer){
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

        for(int tx = x; tx < x+_size; tx++){
            for(int ty = y; ty < y+_size; ty++){
                unsigned int oldColor = layer.getPixel(tx, ty);
                float alphaSrc = oldColor & 0xFF;
                unsigned int alphaDst = static_cast<unsigned int>(std::clamp(alphaSrc - _strength, 0.0f, 255.0f));
                unsigned int color = (oldColor & 0xFFFFFF00) | alphaDst;

                layer.putPixel(tx, ty, color);
            }
        }

        if (D > 0){
            x+=dir;
            D -= 2*dy;
        } else
            D += 2*dx;
    }
    
    return modifiedPixels;
}

using namespace emscripten;

EMSCRIPTEN_BINDINGS(erase_module){
    class_<Erase, base<IGraphic>>("Erase")
        .constructor<int, int , int, int, unsigned int, float>()
        .smart_ptr<std::shared_ptr<Erase>>("shared_ptr<Erase>")
        .function("draw", &Erase::draw);
};
