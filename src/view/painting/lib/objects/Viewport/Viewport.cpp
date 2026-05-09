#include "Viewport.h"

Viewport::Viewport(int width, int height){
    _width = width;
    _height = height;
    _canvasSettings = new CanvasSettings();
}

void Viewport::resize(int width, int height){
    _width = width;
    _height = height;
}

void Viewport::setCursor(double x, double y){
    _x = x;
    _y = y;
}
Point Viewport::getCursor(){
    return Point((int)_x, (int)_y);
}

Point Viewport::cursorToCanvas(int x, int y){
    Point sketchPosition = _canvasSettings->getSketchPosition();
    
    return {
        (int)std::floor((x - sketchPosition.x) / _canvasSettings->getScale()),
        (int)std::floor((y - sketchPosition.y) / _canvasSettings->getScale())
    };
}

Point Viewport::canvasToWorld(int x, int y){
    Point sketchPosition = _canvasSettings->getSketchPosition();
    return {
       (int)((x + sketchPosition.x) * _canvasSettings->getScale()),
       (int)((y + sketchPosition.y) * _canvasSettings->getScale())
    };
}
PointF Viewport::canvasToWorld(float x, float y){
    Point sketchPosition = _canvasSettings->getSketchPosition();
    return {
       (x + sketchPosition.x) * _canvasSettings->getScale(),
       (y + sketchPosition.y) * _canvasSettings->getScale()
    };
}
void Viewport::render(){
}
int Viewport::getWidth(){
    return _width;
}
int Viewport::getHeight(){
    return _height;
}
CanvasSettings* Viewport::getCanvasSettings(){
    return _canvasSettings;
}