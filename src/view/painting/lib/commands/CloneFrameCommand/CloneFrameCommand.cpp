#include "CloneFrameCommand.h"

CloneFrameCommand::CloneFrameCommand(Guid frameId, Editor& editor) : _frameId(frameId),_editor(editor){

}
CloneFrameCommand::~CloneFrameCommand(){}
void CloneFrameCommand::execute(){
    // _editor.onAddFrame();
}
void CloneFrameCommand::undo(){
    // _editor.onRemoveFrame(_frameId);
}