#include "MoveFrameToCommand.h"

MoveFrameToCommand::MoveFrameToCommand(Editor& editor, Guid frameId, int index) : _editor(editor){
    
}
MoveFrameToCommand::~MoveFrameToCommand(){}
void MoveFrameToCommand::execute(){
    Frame frame;
    // _editor.onMoveFrameToCommand();
}
void MoveFrameToCommand::undo(){
    // _editor.onMoveFrameToCommand();
}