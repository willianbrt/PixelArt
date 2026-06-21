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

    _brushContext->transformation.setScale({1.0f, 1.0f});
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
    
    _heightPattern = _brushContext->selectedPattern->height*_drawingContext->size;
    _widthPattern = _brushContext->selectedPattern->width*_drawingContext->size;

    
    const PointF* scale = _brushContext->transformation.getScale();
    float halfW = _brushContext->selectedPattern->width*0.5f;
    float halfH = _brushContext->selectedPattern->height*0.5f;


    StampRasterize stamp(to,
        {_brushContext->selectedPattern->width, _brushContext->selectedPattern->height},
        {_toolRuntimeContext.screenWidth, _toolRuntimeContext.screenHeight}, _brushContext->transformation);
    while(stamp.hasNext()){
        Point it = stamp.next();
            
        float dx = (it.x + 0.5f) - to.x ;
        float dy = (it.y + 0.5f) - to.y ;
        
        PointF src = _brushContext->transformation.unrotate({dx,  dy});
        src.x = std::floor((src.x/ scale->x) + halfW);
        src.y = std::floor((src.y/ scale->y) + halfH);


        // Point src = stamp.getSrcPoint();

        if(src.x < 0 || src.y < 0 || src.x > _brushContext->selectedPattern->width || src.y > _brushContext->selectedPattern->height) continue;
        
        printf("it: (%i, %i) src: (%f, %f)\n", it.x, it.y, src.x, src.y);

        unsigned int topColor = _drawingContext->color;

        // GraphicsEngine::setOpacity(topColor, (_brushContext->selectedPattern->buffer[src.y* _brushContext->selectedPattern->width + src.x] & 0xFF) / 255.0f);

        _toolRuntimeContext.drawingSession->blendMirroredPixel(it.x, it.y, topColor, _symmetryContext);
    }
    
    _from = to;
}
void BrushStrategy::onTracking(int x, int y){
    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    if (to == _from) return;

    LineRasterize line(_from, to);
    while(line.hasNext()){
        Point p = line.next();

        // stamp(p);
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