#include "TranslateSession.h"

TranslateSession::TranslateSession(SelectContext* selection)
: 
_selectionContext(selection)
{
}
bool TranslateSession::begin(Point point, Viewport* viewport){
    if( _selectionContext->selectionBox.isInsideRotatedBounding(point)) return false;

    _startPoint = viewport->cursorToCanvas(point.x, point.y);
    return true;
}
void TranslateSession::update(const Point& mouse){
    Point delta = {
        mouse.x - _startPoint.x,
        mouse.y - _startPoint.y
    };

    for(int i = 0; i < 4; i++){
        _selectionContext->selectionBox.corners[i].x += delta.x;
        _selectionContext->selectionBox.corners[i].y += delta.y;
    }

    _startPoint = mouse;
}

    