#include "LayerOpacityCommand.h"

LayerOpacityCommand::LayerOpacityCommand(Layer& layer, float oldOpacity, float newOpacity) : _layer(layer), _oldOpacity(oldOpacity), _newOpacity(newOpacity){}
LayerOpacityCommand::~LayerOpacityCommand(){}
void LayerOpacityCommand::execute(){
    // _oldOpacity = _layer.getOpacity();
    _layer.setOpacity(_newOpacity);
}
void LayerOpacityCommand::undo(){
    _layer.setOpacity(_oldOpacity);
}