#ifndef PREVIEW_H
#define PREVIEW_H

#include <emscripten.h>
#include <emscripten/bind.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#include "../layer/Layers.h"

#include "../../helpers/Guid/Guid.h"

#include "../../interfaces/ITile/ITile.h"
#include "../../interfaces/IGraphic/IGraphic.h"
#include "../../graphics/surface/Surface.h"

using namespace std;
struct ModifedPixel{
    unsigned int index;
    unsigned int newColor;
    unsigned int oldColor;
};
class Preview {
private: 
    Layer* _layer;
    bool* _dirtyArea;
    std::vector<ModifedPixel> _updatedPixels;

public:
    Preview(Layer* layer);
    ~Preview();

    Layer* getPtrLayer() const;
    unsigned int getPixelFiltered(int index);
    unsigned int getPixel(int x, int y);
    unsigned int getPixel(unsigned int index);
    void putPixel(int x, int y, unsigned int color);
    void putPixel(unsigned int index, unsigned int color);
    bool isDirty(unsigned int index);
    std::vector<ModifedPixel> getModifiedPixels();
    void commit();
    void uncommit();
};
#endif