#include "AddLayerCommand.h"

AddLayerCommand::AddLayerCommand(Frame& frame, Layer layer, size_t index) : _layer(layer), _frame(frame), _index(index)
{

}
AddLayerCommand::~AddLayerCommand(){}
void AddLayerCommand::execute(){

    _frame.addLayer(make_unique<Layer>(_layer), _index);
}
void AddLayerCommand::undo(){
    // _frame.removeLayer(_layer.getID());
}