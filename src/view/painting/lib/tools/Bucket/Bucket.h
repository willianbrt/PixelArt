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

class Bucket :  public IGraphic {
public:
    Bucket(int x, int y,
        unsigned int newColorHex,
        bool isMirrorX, bool isMirrorY, 
        int nRows, int nCol);

    void draw(Layer& layer);

private:
    void putPixel(Layer& layer, int x, int y, unsigned int color);

    unsigned int _newColorHex;
    vector<unsigned int> modifiedPixels;
    Point _point;
};

#endif