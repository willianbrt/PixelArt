#include "./CopyCommand.h"

CopyCommand::CopyCommand(Editor* editor, Clipboard* clipboard, ToolManager* toolManager){
    _editor = editor;
    _clipboard = clipboard;
    _toolManager = toolManager;
}
CopyCommand::~CopyCommand(){}
void CopyCommand::execute(){
_clipboard.copy(_editor->getSelectContext());
    
}
void CopyCommand::undo(){}