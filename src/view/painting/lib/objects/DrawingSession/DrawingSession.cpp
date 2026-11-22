#include "DrawingSession.h"

DrawingSession::DrawingSession(Preview* preview, DirtyManager* dirtyManager, SymmetryContext* symmetryContext, CanvasSettings* canvasSettings)
{
    _preview = preview;
    _dirtyManager = dirtyManager;
    _canvasSettings = canvasSettings;
    _symmetryContext = symmetryContext;
}
void DrawingSession::begin(Layer* layer){
    Bounding dirty = _preview->getDirtyArea();
        
    dirty.start.x = std::max(dirty.start.x, 0);
    dirty.start.y = std::max(dirty.start.y, 0);
    dirty.end.x =   std::min(dirty.end.x,   layer->getWidth());
    dirty.end.y =   std::min(dirty.end.y,   layer->getHeight());

    _preview->setTarget(layer);
    _dirtyManager->markDirty(dirty);
}
void DrawingSession::commit(){
    Bounding dirty = _preview->getDirtyArea();
    _preview->commit();
    _dirtyManager->markDirty(dirty);
}
void DrawingSession::clear(){
    Bounding dirty = _preview->getDirtyArea();
    _preview->clear();
    _dirtyManager->markDirty(dirty);
}
Color DrawingSession::getModifiedPixel(int x, int y){
    return _preview->getPixel(x,y);
}
Color DrawingSession::getOriginalPixel(int x, int y){
    return _preview->getOriginalPixel(x,y);
}

void DrawingSession::putPixel(int x, int y, const Color& color, IPixelWriter* pixelWriter, ISurface* mask){
    Layer* layer = _preview->getTarget();
    if(layer->isLock()) return;


    Point p = tiling(x, y);

    pixelWriter->plot(_preview, p.x, p.y, color);

    symmetry(p.x, p.y, color, pixelWriter);

    _dirtyManager->markDirty(_preview->getDirtyArea());
}

Point DrawingSession::tiling(int& x, int& y){
    Layer* layer = _preview->getTarget();
    Point p = { x, y };
    
    if(_canvasSettings->tilingContext.isTilingX){
        p.x = GraphicsEngine::clampedTilePoint(x, layer->getWidth());
    }
    if(_canvasSettings->tilingContext.isTilingY){
        p.y = GraphicsEngine::clampedTilePoint(y, layer->getHeight());
    }

    return p;
}

void DrawingSession::symmetry(int& x, int& y, const Color& color, IPixelWriter* pixelWriter){    
    Layer* layer = _preview->getTarget();
    
    int toMirrorX = _symmetryContext->pointMirrored(x, layer->getWidth());
    int toMirrorY = _symmetryContext->pointMirrored(y, layer->getHeight());

    if(_symmetryContext->isMirrorX){
        pixelWriter->plot(_preview, toMirrorX, y, color);
    }            
    if(_symmetryContext->isMirrorY){
        pixelWriter->plot(_preview, x, toMirrorY, color);
    }
    if(_symmetryContext->isMirrorX && _symmetryContext->isMirrorY){
        pixelWriter->plot(_preview, toMirrorX, toMirrorY, color);
    }
}


Eraser::Eraser(){}
void Eraser::plot(Preview* drawing, int& x, int& y, const Color& color){
    Color drawingColor = drawing->getPixel(x, y);
    drawingColor.alpha *=  color.alpha;
    drawing->putPixel(x, y, drawingColor.hex());
};
Mask::Mask() {}
void Mask::plot(Preview* drawing, int& x, int& y, const Color& color){
    Color originalColor = drawing->getOriginalPixel(x, y);
    Color result = GraphicsEngine::blendColors(originalColor, color);
    
    if(result.alpha == 0.0f) return;
    
    drawing->putPixel(x, y, result.hex());
};
PreserveAlpha::PreserveAlpha() {}
void PreserveAlpha::plot(Preview* drawing, int& x, int& y, const Color& color){
    Color modifiedColor = drawing->getPixel(x, y);
    Color topColor = GraphicsEngine::blendColors(modifiedColor, color);
    drawing->putPixel(x, y, topColor.hex());
};
Normal::Normal() {}
void Normal::plot(Preview* drawing, int& x, int& y, const Color& color){
    Color modifiedColor = drawing->getPixel(x, y);
    Color result = GraphicsEngine::blendColors(modifiedColor, color);
    
    if(result.alpha == 0.0f) return; 
      
    drawing->putPixel(x, y, result.hex());
};

Eraser WriteMode::eraser;
Normal WriteMode::normal;
PreserveAlpha WriteMode::alpha;
Mask WriteMode::mask;