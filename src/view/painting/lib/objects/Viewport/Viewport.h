#ifndef VIEWPORTCANVAS_H
#define VIEWPORTCANVAS_H

#include "../../graphics/Pixel/Pixel.h"
#include "../../objects/CanvasSettings/CanvasSettings.h"

struct ViewportSettings {

};
class Viewport {
private:
    int _width;
    int _height;
    double _x, _y;
    CanvasSettings* _canvasSettings;

public: 
    Viewport(int width, int height);
    void resize(int width, int height);
    void render();

    void setCursor(double x, double y);
    Point getCursor();
    Point cursorToCanvas(int x, int y);
    int getWidth();
    int getHeight();
    CanvasSettings* getCanvasSettings();
};

#endif