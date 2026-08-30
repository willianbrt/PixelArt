#include "./CircleStrategy.h"

CircleStrategy::~CircleStrategy(){}

CircleStrategy::CircleStrategy(CircleContext* circleContext,  DrawingContext* drawingContext) :
_circleContext(circleContext),
_drawingContext(drawingContext)
{
    _drawingContext->color = 0xff0000ff;
    _drawingContext->size = 3;
    _drawingContext->hardness = 1.0f;

    _circleContext->isFilled = false;
}
void CircleStrategy::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
    _cursorContext.enable = false;
    _initialized = true;
    _toolRuntimeContext.drawingSession->begin(_toolRuntimeContext.layer);

    pivotPoint = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    _to = pivotPoint;
}
void CircleStrategy::onTracking(int x, int y){
    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    if (to == _to) return;
    _to = to;

    draw();
}
void CircleStrategy::onRelease(){ done(); }

void CircleStrategy::draw(){
    _toolRuntimeContext.drawingSession->clear();

    const Point center = {
        (pivotPoint.x + _to.x) >> 1,
        (pivotPoint.y + _to.y) >> 1
    };
    
    const Point diameter = { 
        std::max(std::abs(_to.x - pivotPoint.x), _drawingContext->size-1), 
        std::max(std::abs(_to.y - pivotPoint.y), _drawingContext->size-1)
    };
    
    CircleRasterize circle(diameter, center);
    circle.filled(_circleContext->isFilled);
    circle.thinkenss(_drawingContext->size);
    circle.draw(*this);
}
void CircleStrategy::plot(const int& x, const int& y){
    _toolRuntimeContext.drawingSession->blendMirroredPixel(x, y, _drawingContext->color);
}

bool CircleStrategy::isInitialized(){
    return _initialized;
}
void CircleStrategy::done() {
    _toolRuntimeContext.drawingSession->commit();
    _initialized = false;
    _cursorContext.enable = true;
}
void CircleStrategy::abort(){
    _toolRuntimeContext.drawingSession->clear();
    _initialized = false;
    _cursorContext.enable = true;
}
CursorContext* CircleStrategy::getCursorContext(){
    _cursorContext.scale = _drawingContext->size;
    return &_cursorContext;
}
