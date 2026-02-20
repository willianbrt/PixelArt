#include "MoveFrameToCommand.h"

MoveFrameToCommand::MoveFrameToCommand(Editor& editor, Guid frameId, int toIndex) : _editor(editor), _frameId(frameId), _toIndex(toIndex) {
    
}
MoveFrameToCommand::~MoveFrameToCommand(){}
void MoveFrameToCommand::execute(){
    _originalIndex = _editor.getFrameIndex(_frameId);
    _editor.bringFrameTo(_frameId, _toIndex);
}
void MoveFrameToCommand::undo(){
    _editor.bringFrameTo(_frameId, _originalIndex);
}