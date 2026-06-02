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
    _toolRuntimeContext.cursor->enable = false;
    _toolRuntimeContext.cursor->pattern = _brushContext->selectedPattern;
    _toolRuntimeContext.cursor->scale = _drawingContext->size;
    

    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
        
    _heightPattern = _brushContext->selectedPattern->height*_drawingContext->size;
    _widthPattern = _brushContext->selectedPattern->width*_drawingContext->size;

    stamp(to);
    
    _from = to;
}
void BrushStrategy::onTracking(int x, int y){
    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    if (to.x == _from.x && to.y == _from.y) return;
    
    
    if (std::abs(to.x - _from.x) > std::abs(to.y - _from.y)) {
        drawHorizontalBrush(_from, to);
    } else {
        drawVerticalBrush(_from, to);
    }
    
    _from = to;
}
void BrushStrategy::onRelease(){
    _toolRuntimeContext.cursor->enable = true;

    done();
}

void BrushStrategy::drawHorizontalBrush(Point to, Point from){
    if(to.x < from.x){
        std::swap(to, from);
    }
    
    int dx = to.x - from.x;
    int dy = to.y - from.y;

    int dir = (dy < 0) ? -1 : 1;
    dy = std::abs(dy); 

    int D = 2*dy - dx;
    int y = from.y;
    
    for(int x = from.x; x <= to.x; x++){
        stamp({ x, y });
        
        if (D >= 0){
            y+=dir;
            D -= 2*dx;
        }
        D += 2*dy;
    }
}
void BrushStrategy::drawVerticalBrush(Point to, Point from){
    if(to.y < from.y){
        std::swap(to, from);
    }
    
    int dx = to.x - from.x;
    int dy = to.y - from.y;
    
    int dir = (dx < 0) ? -1 : 1;
    dx = std::abs(dx); 
    
    int D = 2*dx - dy;
    int x = from.x;
    
    for(int y = from.y; y <= to.y; y++){ 
        stamp({ x, y });
        
        if (D > 0){
            x+=dir;
            D -= 2*dy;
        }
        D += 2*dx;
    }
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

            Point p = {
                GraphicsEngine::clampedTilePoint(x, _toolRuntimeContext.layer->getWidth()),
                GraphicsEngine::clampedTilePoint(y, _toolRuntimeContext.layer->getHeight())
            };

            putMirroredPixel(p.x, p.y, topColor);

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
void BrushStrategy::putMirroredPixel(int x, int y, unsigned int color){
    _toolRuntimeContext.preview->putPixel(x, y,  GraphicsEngine::blendColors(_toolRuntimeContext.preview->getPixel(x, y), color));

    int toMirrorX = _symmetryContext->pointMirrored(x, _toolRuntimeContext.layer->getWidth());
    int toMirrorY = _symmetryContext->pointMirrored(y, _toolRuntimeContext.layer->getHeight());

    if(_symmetryContext->isMirrorX){
        _toolRuntimeContext.preview->putPixel(toMirrorX, y, GraphicsEngine::blendColors(_toolRuntimeContext.preview->getPixel(toMirrorX, y), color));
    }            
    if(_symmetryContext->isMirrorY){
        _toolRuntimeContext.preview->putPixel(x, toMirrorY, GraphicsEngine::blendColors(_toolRuntimeContext.preview->getPixel(x, toMirrorY), color));
    }
    if(_symmetryContext->isMirrorX && _symmetryContext->isMirrorY){
        _toolRuntimeContext.preview->putPixel(toMirrorX, toMirrorY, GraphicsEngine::blendColors(_toolRuntimeContext.preview->getPixel(toMirrorX, toMirrorY), color));
    }
}

void BrushStrategy::done() {
    _toolRuntimeContext.preview->commit();
}
void BrushStrategy::abort(){
    _toolRuntimeContext.preview->clear();
}