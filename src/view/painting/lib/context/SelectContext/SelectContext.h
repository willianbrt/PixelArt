#ifndef SELECTCONTEXT_H
#define SELECTCONTEXT_H

// #include "../../interfaces/IToolContext/IToolContext.h"
#include "../../graphics/surface/Surface.h"

struct SelectContext {
private:

public:
SelectContext();
    Surface* data;
    Bounding selectArea;
    Corners corners  = Corners();
};

#endif