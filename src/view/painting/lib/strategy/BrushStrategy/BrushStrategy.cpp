#include "BrushStrategy.h"

BrushStrategy::BrushStrategy(BrushContext* brushContext, DrawingContext* drawingContext, SymmetryContext* symmetryContext) :
_brushContext(brushContext),
_drawingContext(drawingContext),
_symmetryContext(symmetryContext)
{
    _brushContext->setActivePattern("brush_1");

    _drawingContext->color = 0xff0000ff;
    _drawingContext->size = 1;
    _drawingContext->hardness = 1.0f;

    _brushContext->transformation.setScale({2.0f, 2.0f});
    _brushContext->transformation.setRad(45  * M_PI / 180);

    _symmetryContext->isMirrorX =false;
    _symmetryContext->isMirrorY =false;
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
    StampRasterize stamp(pixel,
        {_brushContext->selectedPattern->width, _brushContext->selectedPattern->height},
        {_toolRuntimeContext.screenWidth, _toolRuntimeContext.screenHeight},
        _brushContext->transformation);

    while(stamp.hasNext()){
        Point it = stamp.next();
        Point src = stamp.getSrcPoint();
            
        if(src.x < 0 || src.y < 0 || src.x > _brushContext->selectedPattern->width || src.y > _brushContext->selectedPattern->height) continue;

        unsigned int topColor = _drawingContext->color;
        GraphicsEngine::setOpacity(topColor, (_brushContext->selectedPattern->buffer[src.y* _brushContext->selectedPattern->width + src.x] & 0xFF) / 255.0f);

        _toolRuntimeContext.drawingSession->blendMirroredPixel(it.x, it.y, topColor, _symmetryContext);
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