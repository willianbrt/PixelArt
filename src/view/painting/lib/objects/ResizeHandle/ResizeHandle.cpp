#include "ResizeHandle.h"


ResizeHandle::ResizeHandle(SelectionBox* selectionBox, ENUM_MARKER marker){
    _selectionBox = selectionBox;
    _marker = marker;
}
PointF ResizeHandle::getPosition(Viewport* viewport){
    // printf("h: %f, %f\n", _selectionBox->corners[_marker].x, _selectionBox->corners[_marker].y);
    return viewport->canvasToWorld(_selectionBox->corners[_marker].x, _selectionBox->corners[_marker].y);
}
bool ResizeHandle::isActive(){ return true; }
bool ResizeHandle::hitTest(Point point, Viewport* viewport){
    PointF position = getPosition(viewport);

    float halfHitBox = size*0.5f* viewport->getCanvasSettings()->getScale();
    return (point.x >= position.x - halfHitBox && point.x < position.x + halfHitBox) &&
        (point.y >= position.y - halfHitBox && point.y < position.y + halfHitBox);
}