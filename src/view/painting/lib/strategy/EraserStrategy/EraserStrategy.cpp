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
    LineRasterize line(_from, pixel);
    while(line.hasNext()){
        Point tracking = line.next();

        ISurface* shape = _drawingContext->shape->getMask();
                
        StampRasterize stamp({(float)tracking.x + 0.5f, (float)tracking.y + 0.5f},
            {shape->getWidth(), shape->getHeight()},
            _toolRuntimeContext.drawingAreaSize,
            _drawingContext->shape->getTransformation());

        while(stamp.hasNext()){
            Point it = stamp.next();
            Point src = stamp.getSrcPoint();

            if(src.x < 0 || src.y < 0 || src.x >= shape->getWidth() || src.y >= shape->getHeight()) continue;
            
            int shapeMask = (shape->getPixel(src.x, src.y) & 0xFF);
            if(shapeMask == 0) continue;

            _toolRuntimeContext.drawingSession->putPixel(it.x, it.y, static_cast<int>((1.0f - shapeMask)*_drawingContext->hardness), &WriteMode::eraser);
        }
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
    return &_cursorContext;
}