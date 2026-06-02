#include "Viewport.h"

Viewport::Viewport(int width, int height){
    _width = width;
    _height = height;
    _cursorContext = new CursorContext();
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
void Viewport::render(){
    
}
int Viewport::getWidth(){
    return _width;
}
int Viewport::getHeight(){
    return _height;
}
CursorContext* Viewport::getCursorContext(){
    return _cursorContext;
}