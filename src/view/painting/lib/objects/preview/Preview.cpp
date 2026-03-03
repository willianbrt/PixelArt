#include "Preview.h"

Preview::Preview(Layer* layer) {
    _layer = layer;
    _dirty = std::vector<bool>(_layer->getLength());
    _newColor = std::vector<unsigned int>(_layer->getLength());
    
    // _dirtyArea = (bool*) malloc(_layer->getLength()*sizeof(bool));
    // if(!_dirtyArea){
    //     free(_dirtyArea);
    //     throw std::runtime_error("Impossível alocar memoria");
    // }

    // memset(_dirtyArea, false, _layer->getLength()*sizeof(bool));
}

Preview::~Preview() {
    _dirty.clear();
    _newColor.clear();
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

    putPixel(x + y*_layer->getWidth(), colorHex);
}
void Preview::putPixel(unsigned int index, unsigned int colorHex){
    if(index > _layer->getLength() || index < 0) return;

    _newColor[index] = colorHex;
    _dirty[index] = true;
}
bool Preview::isDirty(unsigned int index){
    return _dirty[index];
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
}