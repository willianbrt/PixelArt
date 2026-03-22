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
Point Viewport::cursorToCanvas(int x, int y){
    Point sketchPosition = _canvasSettings->getSketchPosition();
    Point point = Point(
        std::floor((x - sketchPosition.x) / _canvasSettings->getScale()),
        std::floor((y - sketchPosition.y) / _canvasSettings->getScale())
    );
    return point;
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