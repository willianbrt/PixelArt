#include "SelectContext.h"

SelectContext::SelectContext()
{

}

PointF SelectContext::cornerRotate(Viewport* viewport,  PointF cornerPosition){
    CanvasSettings* settings = viewport->getCanvasSettings();
    PointF c = corners.getCenter();

    PointF dir = {
        (c.x - cornerPosition.x),
        (c.y - cornerPosition.y)   
    };
    float dist = std::sqrtf(dir.x*dir.x + dir.y*dir.y);
    
    PointF corner = (dist != 0) ? viewport->canvasToWorld(
        cornerPosition.x + (dir.x / dist),
        cornerPosition.y + (dir.y / dist)
    ) : cornerPosition;
    return {
        corner.x,
        corner.y
    };
}