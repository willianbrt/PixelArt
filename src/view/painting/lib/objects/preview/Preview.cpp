#include "Preview.h"

Preview::Preview(int width, int height) {
    _dirty = (bool*) malloc(width*height*sizeof(bool));
    _newColor = (unsigned int*) malloc(width*height*sizeof(unsigned int));
    _length = width*height;
}

Preview::~Preview() {
    free(_dirty);
    free(_newColor);
}
unsigned int Preview::getPixel(int x, int y) {
    return getPixel(x + _layer->getWidth()*y);
}
unsigned int Preview::getPixel(unsigned int index) {
    if(isDirty(index)) return _newColor[index];
    
    return _layer->getPixel(index);
}
void Preview::putPixel(int x, int y, unsigned int colorHex){
    if (!_layer->isInsideSkecth(x, y)) return;
    
    dirtyArea.start.x = std::min(dirtyArea.start.x, x);
    dirtyArea.start.y = std::min(dirtyArea.start.y, y);
    
    dirtyArea.end.x = std::max(dirtyArea.end.x, x+1);
    dirtyArea.end.y = std::max(dirtyArea.end.y, y+1);
    
    unsigned int index = x + y*_layer->getWidth();
    _newColor[index] = colorHex;
    _dirty[index] = true;
}
void Preview::setTarget(Layer* layer){
    clear();
    _layer = layer;
}
bool Preview::isDirty(unsigned int index){
    return _dirty[index];
}
unsigned int* Preview::getBuffer(){
    return _newColor;
}
std::vector<ModifedPixel> Preview::getModifiedPixels(){
    std::vector<ModifedPixel> modifiedPixels;
    for(int i = 0; i < _layer->getLength(); i++){
        if(!isDirty(i)) continue;
        
        ModifedPixel modifiedPixel;
        modifiedPixel.index = i;
        modifiedPixel.newColor = _newColor[i];
        modifiedPixel.oldColor = _layer->getPixel(i);
        
        modifiedPixels.push_back(modifiedPixel);
    }
    return modifiedPixels;
}
void Preview::commit(){
    for(int i = 0; i < _layer->getLength(); i++){
        if(isDirty(i)) _layer->putPixel(i, _newColor[i]);
    }
    clear();
}
void Preview::clear(){
    std::fill(_dirty, _dirty+_length, false);
    memset(_newColor, 0, _length*sizeof(unsigned int));

    dirtyArea.start = {INT_MAX, INT_MAX};
    dirtyArea.end = {INT_MIN, INT_MIN};
}
Bounding Preview::getDirtyArea(){
    if(dirtyArea.start.x == INT_MAX && dirtyArea.start.y == INT_MAX && dirtyArea.end.x == INT_MIN && dirtyArea.end.y == INT_MIN){
        return {{0,0},{0,0}};
    }

    return dirtyArea;
}
Layer* Preview::getTarget(){
    return _layer;
}
 int Preview::getWidth(){
    return _layer->getWidth();
} 
int Preview::getHeight(){
     return _layer->getHeight();

 }