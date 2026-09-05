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
    moveTo({
        transform->pan.x + (to.x - _from.x),
        transform->pan.y + (to.y - _from.y),
    });
    _from = to;
}
void PanStrategy::onRelease(){
    done();
}

void PanStrategy::moveTo(Point p){
    CanvasTransform* transform = &_toolRuntimeContext.canvasSettings->canvasTransform;
    
    int viewportWidth  = _toolRuntimeContext.viewport->width;
    int viewportHeight = _toolRuntimeContext.viewport->height;    

    Point initialPosition = getInitialPosition();

    int currentWidth = _toolRuntimeContext.editor->getWidth()*transform->scale;
    int currentHeight= _toolRuntimeContext.editor->getHeight()*transform->scale;

    int minLeftOffset, minTopOffset;
    int maxLeftOffset , maxTopOffset;

    if(currentWidth <= viewportWidth){
        minLeftOffset = 0;
        maxLeftOffset = _toolRuntimeContext.viewport->width - currentWidth;
    } else {
        maxLeftOffset = initialPosition.x;
        minLeftOffset = _toolRuntimeContext.viewport->width - currentWidth - maxLeftOffset;
    }

    if(currentHeight <= viewportHeight){
        minTopOffset = 0;
        maxTopOffset = _toolRuntimeContext.viewport->height - currentHeight;
    } else {
        maxTopOffset = initialPosition.y;
        minTopOffset = _toolRuntimeContext.viewport->height - currentHeight - maxTopOffset;
    }

    transform->pan.x = std::max(minLeftOffset, std::min(maxLeftOffset, p.x));
    transform->pan.y  =  std::max(minTopOffset, std::min(maxTopOffset, p.y));
}
Point PanStrategy::getInitialPosition(){
    int viewportWidth  = _toolRuntimeContext.viewport->width;
    int viewportHeight = _toolRuntimeContext.viewport->height;

    Point p = {0,0};
    int minScale = std::floor(std::min(viewportWidth /  _toolRuntimeContext.editor->getWidth(), viewportHeight / _toolRuntimeContext.editor->getHeight()));

    p.x = std::floor((viewportWidth - ( _toolRuntimeContext.editor->getWidth()*minScale)) / 2);
    p.y = std::floor((viewportHeight - (_toolRuntimeContext.editor->getHeight()*minScale)) / 2);
    return p;
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