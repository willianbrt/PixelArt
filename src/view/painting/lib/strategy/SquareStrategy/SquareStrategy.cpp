#include "./SquareStrategy.h"

SquareStrategy::~SquareStrategy(){}

SquareStrategy::SquareStrategy(SquareContext* squareContext,  DrawingContext* drawingContext) :
_squareContext(squareContext),
_drawingContext(drawingContext)
{
    _squareContext->isFilled = false;
}
void SquareStrategy::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
    _cursorContext.enable = false;
    _initialized = true;
    _toolRuntimeContext.drawingSession->begin(_toolRuntimeContext.layer);

    pivotPoint = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    _to = pivotPoint;
}
void SquareStrategy::onTracking(int x, int y){
    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    if (to == _to) return;
    _to = to;

    draw();
}
void SquareStrategy::onRelease(){ done(); }

void SquareStrategy::draw(){
    _toolRuntimeContext.drawingSession->clear();

    const Point center = {
        (pivotPoint.x + _to.x) >> 1,
        (pivotPoint.y + _to.y) >> 1
    };
    
    int thinkeness = _drawingContext->getSize();
    const Point diameter = { 
        std::max(std::abs(_to.x - pivotPoint.x), thinkeness-1), 
        std::max(std::abs(_to.y - pivotPoint.y), thinkeness-1)
    };
    
    SquareRasterize square(diameter, center);
    square.filled(_squareContext->isFilled);
    square.thinkenss(thinkeness);
    square.draw(*this);
}
void SquareStrategy::plot(const int& x, const int& y){
    _toolRuntimeContext.drawingSession->putPixel(x, y, _drawingContext->getColor(), &WriteMode::normal);
}

bool SquareStrategy::isInitialized(){
    return _initialized;
}
void SquareStrategy::done() {
    _toolRuntimeContext.drawingSession->commit();
    _initialized = false;
    _cursorContext.enable = true;
}
void SquareStrategy::abort(){
    _toolRuntimeContext.drawingSession->clear();
    _initialized = false;
    _cursorContext.enable = true;
}
CursorContext* SquareStrategy::getCursorContext(){
    _cursorContext.scale = _drawingContext->getSize();
    return &_cursorContext;
}
