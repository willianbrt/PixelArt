
#ifndef ICLONEFRAMECOMMAND_H
#define ICLONEFRAMECOMMAND_H

#include <vector>
#include "../../app/Editor/Editor.h"
#include "../../interfaces/ICommand/ICommand.h"

class CloneFrameCommand : ICommand {
private:
public:
    CloneFrameCommand(Editor& editor);
    ~CloneFrameCommand();
    void execute() override;
    void undo() override;
};

#endif