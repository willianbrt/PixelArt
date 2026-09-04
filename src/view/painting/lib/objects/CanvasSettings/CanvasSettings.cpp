#include "CanvasSettings.h"


CanvasSettings::CanvasSettings(){
}

Point CanvasSettings::cursorToCanvas(int x, int y){
    Point p;
    p.x = (tilingContext.isTilingX) ?
        (int)std::floor((x - canvasTransform.pan.x) / (canvasTransform.scale / TilingContext::N_TILE_X)) :
        (int)std::floor((x - canvasTransform.pan.x) / (canvasTransform.scale));
    p.y = (tilingContext.isTilingY) ? 
        (int)std::floor((y - canvasTransform.pan.y) / (canvasTransform.scale / TilingContext::N_TILE_Y)) :
        (int)std::floor((y - canvasTransform.pan.y) / (canvasTransform.scale));

    return p;
}

Point CanvasSettings::canvasToWorld(int x, int y){
    Point p;
    p.x = (tilingContext.isTilingX) ?
        (int)std::floor((x - canvasTransform.pan.x) / (canvasTransform.scale / TilingContext::N_TILE_X)) :
        (int)std::floor((x - canvasTransform.pan.x) / (canvasTransform.scale));
    p.y = (tilingContext.isTilingY) ? 
        (int)std::floor((y - canvasTransform.pan.y) / (canvasTransform.scale / TilingContext::N_TILE_Y)) :
        (int)std::floor((y - canvasTransform.pan.y) / (canvasTransform.scale));
    return p;
}
PointF CanvasSettings::canvasToWorld(float x, float y){
    PointF p;
    p.x = (tilingContext.isTilingX) ?
        (x - canvasTransform.pan.x) / (canvasTransform.scale / TilingContext::N_TILE_X) :
        (x - canvasTransform.pan.x) / (canvasTransform.scale);
    p.y = (tilingContext.isTilingY) ? 
        (y - canvasTransform.pan.y) / (canvasTransform.scale / TilingContext::N_TILE_Y) :
        (y - canvasTransform.pan.y) / (canvasTransform.scale);
    return p;
}