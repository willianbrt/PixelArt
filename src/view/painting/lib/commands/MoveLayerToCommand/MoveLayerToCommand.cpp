#include "MoveLayerToCommand.h"

MoveLayerToCommand::MoveLayerToCommand(Frame& frame, Guid frameId, int index) : _frame(frame){
    
}
MoveLayerToCommand::~MoveLayerToCommand(){}
void MoveLayerToCommand::execute(){
    // _frame.onMoveLayerToCommand();
}
void MoveLayerToCommand::undo(){
    // _frame.onMoveLayerToCommand();
}