#include "LayerVisibilityCommand.h"

LayerVisibilityCommand::LayerVisibilityCommand(Layer& layer, bool newIsVisible) : _layer(layer), _newIsVisible(newIsVisible){}
LayerVisibilityCommand::~LayerVisibilityCommand(){}
void LayerVisibilityCommand::execute(){
    _oldIsVisible = _layer.isVisible();
    _layer.setVisible(_newIsVisible);
}
void LayerVisibilityCommand::undo(){
    _layer.setVisible(_oldIsVisible);
}