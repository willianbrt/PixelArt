#include "LayerRenameCommand.h"

LayerRenameCommand::LayerRenameCommand(Layer& layer, string newName) : _layer(layer), _newName(newName){}
LayerRenameCommand::~LayerRenameCommand(){}
void LayerRenameCommand::execute(){
    _oldName = _layer.getName();
    _layer.setName(_newName);
}
void LayerRenameCommand::undo(){
    _layer.setName(_oldName);
}