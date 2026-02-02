#ifndef ERASE 
#define ERASE
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <algorithm>

#include "../../interfaces/ITile/ITile.h"
#include "../../interfaces/IGraphic/IGraphic.h"
#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/Pixel/Pixel.h"
#include "../../objects/layer/Layers.h"

class Erase : public IGraphic {
public:
    Erase(int toX, int toY, 
        int fromX, int fromY, 
        unsigned int size, float strength,
        bool isMirrorX, bool isMirrorY, 
        int nRows, int nCol);

    void draw(Layer& layer);

private:
    vector<Pixel> modifiedPixels;
    vector<Pixel> drawHorizontalErase(Layer& layer, int screenWidth, int screenHeight);
    vector<Pixel> drawVerticalErase(Layer& layer, int screenWidth, int screenHeight);
    void putPixel(Layer& layer, int x, int y, unsigned int color);
    
    unsigned int _size;
    unsigned int _strength;
    Point _to, _from;
};

#endif