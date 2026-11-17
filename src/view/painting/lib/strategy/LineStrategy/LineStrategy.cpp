#include "LineStrategy.h"

LineStrategy::LineStrategy(LineContext* lineContext, DrawingContext* drawingContext) :
_lineContext(lineContext),
_drawingContext(drawingContext)
{
}
void LineStrategy::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
    _cursorContext.enable = false;
    _initialized = true;
    _toolRuntimeContext.drawingSession->begin(_toolRuntimeContext.layer);

    _pivot = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    _to = _pivot;
}
void LineStrategy::onTracking(int x, int y){
    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    if (to == _to) return;
    _to = to;

    draw();
}
void LineStrategy::onRelease(){
    done();
}

void LineStrategy::draw(){
    _toolRuntimeContext.drawingSession->clear();
    int thinkness = _drawingContext->getSize();
    int color = _drawingContext->getColor();

    LineRasterize line(_pivot, _to);
    Point start = {
        INT_MAX,
        INT_MAX
    };
    Point end = {
        INT_MIN,
        INT_MIN
    };
    while(line.hasNext()){
        Point p = line.next();

        int half = (thinkness >> 1);
        for(int x = 0; x < thinkness; x++){
            int px = p.x - half + x;
            for(int y = 0; y < thinkness; y++){
                int py = p.y - half + y;

                if(px >= start.x && px <= end.x &&
                py >= start.y && py <= end.y) continue;

                _toolRuntimeContext.drawingSession->putPixel(px, py, color, &WriteMode::normal);
            }
        }
        
        start = {
            p.x-half,
            p.y-half
        };
        end = {
            start.x+(thinkness)-1,
            start.y+(thinkness)-1
        };
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
    _cursorContext.scale = _drawingContext->getSize();
    return &_cursorContext;
}