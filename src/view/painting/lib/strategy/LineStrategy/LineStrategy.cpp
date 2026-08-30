#include "LineStrategy.h"

LineStrategy::LineStrategy(LineContext* lineContext, DrawingContext* drawingContext) :
_lineContext(lineContext),
_drawingContext(drawingContext)
{
    _drawingContext->color = 0xff0000ff;
    _drawingContext->size = 1;
    _drawingContext->hardness = 1.0f;
}
void LineStrategy::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
    _cursorContext.enable = false;
    _initialized = true;
    _toolRuntimeContext.drawingSession->begin(_toolRuntimeContext.layer);

    _from = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    draw(_from);

}
void LineStrategy::onTracking(int x, int y){
    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    if (to == _from) return;

    draw(to);
}
void LineStrategy::onRelease(){
    done();
}

void LineStrategy::draw(const Point& pixel){
    _toolRuntimeContext.drawingSession->clear();
    LineRasterize line(_from, pixel);
    while(line.hasNext()){
        Point p = line.next();
        _toolRuntimeContext.drawingSession->blendMirroredPixel(p.x, p.y, _drawingContext->color);
    }
}
bool LineStrategy::isInitialized(){
    return _initialized;
}
void LineStrategy::done() {
    _toolRuntimeContext.drawingSession->commit();
    _initialized = false;
    _cursorContext.enable = true;
}
void LineStrategy::abort(){
    _toolRuntimeContext.drawingSession->clear();
    _initialized = false;
    _cursorContext.enable = true;
}
CursorContext* LineStrategy::getCursorContext(){
    _cursorContext.scale = _drawingContext->size;
    return &_cursorContext;
}