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
        (int)std::floor((x + canvasTransform.pan.x) * (canvasTransform.scale / TilingContext::N_TILE_X)) :
        (int)std::floor((x + canvasTransform.pan.x) * (canvasTransform.scale));
    p.y = (tilingContext.isTilingY) ? 
        (int)std::floor((y + canvasTransform.pan.y) * (canvasTransform.scale / TilingContext::N_TILE_Y)) :
        (int)std::floor((y + canvasTransform.pan.y) * (canvasTransform.scale));
    return p;
}
PointF CanvasSettings::canvasToWorld(float x, float y){
    PointF p;
    p.x = (tilingContext.isTilingX) ?
        (x + canvasTransform.pan.x) * (canvasTransform.scale / TilingContext::N_TILE_X) :
        (x + canvasTransform.pan.x) * (canvasTransform.scale);
    p.y = (tilingContext.isTilingY) ? 
        (y + canvasTransform.pan.y) * (canvasTransform.scale / TilingContext::N_TILE_Y) :
        (y + canvasTransform.pan.y) * (canvasTransform.scale);
    return p;
}


void CanvasSettings::zoom(float scale, Point pivot, ViewportContext* viewport, Surface* surface){
    if(scale < 1) return;

    int x, y;
    int width = surface->getWidth()*scale;
    int height = surface->getHeight()*scale;

    if(width <= viewport->width){
        x = (viewport->width - width) * 0.5;
    }else{
        int currentWidth = width*canvasTransform.scale;
        int endOfAxisX = canvasTransform.pan.x + currentWidth;
        int zoomPointX = std::min(endOfAxisX, std::max(canvasTransform.pan.x, pivot.x));
        x = zoomPointX - (zoomPointX - canvasTransform.pan.x) * (scale / canvasTransform.scale);
    }

    if(height <= viewport->height){
        y = (viewport->height - height) * 0.5;
    } else {
        int currentHeight = height*canvasTransform.scale;
        int endOfAxisY = canvasTransform.pan.y + currentHeight;
        int zoomPointY = std::min(endOfAxisY, std::max(canvasTransform.pan.y, pivot.y));
        y = zoomPointY - (zoomPointY - canvasTransform.pan.y) * (scale / canvasTransform.scale);
    }


    canvasTransform.scale = scale;
    pan({x,y}, viewport, surface);
}
void CanvasSettings::pan(Point p, ViewportContext* viewport, Surface* surface){
    Point initialPosition = getFitPosition(viewport, surface);

    int currentWidth = surface->getWidth()*canvasTransform.scale;
    int currentHeight= surface->getHeight()*canvasTransform.scale;

    int minLeftOffset, minTopOffset;
    int maxLeftOffset , maxTopOffset;

    if(currentWidth <= viewport->width){
        minLeftOffset = 0;
        maxLeftOffset = viewport->width - currentWidth;
    } else {
        maxLeftOffset = initialPosition.x;
        minLeftOffset = viewport->width - currentWidth - maxLeftOffset;
    }

    if(currentHeight <= viewport->height){
        minTopOffset = 0;
        maxTopOffset = viewport->height - currentHeight;
    } else {
        maxTopOffset = initialPosition.y;
        minTopOffset = viewport->height - currentHeight - maxTopOffset;
    }

    canvasTransform.pan.x = std::max(minLeftOffset, std::min(maxLeftOffset, p.x));
    canvasTransform.pan.y  =  std::max(minTopOffset, std::min(maxTopOffset, p.y));
}
Point CanvasSettings::getFitPosition(ViewportContext* viewport, Surface* surface){
    Point p = {0,0};
    int minScale = std::floor(getFitScale(viewport, surface));

    p.x = std::floor((viewport->width - (surface->getWidth()*minScale)) / 2);
    p.y = std::floor((viewport->height - (surface->getHeight()*minScale)) / 2);
    return p;
}
float CanvasSettings::getFitScale(ViewportContext* viewport, Surface* surface){
    return std::min(viewport->width /  surface->getWidth(), viewport->height / surface->getHeight());
}