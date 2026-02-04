
#ifndef IREMOVEFRAMECOMMAND_H
#define IREMOVEFRAMECOMMAND_H

#include <vector>
#include "../../app/Editor/Editor.h"
#include "../../interfaces/ICommand/ICommand.h"

class RemoveFrameCommand : ICommand {
private:
public:
    RemoveFrameCommand(Editor& editor);
    ~RemoveFrameCommand();
    void execute() override;
    void undo() override;
};

#endif