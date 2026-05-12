#ifndef SELECTCONTEXT_H
#define SELECTCONTEXT_H

// #include "../../interfaces/IToolContext/IToolContext.h"
#include "../../graphics/surface/Surface.h"
#include "../../objects/Viewport/Viewport.h"
#include "../../graphics/Transformation/Transformation.h"

struct SelectContext {
private:

public:
SelectContext();
    Surface* data;
    Bounding selectArea;
    Transformation transformation;

    Corners corners  = Corners();
    Corners cornersRotate  = Corners();
        
    PointF cornerRotate(Viewport* viewport, PointF cornerPosition);
};

#endif