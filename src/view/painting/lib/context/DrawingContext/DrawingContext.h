#ifndef DRAWINGCONTEXT_H
#define DRAWINGCONTEXT_H
#include "../../interfaces/IToolContext/IToolContext.h"

class DrawingContext : public IToolContext{
public:
    int size = 1;
    unsigned int color = 0x000000FF;
    float hardness = 1.0f;
};

#endif
