#ifndef BUCKET_H 
#define BUCKET_H
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <algorithm>

#include "../../interfaces/ITile/ITile.h"
#include "../../interfaces/IGraphic/IGraphic.h"
#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/Pixel/Pixel.h"
#include "../../objects/layer/Layers.h"

class Bucket : public IGraphic {
public:
    Bucket(unsigned int x, unsigned int y, unsigned int newColorHex);

    void draw(Layer& layer);

private:
    unsigned int _newColorHex;
    vector<unsigned int> modifiedPixels;
    Point _point;
};

#endif