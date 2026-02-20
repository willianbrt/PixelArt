#include "LayerVisibilityCommand.h"

LayerVisibilityCommand::LayerVisibilityCommand(Layer& layer, bool newIsVisible) : _layer(layer), _newIsVisible(newIsVisible){}
LayerVisibilityCommand::~LayerVisibilityCommand(){}
void LayerVisibilityCommand::execute(){}
void LayerVisibilityCommand::undo(){}