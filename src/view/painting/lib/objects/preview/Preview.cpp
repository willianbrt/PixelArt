#include "Preview.h"

Preview::Preview(Layer* layer) {
    _layer = layer;
    
    _dirtyArea = (bool*) malloc(_layer->getLength()*sizeof(bool));
    if(!_dirtyArea){
        free(_dirtyArea);
        throw std::runtime_error("Impossível alocar memoria");
    }

    memset(_dirtyArea, false, _layer->getLength()*sizeof(bool));
}

Preview::~Preview() {
    free(_dirtyArea);
    free(_layer);
    _updatedPixels.clear();
}
Layer* Preview::getPtrLayer() const {
    return _layer;
}
// unsigned int Preview::getFilteredPixel(unsigned int index) {
//     return _layer->getFilteredPixel(index);
// }

unsigned int Preview::getPixel(int x, int y) {
    return _layer->getPixel(x, y);
}
unsigned int Preview::getPixel(unsigned int index) {
    return _layer->getPixel(index);
}
void Preview::putPixel(int x, int y, unsigned int colorHex){    
    if (!_layer->isInsideSkecth(x, y)) return;

    putPixel(x + y*_layer->getWidth(), colorHex);
}
void Preview::putPixel(unsigned int index, unsigned int colorHex){
    if(index > _layer->getLength() || index < 0) return;
    if(_dirtyArea[index] == 0) return;
    
    _dirtyArea[index] = true;

    ModifedPixel modifiedPixel;
    modifiedPixel.index = index;
    modifiedPixel.newColor= colorHex;
    modifiedPixel.oldColor = _layer->getPixel(index);
    _updatedPixels.push_back(modifiedPixel);
}
bool Preview::isDirty(unsigned int index){
    return _dirtyArea[index];
}
std::vector<ModifedPixel> Preview::getModifiedPixels(){
    return _updatedPixels;
}
void Preview::commit(){
    for(ModifedPixel updatedPixel : _updatedPixels){
        _layer->putPixel(updatedPixel.index, updatedPixel.newColor);
    }
}
void Preview::uncommit(){
    for(ModifedPixel updatedPixel : _updatedPixels){
        _layer->putPixel(updatedPixel.index, updatedPixel.oldColor);
    }
}