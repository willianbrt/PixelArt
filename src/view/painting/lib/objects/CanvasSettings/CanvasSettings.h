
#ifndef DRAWAREA_H
#define DRAWAREA_H
#include "../../graphics/Pixel/Pixel.h"

class CanvasSettings{
private: 
    Point _sketchPosition;
    float _scale = 1.0f;
    int _tilesX = 1;
    int _tilesY = 1;
    int _gridDivisionsX = 1;
    int _gridDivisionsY = 1;

public: 
    CanvasSettings();
    void setScale(float scale);
    void setTilesX(int tilesX);
    void setTilesY(int tilesY);
    void setGridDivisionsX(int gridDivisionsX);
    void setGridDivisionsY(int gridDivisionsY);
    void setSketchPosition(int x, int y);
    
    float getScale();
    Point getSketchPosition();
    int getTilesX();
    int getTilesY();
    int getGridDivisionsX();
    int getGridDivisionsY();
    
    Point cursorToCanvas(int x, int y);
    Point canvasToWorld(int x, int y);
    PointF canvasToWorld(float x, float y);
};

#endif