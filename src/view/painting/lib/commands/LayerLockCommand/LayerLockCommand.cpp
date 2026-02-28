#include "LayerLockCommand.h"

LayerLockCommand::LayerLockCommand(Layer& layer, bool newIsLock) : _layer(layer), _newIsLock(newIsLock){}
LayerLockCommand::~LayerLockCommand(){}
void LayerLockCommand::execute(){
    _oldIsLock = _layer.isLock();
    _layer.setLock(_newIsLock);
}
void LayerLockCommand::undo(){
    _layer.setLock(_oldIsLock);
}