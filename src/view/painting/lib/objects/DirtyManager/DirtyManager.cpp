#include "./DirtyManager.h"

DirtyManager::DirtyManager(){
}
DirtyManager::~DirtyManager(){}

 void DirtyManager::markDirty(Bounding dirtyArea){
    _dirty.start.x = std::min(_dirty.start.x, dirtyArea.start.x);
    _dirty.start.y = std::min(_dirty.start.y, dirtyArea.start.y);
    _dirty.end.x =   std::max(_dirty.end.x,   dirtyArea.end.x);
    _dirty.end.y =   std::max(_dirty.end.y,   dirtyArea.end.y);

    _hasDirty = true;
 }
 Bounding DirtyManager::dirty()
 {
     return _dirty;
 }
 bool DirtyManager::hasDirty(){
    return _hasDirty;
 }
 void DirtyManager::validade(){
   _dirty = Bounding();
   _hasDirty = false;
 }