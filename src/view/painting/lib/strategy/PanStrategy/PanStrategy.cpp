#include "./PanStrategy.h"

PanStrategy::PanStrategy(){
    
}

PanStrategy::~PanStrategy()
{
}
void PanStrategy::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
    _cursorContext.enable = false;
    _initialized = true;

    _from = {x,y};
}
void PanStrategy::onTracking(int x, int y){
    Point to = {x,y};
    CanvasTransform* transform = &_toolRuntimeContext.canvasSettings->canvasTransform;
    _toolRuntimeContext.canvasSettings->pan(
        {
            transform->pan.x + (to.x - _from.x),
            transform->pan.y + (to.y - _from.y),
        },
        _toolRuntimeContext.viewport, _toolRuntimeContext.editor->getSurface());

    _from = to;
}
void PanStrategy::onRelease(){
    done();
}

bool PanStrategy::isInitialized(){
    return _initialized;
}
void PanStrategy::done() {
    _initialized = false;
    _cursorContext.enable = true;
}
void PanStrategy::abort(){
    _toolRuntimeContext.drawingSession->clear();
    _initialized = false;
    _cursorContext.enable = true;
}
CursorContext* PanStrategy::getCursorContext(){
    return &_cursorContext;
}