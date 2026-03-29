#include "BrushStrategy.h"

BrushStrategy::BrushStrategy(BrushContext* brushContext, DrawingContext* context) :
_brushContext(brushContext),
_context(context)
{
    _brushContext->selectedPattern = "brush_1";
    _context->color = 0xff0000ff;
    _context->size = 1;
    _context->hardness = 1.0f;

    hoverPreview = new HoverPreview();
    _pattern = _brushContext->getPattern(_brushContext->selectedPattern);
    hoverPreview->pattern = &_pattern;

    viewport = AppContext::instance().getViewport();
}
void BrushStrategy::onPressed(int x, int y){
    hoverPreview->enable = false;

    Point to = viewport->cursorToCanvas(x, y);
    
    editor = AppContext::instance().getEditorManager()->getActiveEditor();
    
    layer = editor->getActiveFrame()->getActiveLayer();
    preview = editor->preview();
    preview->setTarget(layer);

    CanvasSettings* canvasSettings = viewport->getCanvasSettings();
    screenWidth = canvasSettings->getTilesX() * editor->getWidth();
    screenHeight = canvasSettings->getTilesY() * editor->getHeight();
    
    _pattern = _brushContext->getPattern(_brushContext->selectedPattern);
    _heightPattern = _pattern.height*_context->size;
    _widthPattern = _pattern.width*_context->size;

    stamp(to);
    
    _from = to;
}
void BrushStrategy::onTracking(int x, int y){
    Point to = viewport->cursorToCanvas(x, y);
    if (to.x == _from.x && to.y == _from.y) return;
    
    if (std::abs(to.x - _from.x) > std::abs(to.y - _from.y)) {
        drawHorizontalBrush(_from, to);
    } else {
        drawVerticalBrush(_from, to);
    }
    
    _from = to;
}
void BrushStrategy::onRelease(int x, int y){
    hoverPreview->enable = true;

    preview->commit();
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

    if(startPixel.x >= screenWidth || startPixel.y >= screenHeight) return;
    if(startPixel.x < -_widthPattern || startPixel.y < -_heightPattern) return;


    Bounding boundingStamp;
    boundingStamp.start.x = startPixel.x < 0 ? 0 : startPixel.x;
    boundingStamp.start.y = startPixel.y < 0 ? 0 : startPixel.y;
    boundingStamp.end.x = startPixel.x + _widthPattern >= screenWidth ? screenWidth : startPixel.x + _widthPattern;  
    boundingStamp.end.y = startPixel.y + _heightPattern >= screenHeight ? screenHeight : startPixel.y + _heightPattern;


    int startSrcX = startPixel.x < 0 ? -startPixel.x  / _context->size : 0;
    int startErrX = startPixel.x < 0 ? startPixel.x % _widthPattern : 0;
    
    int srcY =  startPixel.y < 0 ? -startPixel.y  / _context->size : 0;
    int errY = startPixel.y < 0 ? startPixel.y % _heightPattern : 0;
    
    for(int y = boundingStamp.start.y; y < boundingStamp.end.y; y ++){
        int srcX = startSrcX;
        int errX = startErrX;
        
        for(int x =  boundingStamp.start.x; x <  boundingStamp.end.x; x ++){
            unsigned int topColor = _context->color;
            GraphicsEngine::setOpacity(topColor, (_pattern.buffer[srcY*_pattern.height + srcX] & 0xFF) / 255.0f);  

            Point p = {
                GraphicsEngine::clampedTilePoint(x, layer->getWidth()),
                GraphicsEngine::clampedTilePoint(y, layer->getHeight())
            };

            putMirroredPixel(p.x, p.y, topColor);

            errX += _pattern.width;
            if(errX >= _widthPattern){
                srcX++;
                errX-=_widthPattern;
            }
        }

        errY += _pattern.height;
        if(errY >= _heightPattern){
            srcY++;
            errY-=_heightPattern;
        }
    }
}
void BrushStrategy::putMirroredPixel(int x, int y, unsigned int color){
    preview->putPixel(x, y,  GraphicsEngine::blendColors(preview->getPixel(x, y), color));

    int toMirrorX = GraphicsEngine::pointMirrored(x, layer->getWidth());
    int toMirrorY = GraphicsEngine::pointMirrored(y, layer->getHeight());

    if(_context->isMirrorX){
        preview->putPixel(toMirrorX, y, GraphicsEngine::blendColors(preview->getPixel(toMirrorX, y), color));
    }            
    if(_context->isMirrorY){
        preview->putPixel(x, toMirrorY, GraphicsEngine::blendColors(preview->getPixel(x, toMirrorY), color));
    }
    if(_context->isMirrorX && _context->isMirrorY){
        preview->putPixel(toMirrorX, toMirrorY, GraphicsEngine::blendColors(preview->getPixel(toMirrorX, toMirrorY), color));
    }
}

HoverPreview* BrushStrategy::getHoverPreview(){
    return hoverPreview;
}


#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<BrushStrategy>("BrushStrategy")
        .constructor<BrushContext*, DrawingContext*>()
        .function("onPressed", &BrushStrategy::onPressed)
        .function("onTracking", &BrushStrategy::onTracking)
        .function("onRelease", &BrushStrategy::onRelease)
        ;
};