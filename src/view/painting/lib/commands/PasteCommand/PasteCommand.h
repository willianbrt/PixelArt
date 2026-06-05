#ifndef PASTECOMMAND_H
#define PASTECOMMAND_H

#include "../../interfaces/ICommand/ICommand.h"
#include "../../objects/Editor/Editor.h"
#include "../../objects/ToolManager/ToolManager.h"
#include "../../objects/Clipboard/Clipboard.h"


class PasteCommand : ICommand{
private:
    Editor* _editor;
    ToolManager* _toolManager;
    Clipboard* _clipboard;
    Surface* _surface;
public:
PasteCommand(Editor* editor,Surface* surface, Clipboard* clipboard, ToolManger* toolManager);
~PasteCommand();
void execute() override;
void undo() override;
};
#endif