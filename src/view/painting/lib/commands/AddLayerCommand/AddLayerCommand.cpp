#include "AddLayerCommand.h"

AddLayerCommand::AddLayerCommand(Frame& frame, std::unique_ptr<Layer> layer, size_t index) : _layer(std::move(layer)), _frame(frame), _index(index)
{

}
AddLayerCommand::~AddLayerCommand(){}
void AddLayerCommand::execute(){

    _frame.addLayer(std::move(_layer), _index);
    // _frame.addLayer(make_unique<Layer>(_layer), _index);
}
void AddLayerCommand::undo(){
    // _frame.removeLayer(_layer.getID());
}