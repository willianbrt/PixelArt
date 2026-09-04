#include "RotateHandle.h"


RotateHandle::RotateHandle(SelectionBox* selectionBox, ENUM_MARKER marker){
    _selectionBox = selectionBox;
    _marker = marker;
}
PointF RotateHandle::getPosition(CanvasSettings* viewport){
    PointF c = _selectionBox->getCenter();
    // printf("s:%f,%f\n",_selectionBox->corners[0].x, _selectionBox->corners[0].y);
    // printf("s:%f,%f\n",c.x, c.y);
    PointF dir = {
        (c.x - _selectionBox->corners[_marker].x),
        (c.y - _selectionBox->corners[_marker].y)
    };
    float dist = std::sqrtf(dir.x*dir.x + dir.y*dir.y);
    return (dist != 0) ? 
            viewport->canvasToWorld(_selectionBox->corners[_marker].x + (dir.x / dist), _selectionBox->corners[_marker].y + (dir.y / dist)) :
            viewport->canvasToWorld(_selectionBox->corners[_marker].x, _selectionBox->corners[_marker].y);
}
bool RotateHandle::isActive(){ return true; }
bool RotateHandle::hitTest(Point point, CanvasSettings* viewport){
    PointF position = getPosition(viewport);

    float halfHitBox = size*0.5f* viewport->canvasTransform.scale;
    return (point.x >= position.x - halfHitBox && point.x < position.x + halfHitBox) &&
        (point.y >= position.y - halfHitBox && point.y < position.y + halfHitBox);
}