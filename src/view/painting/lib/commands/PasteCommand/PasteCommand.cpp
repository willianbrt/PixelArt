#include "./PasteCommand.h"

PasteCommand::PasteCommand(Editor* editor, Clipboard* clipboard, ToolManager* toolManager){
    _editor = editor;
    _clipboard = clipboard;
    _toolManager = toolManager;
}
PasteCommand::~PasteCommand(){
}
void PasteCommand::execute(){
    ToolSettings* toolSettings = AppContext::instance().getToolSettings();
    _toolManager->setRightToolPressed(new SelectStrategy(_editor->getSelectContext(), &toolSettings->symmetryContext));
    _clipboard->paste(_editor);

}
void PasteCommand::undo(){}