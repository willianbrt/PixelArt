#include "SelectContext.h"

SelectContext::SelectContext()
{

}

Point SelectContext::cornerRotate(Viewport* viewport,  Point cornerPosition){
    CanvasSettings* settings = viewport->getCanvasSettings();
    PointF c = {
        (corners.bottomLeft.x + corners.bottomRight.x + corners.topLeft.x+ corners.topRight.x) * 0.25f, 
        (corners.bottomLeft.y + corners.bottomRight.y + corners.topLeft.y+ corners.topRight.y) * 0.25f
    };

    PointF dir = {
        (c.x - cornerPosition.x),
        (c.y - cornerPosition.y)   
    };
    float dist = std::sqrtf(dir.x*dir.x + dir.y*dir.y);
    
    PointF corner = viewport->canvasToWorld(
        cornerPosition.x + (dir.x / dist),
        cornerPosition.y + (dir.y / dist)
    );
    return {
        (int)std::floor(corner.x + 0.5f),
        (int)std::floor(corner.y + 0.5f)
    };
}