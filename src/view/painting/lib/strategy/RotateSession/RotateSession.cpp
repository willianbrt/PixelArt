#include "RotateSession.h"

RotateSession::RotateSession(SelectContext* selection)
: 
_selectionContext(selection)
{
}
ENUM_MARKER RotateSession::hitTest(Point point, Viewport* viewport){
    ENUM_MARKER inside;
    for(int i = 0; i < 4; i++){
        if(_selectionContext->rotateHandle[i].hitTest(point, viewport)) return (ENUM_MARKER)i;        
    }
    return ENUM_MARKER::UNKNOW;
}
bool RotateSession::begin(Point point, Viewport* viewport){
    ENUM_MARKER marker = hitTest(point, viewport);
    if(marker == ENUM_MARKER::UNKNOW) return false;

    _activeMarker = marker;
    _startPoint = viewport->cursorToCanvas(point.x, point.y);
    return true;
}
void RotateSession::update(const Point& mouse){
    PointF dstCenter = _selectionContext->selectionBox.getCenter();

    float radBefore = std::atan2(_startPoint.y - dstCenter.y, _startPoint.x - dstCenter.x);
    float radAfter  = std::atan2(mouse.y - dstCenter.y, mouse.x - dstCenter.x);

    _selectionContext->transformation.setRad(_selectionContext->transformation.getRad() + (radAfter - radBefore));

    const PointF* scale = _selectionContext->transformation.getScale();
    const PointF resized = {
        _selectionContext->srcArea.getWidth()* scale->x,
        _selectionContext->srcArea.getHeight()* scale->y
    };
    PointF axisX = _selectionContext->transformation.rotate({resized.x * 0.5f, 0});
    PointF axisY = _selectionContext->transformation.rotate({0, resized.y * 0.5f});

    for(int i = 0; i < 4; i++){
        _selectionContext->selectionBox.corners[i] = {
            dstCenter.x +
            _selectionContext->selectionBox.sign[i].x * axisX.x +
            _selectionContext->selectionBox.sign[i].y * axisY.x,

            dstCenter.y +
            _selectionContext->selectionBox.sign[i].x * axisX.y +
            _selectionContext->selectionBox.sign[i].y * axisY.y
        };
    }
    

    _startPoint = mouse;
}

    