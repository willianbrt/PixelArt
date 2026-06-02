#ifndef VIEWPORTCANVAS_H
#define VIEWPORTCANVAS_H

#include "../../graphics/Pixel/Pixel.h"
#include "../../gpu/Pass/SketchPass/SketchPass.h"
#include "../../context/ViewportContext/ViewportContext.h"
#include "../../context/CursorContext/CursorContext.h"

struct ViewportSettings {

};
class Viewport {
private:
    int _width;
    int _height;
    double _x, _y;
    CursorContext* _cursorContext;
    
    public: 
    Viewport(int width, int height);
    void resize(int width, int height);
    void render();
    
    void setCursor(double x, double y);
    Point getCursor();
    Point cursorToCanvas(int x, int y);
    Point canvasToWorld(int x, int y);
    PointF canvasToWorld(float x, float y);
    int getWidth();
    int getHeight();
    CursorContext* getCursorContext();
};

#endif