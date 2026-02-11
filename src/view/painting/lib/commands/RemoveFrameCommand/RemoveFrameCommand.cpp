#include "RemoveFrameCommand.h"

RemoveFrameCommand::RemoveFrameCommand(Editor& editor, Guid frameId) : _editor(editor){}
RemoveFrameCommand::~RemoveFrameCommand(){}
void RemoveFrameCommand::execute(){}
void RemoveFrameCommand::undo(){}