#include "MoveLayerToCommand.h"

MoveLayerToCommand::MoveLayerToCommand(Frame& frame, Guid layerId, int toIndex) : _frame(frame), _layerId(layerId), _toIndex(toIndex) {
    
}
MoveLayerToCommand::~MoveLayerToCommand(){}
void MoveLayerToCommand::execute(){
    _originalIndex = _frame.getLayerIndex(_layerId);
    _frame.bringLayerTo(_layerId, _toIndex);
}
void MoveLayerToCommand::undo(){
    _frame.bringLayerTo(_layerId, _originalIndex);
}