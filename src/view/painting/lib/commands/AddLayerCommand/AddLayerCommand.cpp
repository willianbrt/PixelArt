#include "AddLayerCommand.h"

AddLayerCommand::AddLayerCommand(Layer layer, Frame& frame) : _layer(layer), _frame(frame)
{

}
AddLayerCommand::~AddLayerCommand(){}
void AddLayerCommand::execute(){
    _frame.addLayer(&_layer);
}
void AddLayerCommand::undo(){
    _frame.removeLayer(_layer.getID());
}