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
#include "../../interfaces/ISurface/ISurface.h"
#include "../../interfaces/IGraphic/IGraphic.h"
#include "../../graphics/surface/Surface.h"

using namespace std;
struct ModifedPixel{
    unsigned int index=0;
    unsigned int newColor=0;
    unsigned int oldColor=0;
};
class Preview : public ISurface {
private: 
    Layer* _layer = nullptr;
    size_t _length;
    bool* _dirty = nullptr;
    unsigned int* _newColor = nullptr;
    Bounding dirtyArea;

public:
    Preview(int width, int height);
    ~Preview();

    void putPixel(int x, int y, unsigned int color);
    unsigned int getPixel(int x, int y);
    unsigned int getPixel(unsigned int index);
    unsigned int* getBuffer();

    bool isDirty(unsigned int index);
    Bounding getDirtyArea();
    void setTarget(Layer* layer);
    Layer* getTarget();
    std::vector<ModifedPixel> getModifiedPixels();
    void commit();
    void clear();
};
#endif