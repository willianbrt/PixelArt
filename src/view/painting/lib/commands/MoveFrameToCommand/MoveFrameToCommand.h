
#ifndef IFRAMETOBACKCOMMAND_H
#define IFRAMETOBACKCOMMAND_H

#include <vector>
#include "../../objects/Editor/Editor.h"
#include "../../interfaces/ICommand/ICommand.h"

class MoveFrameToCommand : ICommand {
private:
    Editor& _editor;
    int originalIndex, toIndex;
public:
    MoveFrameToCommand(Editor& editor, Guid frameId, int index);
    ~MoveFrameToCommand();
    void execute() override;
    void undo() override;
};

#endif