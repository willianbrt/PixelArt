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
class Preview : ISurface {
private: 
    Layer* _layer;
    std::vector<bool> _dirty;
    std::vector<unsigned int> _newColor;

public:
    Preview(Layer* layer);
    ~Preview();

    unsigned int getPixel(int x, int y);
    unsigned int getPixel(unsigned int index) override;
    void putPixel(int x, int y, unsigned int color);
    void putPixel(unsigned int index, unsigned int color) override;
    bool isDirty(unsigned int index);
    std::vector<ModifedPixel> getModifiedPixels();
    void commit();
};
#endif