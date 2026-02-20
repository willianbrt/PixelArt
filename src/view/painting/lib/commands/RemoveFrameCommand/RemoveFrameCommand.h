
#ifndef IREMOVEFRAMECOMMAND_H
#define IREMOVEFRAMECOMMAND_H

#include <vector>
#include <algorithm>
#include "../../objects/Editor/Editor.h"
#include "../../interfaces/ICommand/ICommand.h"

class RemoveFrameCommand : ICommand {
private:
    Editor& _editor;
    Guid _frameId;
    size_t _index;
    std::unique_ptr<Frame> _frameRemoved;
    Guid _frameAddedID;
    bool _isAdd = false;
public:
    RemoveFrameCommand(Editor& editor, Guid frameId);
    ~RemoveFrameCommand();
    void execute() override;
    void undo() override;
};

#endif