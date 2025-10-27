#ifndef CIRCLE_H 
#define CIRCLE_H
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <algorithm>
#include <functional>

#include "../../interfaces/ITile/ITile.h"
#include "../../interfaces/IGraphic/IGraphic.h"
#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/Pixel/Pixel.h"
#include "../../objects/layer/Layers.h"
#include "../Bucket/Bucket.h"

class Circle : public IGraphic{
public:
    Circle(int from_x, int from_y, int to_x, int to_y,
            bool isFilled, int thickness,
            unsigned int newColorHex);

    void draw(Layer& layer);

private:
    int cx, cy;
    bool _isFilled;
    int _thickness;
    std::function<void(int, int)> trace;

    unsigned int _newColorHex;
    vector<unsigned int> modifiedPixels;
    // Layer _layer;
    Point _fromPoint;
    Point _toPoint;
};

#endif