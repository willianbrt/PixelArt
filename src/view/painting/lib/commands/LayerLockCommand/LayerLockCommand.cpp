#include "LayerLockCommand.h"

LayerLockCommand::LayerLockCommand(Layer& layer, bool newIsLock) : _layer(layer), _newIsLock(newIsLock){}
LayerLockCommand::~LayerLockCommand(){}
void LayerLockCommand::execute(){}
void LayerLockCommand::undo(){}