#ifndef LINE 
#define LINE
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

#include "../interfaces/ITile/ITile.h"
#include "../interfaces/IGraphic/IGraphic.h"
#include "../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../graphics/Pixel/Pixel.h"
#include "../objects/layer/Layers.h"

class Line {
public:
    Line(Layer& layer, Point to, Point from, unsigned int newColorHex);
    Line(Layer& layer, int toX, int toY, int fromX, int fromY, unsigned int newColorHex);
    // Line(Layer& layer, Point from, int width, int deg, unsigned int newColorHex)

    void draw();
    void addPoint(int fromX, int fromY);

private:
    vector<Pixel> modifiedPixels;
    vector<Pixel> drawHorizontalLine();
    vector<Pixel> drawVerticalLine();
    
    unsigned int _newColorHex;
    Layer _layer ;
    Point _to, _from;
};

#endif