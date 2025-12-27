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

class Erase : public IGraphic {
public:
    Erase(int toX, int toY, int fromX, int fromY, unsigned int size, float strength);

    void draw(Layer& layer);

private:
    vector<Pixel> modifiedPixels;
    vector<Pixel> drawHorizontalErase(Layer& layer);
    vector<Pixel> drawVerticalErase(Layer& layer);
    void stampPixel(Point pixel,Layer& layer);
    
    unsigned int _size;
    unsigned int _strength;
    Point _to, _from;
};

#endif