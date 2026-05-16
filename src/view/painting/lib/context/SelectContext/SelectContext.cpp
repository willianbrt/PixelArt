#include "SelectContext.h"

SelectContext::SelectContext()
{

}

PointF SelectContext::cornerRotate(Viewport* viewport,  PointF cornerPosition){
    CanvasSettings* settings = viewport->getCanvasSettings();
    PointF c = selectionBox.getCenter();

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
std::array<float, 8> SelectContext::getAllHandle(Viewport* viewport){
    std::array<float, 8> vec; 
    for(int i = 0; i < 8; i+=2){
        PointF point = handle[i >> 1].getPosition(viewport);
        vec[i] = point.x;
        vec[i+1] = point.y;
    }
    return vec;
}