#ifndef ITOOL_H
#define ITOOL_H

#include "../../graphics/Pixel/Pixel.h"
#include "../IToolContext/IToolContext.h"

class Layer;

class ITool{
protected:
public:
    virtual ~ITool() = default;
    virtual IToolContext* getContext() = 0;
    virtual void draw(Layer& layer) = 0;
};

#endif