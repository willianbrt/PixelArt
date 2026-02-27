#include "CloneLayerCommand.h"

CloneLayerCommand::CloneLayerCommand(Guid layerId, Frame& frame) : _layerId(layerId),_frame(frame){

}
CloneLayerCommand::~CloneLayerCommand(){}
void CloneLayerCommand::execute(){
    size_t goalIndex = _frame.getLayerIndex(_layerId);
    
    std::unique_ptr<Layer> layerCloned = make_unique<Layer>(
        std::move(_frame.getLayerByIndex(goalIndex)->clone())
    );

    _index = goalIndex+1;
    _frame.addLayer(std::move(layerCloned), _index);
}
void CloneLayerCommand::undo(){
    _frame.removeLayer(_index);
    _index = -1;
}