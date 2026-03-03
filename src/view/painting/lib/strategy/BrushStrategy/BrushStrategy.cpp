#include "BrushStrategy.h"

BrushStrategy::BrushStrategy(BrushContext& brushContext, DrawingContext& context) :
_brushContext(brushContext),
_context(context)
{
}
void BrushStrategy::onPressed(int x, int y){
    Point to(x, y);
    
    layer = AppContext::instance().getEditorManager()->getActiveEditor()->getActiveFrame()->getActiveLayer();
    preview = new Preview(layer);

    screenWidth = _context.nTileX * layer->getWidth();
    screenHeight = _context.nTileY * layer->getHeight();

    from = to;
}
void BrushStrategy::onTracking(int x, int y){
    Point to(x, y);
    if (to.x == from.x && to.y == from.y) return;

    if (std::abs(to.x - from.x) > std::abs(to.y - from.y)) {
        drawHorizontalBrush(from, to);
    } else{
        drawVerticalBrush(from, to);
    }

    from = to;
}
void BrushStrategy::onRelease(int x, int y){

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
        stampPixel(Point(x, y));
        
        if (D >= 0){
            y+=dir;
            D -= 2*dx;
        }
        D += 2*dy;
    }
}
void BrushStrategy::drawVerticalBrush(Point to, Point fromfrom){
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
        stampPixel(Point(x, y));

        if (D > 0){
            x+=dir;
            D -= 2*dy;
        }
        D += 2*dx;
    }
}

void BrushStrategy::stampPixel(Point pixel){
     std::vector<std::vector<float>> pattern = _brushContext.pattern[_brushContext.selectedPattern];
    int heightPattern = (int)pattern.size()*_context.size;
    int widthPattern = (int)pattern[0].size()*_context.size;

    Point startPixel(pixel.x - (widthPattern >> 1), pixel.y - (heightPattern >> 1));
    
    if(startPixel.x >= screenWidth || startPixel.y >= screenHeight) return;
    if(startPixel.x < -widthPattern || startPixel.y < -heightPattern) return;

    Bounding boundingStamp;
    boundingStamp.start.x = startPixel.x < 0 ? 0 : startPixel.x;
    boundingStamp.start.y = startPixel.y < 0 ? 0 : startPixel.y;
    
    boundingStamp.end.x = startPixel.x + widthPattern >= screenWidth ? screenWidth : startPixel.x + widthPattern;  
    boundingStamp.end.y = startPixel.y + heightPattern >= screenHeight ? screenHeight : startPixel.y + heightPattern;

    int startSrcX = startPixel.x < 0 ? -startPixel.x  / _context.size : 0;
    int startErrX = startPixel.x < 0 ? startPixel.x % widthPattern : 0;
    
    int srcY =  startPixel.y < 0 ? -startPixel.y  / _context.size : 0;
    int errY = startPixel.y < 0 ? startPixel.y % heightPattern : 0;
    
    for(int y = boundingStamp.start.y; y < boundingStamp.end.y; y ++){
        int srcX = startSrcX;
        int errX = startErrX;
        
        for(int x =  boundingStamp.start.x; x <  boundingStamp.end.x; x ++){
            float alphaSrc = pattern[srcY][srcX];
            unsigned int topColor = static_cast<int>(alphaSrc * (_context.color >> 24 & 0xFF)) << 24 | (_context.color & 0x00FFFFFF);

            Point p;
            p.x = GraphicsEngine::clampedTilePoint(x, layer->getWidth());
            p.y = GraphicsEngine::clampedTilePoint(y, layer->getHeight());
            putPixel(p.x, p.y, topColor);

            errX +=(int) pattern[0].size();
            if(errX >= widthPattern){
                srcX++;
                errX-=widthPattern;
            }
        }

        errY+= (int)pattern.size();
        if(errY >= heightPattern){
            srcY++;
            errY-=heightPattern;
        }
    }
}
void BrushStrategy::putPixel(int x, int y, unsigned int color){
    preview->putPixel(x, y,  GraphicsEngine::blendColors(preview->getPixel(x, y), color));
    
    int toMirrorX = GraphicsEngine::pointMirrored(x, layer->getWidth());
    int toMirrorY = GraphicsEngine::pointMirrored(y, layer->getHeight());

    if(_context.isMirrorX){
        preview->putPixel(toMirrorX, y, GraphicsEngine::blendColors(preview->getPixel(toMirrorX, y), color));
    }            
    if(_context.isMirrorY){
        preview->putPixel(x, toMirrorY, GraphicsEngine::blendColors(preview->getPixel(x, toMirrorY), color));
    }
    if(_context.isMirrorX && _context.isMirrorY){
        preview->putPixel(toMirrorX, toMirrorY, GraphicsEngine::blendColors(preview->getPixel(toMirrorX, toMirrorY), color));
    }
}
