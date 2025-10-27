#ifndef BRUSH 
#define BRUSH
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <algorithm>

#include "../../interfaces/ITile/ITile.h"
#include "../../interfaces/IGraphic/IGraphic.h"
#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/Pixel/Pixel.h"
#include "../../objects/layer/Layers.h"

class Brush {
public:
    Brush(Layer& layer, int toX, int toY, int fromX, int fromY, const vector<vector<float>> pattern, unsigned int newColorHex, unsigned int size);

    void draw();

private:
    vector<Pixel> modifiedPixels;
    vector<Pixel> drawHorizontalBrush();
    vector<Pixel> drawVerticalBrush();
    void stampPixel(Point pixel);
    
    unsigned int _newColorHex;
    unsigned int _size;
    Layer _layer ;
    Point _to, _from;
    vector<vector<float>> _pattern;
};

#endif