#include "CloneFrameCommand.h"

CloneFrameCommand::CloneFrameCommand(Guid frameId, Editor& editor) : _frameId(frameId),_editor(editor){

}
CloneFrameCommand::~CloneFrameCommand(){}
void CloneFrameCommand::execute(){
    size_t goalIndex = _editor.getFrameIndex(_frameId);
    
    std::unique_ptr<Frame> _frameCloned = make_unique<Frame>(
        std::move(_editor.getFrameByIndex(goalIndex)->clone())
    );

    _index = goalIndex+1;
    _editor.addFrame(std::move(_frameCloned), _index);
}
void CloneFrameCommand::undo(){
    _editor.removeFrame(_index);
    _index = -1;
}