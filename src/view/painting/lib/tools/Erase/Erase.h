#ifndef ERASE 
#define ERASE
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <algorithm>

#include "../../interfaces/ITile/ITile.h"
#include "../../interfaces/IGraphic/IGraphic.h"
#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/Pixel/Pixel.h"
#include "../../objects/layer/Layers.h"

class Erase {
public:
    Erase(Layer& layer, int toX, int toY, int fromX, int fromY, unsigned int size, float strength);

    void draw();

private:
    vector<Pixel> modifiedPixels;
    vector<Pixel> drawHorizontalErase();
    vector<Pixel> drawVerticalErase();
    void stampPixel(Point pixel);
    
    unsigned int _size;
    unsigned int _strength;
    Layer _layer ;
    Point _to, _from;
};

#endif