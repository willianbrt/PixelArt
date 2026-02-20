#include "LayerOpacityCommand.h"

LayerOpacityCommand::LayerOpacityCommand(Layer& layer, float newOpacity) : _layer(layer), _newOpacity(newOpacity){}
LayerOpacityCommand::~LayerOpacityCommand(){}
void LayerOpacityCommand::execute(){}
void LayerOpacityCommand::undo(){}