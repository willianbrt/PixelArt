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
    

    _symmetryContext->isMirrorX =false;
    _symmetryContext->isMirrorY =false;
}
void BrushStrategy::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
    _cursorContext.enable = false;
    _initialized = true;
    _toolRuntimeContext.drawingSession->begin(_toolRuntimeContext.layer);
    
    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    
    _heightPattern = _brushContext->selectedPattern->height*_drawingContext->size;
    _widthPattern = _brushContext->selectedPattern->width*_drawingContext->size;
    
    stamp(to);
    
    _from = to;
}
void BrushStrategy::onTracking(int x, int y){
    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    if (to == _from) return;

    LineRasterize line(_from, to);
    while(line.hasNext()){
        Point p = line.next();
        stamp(p);
    }

    _from = to;
}
void BrushStrategy::onRelease(){
    done();
}

void BrushStrategy::stamp(Point pixel){
    Point startPixel = {
        pixel.x - (_widthPattern >> 1),
        pixel.y - (_heightPattern >> 1)
    };

    if(startPixel.x >= _toolRuntimeContext.screenWidth || startPixel.y >= _toolRuntimeContext.screenHeight) return;
    if(startPixel.x < -_widthPattern || startPixel.y < -_heightPattern) return;
    
    Bounding boundingStamp;
    boundingStamp.start.x = startPixel.x < 0 ? 0 : startPixel.x;
    boundingStamp.start.y = startPixel.y < 0 ? 0 : startPixel.y;
    boundingStamp.end.x = startPixel.x + _widthPattern >= _toolRuntimeContext.screenWidth ? _toolRuntimeContext.screenWidth : startPixel.x + _widthPattern;  
    boundingStamp.end.y = startPixel.y + _heightPattern >= _toolRuntimeContext.screenHeight ? _toolRuntimeContext.screenHeight : startPixel.y + _heightPattern;
    
    
    int startSrcX = startPixel.x < 0 ? -startPixel.x  / _drawingContext->size : 0;
    int startErrX = startPixel.x < 0 ? startPixel.x % _widthPattern : 0;
    
    int srcY =  startPixel.y < 0 ? -startPixel.y  / _drawingContext->size : 0;
    int errY = startPixel.y < 0 ? startPixel.y % _heightPattern : 0;
    
    for(int y = boundingStamp.start.y; y < boundingStamp.end.y; y ++){
        int srcX = startSrcX;
        int errX = startErrX;
        
        for(int x =  boundingStamp.start.x; x <  boundingStamp.end.x; x ++){
            unsigned int topColor = _drawingContext->color;
            GraphicsEngine::setOpacity(topColor, (_brushContext->selectedPattern->buffer[srcY* _brushContext->selectedPattern->width + srcX] & 0xFF) / 255.0f);

            _toolRuntimeContext.drawingSession->blendMirroredPixel(x, y, topColor, _symmetryContext);

            errX += _brushContext->selectedPattern->width;
            if(errX >= _widthPattern){
                srcX++;
                errX-=_widthPattern;
            }
        }

        errY += _brushContext->selectedPattern->height;
        if(errY >= _heightPattern){
            srcY++;
            errY-=_heightPattern;
        }
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