
#ifndef DRAWAREA_H
#define DRAWAREA_H
#include "../../graphics/Pixel/Pixel.h"
#include "../../context/TilingContext/TilingContext.h"
#include "../../context/GridContext/GridContext.h"
#include "../../context/TilingContext/TilingContext.h"
#include "../../context/CanvasTransform/CanvasTransform.h"
#include "../../graphics/surface/Surface.h"
#include "../../context/ViewportContext/ViewportContext.h"

class CanvasSettings{
private:
public: 
    CanvasSettings();
    TilingContext tilingContext;
    CanvasTransform canvasTransform;
    GridContext gridContext;
        
    Point cursorToCanvas(int x, int y);
    Point canvasToWorld(int x, int y);
    PointF canvasToWorld(float x, float y);

    
    void zoom(float scale, Point pivot, ViewportContext* viewport, Surface* surface);
    void pan(Point p, ViewportContext* viewport, Surface* surface);
    Point getFitPosition(ViewportContext* viewport, Surface* surface);
    float getFitScale(ViewportContext* viewport, Surface* surface);
};

#endif