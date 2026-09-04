#ifndef CANVASTRANSFORM_H
#define CANVASTRANSFORM_H
#include "../../interfaces/IToolContext/IToolContext.h"
#include "../../graphics/Pixel/Pixel.h"

class CanvasTransform : public IToolContext{
public:
    Point pan = { 0, 0 };
    float scale = 1.0f;
};

#endif
