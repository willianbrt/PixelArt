#ifndef LINE 
#define LINE
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <algorithm>

#include "../interfaces/ITile/ITile.h"
#include "../interfaces/IGraphic/IGraphic.h"
#include "../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../graphics/Pixel/Pixel.h"
#include "../objects/layer/Layers.h"

using Brush = std::vector<std::vector<float>>;
class Line {
public:
    Line(Layer& layer, Point to, Point from, const vector<vector<float>> pattern, unsigned int newColorHex, unsigned int size);
    Line(Layer& layer, int toX, int toY, int fromX, int fromY, const vector<vector<float>> pattern, unsigned int newColorHex, unsigned int size);
    // Line(Layer& layer, Point from, int width, int deg, unsigned int newColorHex)

    void draw();

private:
    vector<Pixel> modifiedPixels;
    vector<Pixel> drawHorizontalLine();
    vector<Pixel> drawVerticalLine();
    void stampPixel(Point pixel);
    
    unsigned int _newColorHex;
    unsigned int _size;
    Layer _layer ;
    Point _to, _from;
    vector<vector<float>> _pattern;
};

#endif