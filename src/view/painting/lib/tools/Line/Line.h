#ifndef LINE 
#define LINE
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <algorithm>

#include "../../interfaces/ITile/ITile.h"
#include "../../interfaces/IGraphic/IGraphic.h"
#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/Pixel/Pixel.h"
#include "../../objects/layer/Layers.h"

class Line : public IGraphic{
public:
    Line(int toX, int toY, int fromX, int fromY, unsigned int newColorHex, unsigned int size);

    void draw(Layer& layer);

private:
    vector<Pixel> modifiedPixels;
    vector<Pixel> drawHorizontalLine(Layer& layer);
    vector<Pixel> drawVerticalLine(Layer& layer);
    void stampPixel(Point pixel);
    
    unsigned int _newColorHex;
    unsigned int _size;
    Point _to, _from;
};

#endif