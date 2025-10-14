#ifndef CIRCLE_H 
#define CIRCLE_H
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <algorithm>

#include "../../interfaces/ITile/ITile.h"
#include "../../interfaces/IGraphic/IGraphic.h"
#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/Pixel/Pixel.h"
#include "../../objects/layer/Layers.h"

class Circle {
public:
    Circle(Layer& layer, unsigned int x, unsigned int y, unsigned int newColorHex);

    void draw(unsigned int cx_u, unsigned int cy_u, unsigned int r_u);
    // void draw(unsigned int cx, unsigned int cy, unsigned int r);

private:
    unsigned int _newColorHex;
    vector<unsigned int> modifiedPixels;
    Layer _layer ;
    Point _point;
};

#endif