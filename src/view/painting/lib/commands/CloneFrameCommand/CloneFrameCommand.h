
#ifndef ICLONEFRAMECOMMAND_H
#define ICLONEFRAMECOMMAND_H

#include <vector>
#include "../../helpers/Guid/Guid.h"
#include "../../objects/Editor/Editor.h"
#include "../../interfaces/ICommand/ICommand.h"

class CloneFrameCommand : ICommand {
private:
    Editor& _editor;
    Guid _frameId;
    Guid _frameClonedID;
    size_t _index;

public:
    CloneFrameCommand(Editor& editor, Guid frameId);
    ~CloneFrameCommand();
    void execute() override;
    void undo() override;
};

#endif