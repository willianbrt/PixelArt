
#ifndef DRAWAREA_H
#define DRAWAREA_H
#include "../../graphics/Pixel/Pixel.h"
#include "../../context/TilingContext/TilingContext.h"
#include "../../context/GridContext/GridContext.h"
#include "../../context/TilingContext/TilingContext.h"
#include "../../context/CanvasTransform/CanvasTransform.h"

class CanvasSettings{
public: 
    CanvasSettings();
    TilingContext tilingContext;
    CanvasTransform canvasTransform;
    GridContext gridContext;
        
    Point cursorToCanvas(int x, int y);
    Point canvasToWorld(int x, int y);
    PointF canvasToWorld(float x, float y);
};

#endif