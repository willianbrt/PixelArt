#ifndef ITILE_H
#define ITILE_H

#include "../../helpers/Pixel/Pixel.h"

class IGraphic;

class ITile{
public:
    virtual ~ITile() = default;
    virtual void resize(int width, int height) = 0;
    virtual void move(int x, int y) = 0;
    virtual void draw(IGraphic& graphic) = 0;
    virtual unsigned int getPixel(int x, int y) = 0;
};

#endif