#include "CanvasSettings.h"


CanvasSettings::CanvasSettings(){
}
void CanvasSettings::setTilesX(int tilesX){
    _tilesX = tilesX; 
}
void CanvasSettings::setTilesY(int tilesY){
    _tilesY = tilesY; 
}
void CanvasSettings::setScale(float scale){
    _scale = scale;
}

void CanvasSettings::setGridDivisionsX(int gridDivisionsX){
    _gridDivisionsX = gridDivisionsX;
}
void CanvasSettings::setGridDivisionsY(int gridDivisionsY){
    _gridDivisionsY = gridDivisionsY;
}
int CanvasSettings::getGridDivisionsX(){ return _gridDivisionsX; }
int CanvasSettings::getGridDivisionsY(){ return _gridDivisionsY; }
int CanvasSettings::getTilesX(){
    return _tilesX;
}
int CanvasSettings::getTilesY(){
    return _tilesY;
}
float CanvasSettings::getScale(){
    return _scale;
}
Point CanvasSettings::getSketchPosition(){
    return _sketchPosition;
}

void CanvasSettings::setSketchPosition(int x, int y){
    _sketchPosition.x = x;
    _sketchPosition.y = y;
}

