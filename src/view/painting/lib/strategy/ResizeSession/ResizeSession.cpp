#include "ResizeSession.h"

ResizeSession::ResizeSession(SelectContext* selection)
: 
_selectionContext(selection)
{
}
ENUM_MARKER ResizeSession::hitTest(Point point, Viewport* viewport){
    ENUM_MARKER inside;
    for(int i = 0; i < 4; i++){
        if(_selectionContext->handle[i].hitTest(point, viewport)) return (ENUM_MARKER)i;        
    }
    return ENUM_MARKER::UNKNOW;
}
bool ResizeSession::begin(Point point, Viewport* viewport){
    ENUM_MARKER marker = hitTest(point, viewport);
    if(marker == ENUM_MARKER::UNKNOW) return false;

    _activeMarker = marker;
    _startPoint = viewport->cursorToCanvas(point.x, point.y);

    switch (_activeMarker) {
        case ENUM_MARKER::TOP_LEFT:
            pivot   = &_selectionContext->selectionBox.corners[ENUM_MARKER::BOTTOM_RIGHT];
            dragged = &_selectionContext->selectionBox.corners[ENUM_MARKER::TOP_LEFT];
            cornerH = &_selectionContext->selectionBox.corners[ENUM_MARKER::TOP_RIGHT];
            cornerW = &_selectionContext->selectionBox.corners[ENUM_MARKER::BOTTOM_LEFT];
            break;

        case ENUM_MARKER::TOP_RIGHT:
            pivot   = &_selectionContext->selectionBox.corners[ENUM_MARKER::BOTTOM_LEFT];
            dragged = &_selectionContext->selectionBox.corners[ENUM_MARKER::TOP_RIGHT];
            cornerH = &_selectionContext->selectionBox.corners[ENUM_MARKER::TOP_LEFT];
            cornerW = &_selectionContext->selectionBox.corners[ENUM_MARKER::BOTTOM_RIGHT];
            break;

        case ENUM_MARKER::BOTTOM_RIGHT:
            pivot   = &_selectionContext->selectionBox.corners[ENUM_MARKER::TOP_LEFT];
            dragged = &_selectionContext->selectionBox.corners[ENUM_MARKER::BOTTOM_RIGHT];
            cornerH = &_selectionContext->selectionBox.corners[ENUM_MARKER::BOTTOM_LEFT];
            cornerW = &_selectionContext->selectionBox.corners[ENUM_MARKER::TOP_RIGHT];
            break;

        case ENUM_MARKER::BOTTOM_LEFT:
            pivot   = &_selectionContext->selectionBox.corners[ENUM_MARKER::TOP_RIGHT];
            dragged = &_selectionContext->selectionBox.corners[ENUM_MARKER::BOTTOM_LEFT];
            cornerH = &_selectionContext->selectionBox.corners[ENUM_MARKER::BOTTOM_RIGHT];
            cornerW = &_selectionContext->selectionBox.corners[ENUM_MARKER::TOP_LEFT];
            break;
        default:
            throw std::runtime_error("Corner inválido");
    }
    return true;
}
void ResizeSession::update(const Point& mouse){
    PointF pixel = {
        (float)mouse.x,
        (float)mouse.y
    };

    PointF unrotate = _selectionContext->transformation.unrotate(_selectionContext->transformation.distance(pixel, *pivot));
    PointF resized = {
        _selectionContext->selectionBox.sign[_activeMarker].x *  unrotate.x,
        _selectionContext->selectionBox.sign[_activeMarker].y * unrotate.y
    };

    *cornerW = _selectionContext->transformation.fromWidth(_selectionContext->selectionBox.sign[_activeMarker].x * resized.x, *pivot);
    *cornerH = _selectionContext->transformation.fromHeight(_selectionContext->selectionBox.sign[_activeMarker].y * resized.y, *pivot);

    (*dragged) = {
        cornerH->x + cornerW->x - pivot->x,
        cornerH->y + cornerW->y - pivot->y
    };

    _selectionContext->transformation.setScale({
         resized.x / (float)_selectionContext->srcArea.getWidth(),
         resized.y / (float)_selectionContext->srcArea.getHeight()
    });

    _startPoint = mouse;
}

    