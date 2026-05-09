#ifndef SELECTCONTEXT_H
#define SELECTCONTEXT_H

// #include "../../interfaces/IToolContext/IToolContext.h"
#include "../../graphics/surface/Surface.h"
#include "../../objects/Viewport/Viewport.h"

struct SelectContext {
private:

public:
SelectContext();
    Surface* data;
    Bounding selectArea;
    Corners corners  = Corners();
    Corners cornersRotate  = Corners();
        
    Point cornerRotate(Viewport* viewport, Point cornerPosition);
};

#endif