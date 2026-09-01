#include "BrushStrategy.h"

BrushStrategy::BrushStrategy(BrushContext* brushContext, DrawingContext* drawingContext) :
_brushContext(brushContext),
_drawingContext(drawingContext)
{
    _brushContext->setActivePattern("brush_1");
    _brushContext->transformation.setScale({1.0f, 1.0f});
    _brushContext->transformation.setRad(0  * M_PI / 180);
}
void BrushStrategy::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
    _cursorContext.enable = false;
    _initialized = true;
    _toolRuntimeContext.drawingSession->begin(_toolRuntimeContext.layer);

    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);

    draw(to);

    _from = to;
}
void BrushStrategy::onTracking(int x, int y){
    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    if (to == _from) return;

    LineRasterize line(_from, to);
    while(line.hasNext()){
        Point p = line.next();
        draw(p);
    }

    _from = to;
}
void BrushStrategy::onRelease(){
    done();
}

void BrushStrategy::draw(const Point& pixel){
    StampRasterize stamp({(float)pixel.x + 0.5f, (float)pixel.y + 0.5f},
        {_brushContext->selectedPattern->width, _brushContext->selectedPattern->height},
        {_toolRuntimeContext.screenWidth, _toolRuntimeContext.screenHeight},
        _brushContext->transformation);

    while(stamp.hasNext()){
        Point it = stamp.next();
        Point src = stamp.getSrcPoint();

        if(src.x < 0 || src.y < 0 || src.x > _brushContext->selectedPattern->width || src.y > _brushContext->selectedPattern->height) continue;

        unsigned int topColor = _drawingContext->color;
        GraphicsEngine::setOpacity(topColor, (_brushContext->selectedPattern->buffer[src.y* _brushContext->selectedPattern->width + src.x] & 0xFF) / 255.0f);

        _toolRuntimeContext.drawingSession->blendMirroredPixel(it.x, it.y, topColor);
    }
}
bool BrushStrategy::isInitialized(){
    return _initialized;
}
void BrushStrategy::done() {
    _toolRuntimeContext.drawingSession->commit();
    _initialized = false;
    _cursorContext.enable = true;
}
void BrushStrategy::abort(){
    _toolRuntimeContext.drawingSession->clear();
    _initialized = false;
    _cursorContext.enable = true;
}
CursorContext* BrushStrategy::getCursorContext(){
    _cursorContext.pattern = _brushContext->selectedPattern;
    _cursorContext.scale = _drawingContext->size;
    return &_cursorContext;
}