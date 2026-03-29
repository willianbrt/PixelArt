#ifndef DRAWINGCONTEXT_H
#define DRAWINGCONTEXT_H
#include "../../interfaces/IToolContext/IToolContext.h"

class DrawingContext : public IToolContext{
public:
    int size = 1;
    unsigned int color;
    float hardness;
};

#endif
