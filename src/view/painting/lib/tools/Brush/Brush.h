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

class Brush : public IGraphic {
public:
    Brush(int toX, int toY, int fromX, int fromY, const vector<vector<float>> pattern, unsigned int newColorHex, int size);

    void draw(Layer& layer);

private:
    vector<Pixel> modifiedPixels;
    vector<Pixel> drawHorizontalBrush(Layer& layer);
    vector<Pixel> drawVerticalBrush(Layer& layer);
    void stampPixel(Point pixel, Layer& layer);
    
    unsigned int _newColorHex;
    int _size;
    Point _to, _from;
    vector<vector<float>> _pattern;
};

#endif