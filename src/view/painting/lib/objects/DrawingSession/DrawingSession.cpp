#include "DrawingSession.h"

DrawingSession::DrawingSession(Preview* preview, DirtyManager* dirtyManager)
{
    _preview = preview;
    _dirtyManager = dirtyManager;
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

void DrawingSession::blendMirroredPixel(int x, int y, unsigned int color, SymmetryContext* symmetryContext){    
    Layer* layer = _preview->getTarget();

    if(symmetryContext->nTileX > 1)
        x = GraphicsEngine::clampedTilePoint(x, layer->getWidth());
    if(symmetryContext->nTileY > 1)
        y = GraphicsEngine::clampedTilePoint(y, layer->getHeight());

    _preview->putPixel(x, y,  GraphicsEngine::blendColors(_preview->getPixel(x, y), color));
    
    int toMirrorX = symmetryContext->pointMirrored(x, layer->getWidth());
    int toMirrorY = symmetryContext->pointMirrored(y, layer->getHeight());

    if(symmetryContext->isMirrorX){
        _preview->putPixel(toMirrorX, y, GraphicsEngine::blendColors(_preview->getPixel(toMirrorX, y), color));
    }            
    if(symmetryContext->isMirrorY){
        _preview->putPixel(x, toMirrorY, GraphicsEngine::blendColors(_preview->getPixel(x, toMirrorY), color));
    }
    if(symmetryContext->isMirrorX && symmetryContext->isMirrorY){
        _preview->putPixel(toMirrorX, toMirrorY, GraphicsEngine::blendColors(_preview->getPixel(toMirrorX, toMirrorY), color));
        
    }
    _dirtyManager->markDirty(_preview->getDirtyArea());
}
void DrawingSession::putMirroredPixel(int x, int y, unsigned int color, SymmetryContext* symmetryContext){
    Layer* layer = _preview->getTarget();

    if(symmetryContext->nTileX > 1)
        x = GraphicsEngine::clampedTilePoint(x, layer->getWidth());
    if(symmetryContext->nTileY > 1)
        y = GraphicsEngine::clampedTilePoint(y, layer->getHeight());


    _preview->putPixel(x, y, color);
    
    int toMirrorX = symmetryContext->pointMirrored(x, layer->getWidth());
    int toMirrorY = symmetryContext->pointMirrored(y, layer->getHeight());

    if(symmetryContext->isMirrorX){
        _preview->putPixel(toMirrorX, y, color);
    }            
    if(symmetryContext->isMirrorY){
        _preview->putPixel(x, toMirrorY, color);
    }
    if(symmetryContext->isMirrorX && symmetryContext->isMirrorY){
        _preview->putPixel(toMirrorX, toMirrorY, color);
        
    }
    _dirtyManager->markDirty(_preview->getDirtyArea());
}
void DrawingSession::blendPixel(int x, int y, unsigned int color){
    putPixel(x, y, GraphicsEngine::blendColors(_preview->getPixel(x, y), color));
}
void DrawingSession::putPixel(int x, int y, unsigned int color){
    Layer* layer = _preview->getTarget();
    
    x = GraphicsEngine::clampedTilePoint(x, layer->getWidth());
    y = GraphicsEngine::clampedTilePoint(y, layer->getHeight());

    _preview->putPixel(x, y, color);
    _dirtyManager->markDirty(_preview->getDirtyArea());
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