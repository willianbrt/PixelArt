#include "AddFrameCommand.h"

AddFrameCommand::AddFrameCommand(Frame frame, Editor& editor) : _frame(frame), _editor(editor)
{

}
AddFrameCommand::~AddFrameCommand(){}
void AddFrameCommand::execute(){
    _editor.addFrame(&_frame);
}
void AddFrameCommand::undo(){
    _editor.removeFrame(_frame.getID());
}