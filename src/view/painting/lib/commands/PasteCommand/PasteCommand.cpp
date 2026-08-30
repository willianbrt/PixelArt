#include "./PasteCommand.h"

PasteCommand::PasteCommand(Editor* editor, Clipboard* clipboard, ToolManager* toolManager){
    _editor = editor;
    _clipboard = clipboard;
    _toolManager = toolManager;
}
PasteCommand::~PasteCommand(){
}
void PasteCommand::execute(){
    _toolManager->setRightToolPressed(new SelectStrategy(_editor->getSelectContext()));
    _clipboard->paste(_editor);
}
void PasteCommand::undo(){}