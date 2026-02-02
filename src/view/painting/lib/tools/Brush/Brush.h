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
    Brush(int toX, int toY,
         int fromX, int fromY,
         const vector<vector<float>> pattern, unsigned int newColorHex, int size,
         bool isMirrorX, bool isMirrorY, 
         int nRows, int nCols);

    void draw(Layer& layer);

private:
    vector<Pixel> modifiedPixels;
    void drawHorizontalBrush(Layer& layer, Point to, Point from, int screenWidth, int screenHeight);
    void drawVerticalBrush(Layer& layer, Point to, Point from, int screenWidth, int screenHeight);
    void stampPixel(Point pixel, Layer& layer, int screenWidth, int screenHeight);
    void putPixel(Layer& layer, int x, int y, unsigned int color);
    
    unsigned int _newColorHex;
    int _size;
    bool _isTiling;
    Point _to, _from;
    vector<vector<float>> _pattern;
    vector<Point> _points;
};

#endif