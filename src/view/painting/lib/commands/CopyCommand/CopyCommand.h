#ifndef COPYCOMMAND_H
#define COPYCOMMAND_H
#include "../../interfaces/ICommand/ICommand.h"

#include "../../objects/Editor/Editor.h"
#include "../../objects/ToolManager/ToolManager.h"
#include "../../objects/Clipboard/Clipboard.h"

class CopyCommand : ICommand{
private:
    
    Editor* _editor;
    ToolManager* _toolManager;
    Clipboard* _clipboard;
public:
CopyCommand(Editor* editor, Clipboard* clipboard, ToolManager* toolManager);
~CopyCommand();
void execute() override;
void undo() override;
};
#endif