#include "./PasteCommand.h"

PasteCommand::PasteCommand(Editor* editor, Surface* surface, Clipboard* clipboard, ToolManager* toolManager){
    _editor = editor;
    _clipboard = clipboard;
    _toolManager = toolManager;
}
PasteCommand::~PasteCommand(){}
void PasteCommand::execute(){
    // _clipboard.paste(surface, _activeEditor);
    // // ToolSetttings* toolSettings = AppContext::instance().getToolSettings());
    // // toolManager->setRightToolPressed(new SelectStrategy(_activeEditor->getSelectContext(), &toolSettings->symmetryContext));

}
void PasteCommand::undo(){}