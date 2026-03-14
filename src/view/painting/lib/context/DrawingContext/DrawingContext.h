#ifndef DRAWINGCONTEXT_H
#define DRAWINGCONTEXT_H
#include "../../interfaces/IToolContext/IToolContext.h"

class DrawingContext : IToolContext{
public:
    int size = 1;
    unsigned int color;
    float hardness;
    bool isMirrorX;
    bool isMirrorY;
    int nTileX = 1;
    int nTileY = 1;
};

#endif
