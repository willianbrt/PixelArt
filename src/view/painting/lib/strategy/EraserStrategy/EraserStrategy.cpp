#include "EraserStrategy.h"

EraserStrategy::EraserStrategy(EraserContext* eraserContext, DrawingContext* drawingContext) :
_eraserContext(eraserContext),
_drawingContext(drawingContext)
{
}
void EraserStrategy::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
    _cursorContext.enable = false;
    _initialized = true;
    _toolRuntimeContext.drawingSession->begin(_toolRuntimeContext.layer);

    _from = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);

    draw(_from);
}
void EraserStrategy::onTracking(int x, int y){
    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    if (to == _from) return;

    draw(to);

    _from = to;
}
void EraserStrategy::onRelease(){
    done();
}

void EraserStrategy::draw(const Point& pixel){
    unsigned int hardness = static_cast<unsigned int>((1.0 - _drawingContext->hardness)*255.0f);
    LineRasterize line(_from, pixel);
    while(line.hasNext()){
        Point p = line.next();
        _toolRuntimeContext.drawingSession->putPixel(p.x, p.y, hardness, &WriteMode::eraser);
    }
}
bool EraserStrategy::isInitialized(){
    return _initialized;
}
void EraserStrategy::done() {
    _toolRuntimeContext.drawingSession->commit();
    _initialized = false;
    _cursorContext.enable = true;
}
void EraserStrategy::abort(){
    _toolRuntimeContext.drawingSession->clear();
    _initialized = false;
    _cursorContext.enable = true;
}
CursorContext* EraserStrategy::getCursorContext(){
    _cursorContext.scale = _drawingContext->size;
    return &_cursorContext;
}