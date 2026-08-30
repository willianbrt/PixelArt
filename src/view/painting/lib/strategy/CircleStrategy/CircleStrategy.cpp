#include "./CircleStrategy.h"

inline int cross(const Point& A, const Point& B, const Point& P) {
    return (B.x - A.x)*(P.y - A.y) - (B.y - A.y)*(P.x - A.x);
}
CircleStrategy::~CircleStrategy(){}


CircleStrategy::CircleStrategy(CircleContext* circleContext,  DrawingContext* drawingContext, SymmetryContext* symmetryContext) :
_circleContext(circleContext),
_drawingContext(drawingContext),
_symmetryContext(symmetryContext)
{
    _drawingContext->color = 0xff0000ff;
    _drawingContext->size = 3;
    _drawingContext->hardness = 1.0f;

    _symmetryContext->isMirrorX =false;
    _symmetryContext->isMirrorY =false;
    _circleContext->isFilled = false;
}
void CircleStrategy::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
    _cursorContext.enable = false;
    _initialized = true;
    _toolRuntimeContext.drawingSession->begin(_toolRuntimeContext.layer);

    pivotPoint = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);

    _flagPoint = pivotPoint;
}
void CircleStrategy::onTracking(int x, int y){
    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    if (to == _flagPoint) return;

    _toolRuntimeContext.drawingSession->clear();

    Point center = {
        (pivotPoint.x + to.x ) >> 1,
        (pivotPoint.y + to.y ) >> 1
    };
    
    const Point diameter = { 
        std::abs(to.x - pivotPoint.x), 
        std::abs(to.y - pivotPoint.y)
        // std::max(std::abs(to.x - pivotPoint.x), _drawingContext->size-1), 
        // std::max(std::abs(to.y - pivotPoint.y), _drawingContext->size-1)
    };
    
    const Point radius = { 
        diameter.x >> 1, 
        diameter.y >> 1
    };
    CircleRasterize tcircle(diameter);

    auto draw4 = [&](const int& x, const int& y, const unsigned int& color){
        _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x + x, center.y + y, color, _symmetryContext);
    };

    tcircle.draw(draw4);
    _flagPoint = to;
}
void CircleStrategy::onRelease(){ done(); }

void CircleStrategy::draw(const Point& pixel){
}

void CircleStrategy::traceSymetricOutline(const Point& from, const Point& to, const Point& center) {
}

void CircleStrategy::traceFilled(const Point& from, const Point& center) {
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
