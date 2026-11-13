#include "CanvasSettings.h"


CanvasSettings::CanvasSettings(){
}

Point CanvasSettings::cursorToCanvas(int x, int y){
    Point p;
    p.x = (int)std::floor((x - canvasTransform.pan.x) / (canvasTransform.scale));
    p.y = (int)std::floor((y - canvasTransform.pan.y) / (canvasTransform.scale));
    return p;
}

Point CanvasSettings::canvasToWorld(int x, int y){
    PointF p = canvasToWorld((float) x, (float) y);
    return { (int)p.x, (int)p.y };
}
PointF CanvasSettings::canvasToWorld(float x, float y){
    PointF p;
    p.x = canvasTransform.pan.x + x * (canvasTransform.scale );
    p.y = canvasTransform.pan.y + y * (canvasTransform.scale );
    return p;
}


void CanvasSettings::zoom(float scale, Point pivot, ViewportContext* viewport, Surface* surface){
    if(scale < 1) return;

    int x, y;

    int repeatX = tilingContext.isTilingX ? TilingContext::N_TILE_X : 1;
    int repeatY = tilingContext.isTilingY ? TilingContext::N_TILE_Y : 1;

    int width = surface->getWidth()*scale*repeatX;
    int height = surface->getHeight()*scale*repeatY;

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

    int repeatX = tilingContext.isTilingX ? TilingContext::N_TILE_X : 1;
    int repeatY = tilingContext.isTilingY ? TilingContext::N_TILE_Y : 1;

    int currentWidth = std::floor(surface->getWidth()*canvasTransform.scale * repeatX);
    int currentHeight= std::floor(surface->getHeight()*canvasTransform.scale * repeatY);

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
    float minScale = getFitScale(viewport, surface);

    int repeatX = tilingContext.isTilingX ? TilingContext::N_TILE_X : 1;
    int repeatY = tilingContext.isTilingY ? TilingContext::N_TILE_Y : 1;

    p.x = std::floor((viewport->width -  std::floor((surface->getWidth()*repeatX*minScale))) / 2);
    p.y = std::floor((viewport->height - std::floor((surface->getHeight()*repeatY*minScale))) / 2);
    return p;
}
float CanvasSettings::getFitScale(ViewportContext* viewport, Surface* surface){
    int repeatX = tilingContext.isTilingX ? TilingContext::N_TILE_X : 1;
    int repeatY = tilingContext.isTilingY ? TilingContext::N_TILE_Y : 1;

    return std::min(viewport->width /  (surface->getWidth()*repeatX), viewport->height / (surface->getHeight()*repeatY));
}