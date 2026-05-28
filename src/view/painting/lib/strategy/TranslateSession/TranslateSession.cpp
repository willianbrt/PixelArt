#include "TranslateSession.h"

TranslateSession::TranslateSession(SelectContext* selection)
: 
_selectionContext(selection)
{
}
bool TranslateSession::begin(Point point, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
    if(!_selectionContext->selectionBox.isInsideRotatedBounding(_toolRuntimeContext.canvasSettings->cursorToCanvas(point.x, point.y)  )) return false;

    _startPoint = _toolRuntimeContext.canvasSettings->cursorToCanvas(point.x, point.y);
    return true;
}
void TranslateSession::update(const Point& mouse){
    Point localDelta = {
        mouse.x - _startPoint.x,
        mouse.y - _startPoint.y
    };
    const PointF* delta = _selectionContext->transformation.getDelta();

    _selectionContext->transformation.setDelta({
        delta->x + localDelta.x,
        delta->y + localDelta.y
    });

    for(int i = 0; i < 4; i++){
        _selectionContext->selectionBox.corners[i].x += localDelta.x;
        _selectionContext->selectionBox.corners[i].y += localDelta.y;
    } 

    _startPoint = mouse;
}

    