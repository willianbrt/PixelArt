#include "CloneLayerCommand.h"

CloneLayerCommand::CloneLayerCommand(Guid layerId, Frame& frame) : _layerId(layerId),_frame(frame){

}
CloneLayerCommand::~CloneLayerCommand(){}
void CloneLayerCommand::execute(){
    // _frame.addActiveLayer();
}
void CloneLayerCommand::undo(){
    // _frame.onRemoveLayer(_layerId);
}