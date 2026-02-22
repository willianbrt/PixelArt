#include "AddFrameCommand.h"

AddFrameCommand::AddFrameCommand(Editor& editor, std::unique_ptr<Frame> frame, size_t index) : _frame(std::move(frame)), _index(index), _editor(editor)
{

}
AddFrameCommand::~AddFrameCommand(){}
void AddFrameCommand::execute(){
    _editor.addFrame(std::move(_frame), _index);
}
void AddFrameCommand::undo(){
    _editor.removeFrame(_index);
}