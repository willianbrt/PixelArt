
#ifndef IADDFRAMECOMMAND_H
#define IADDFRAMECOMMAND_H

#include <vector>
#include "../../helpers/Guid/Guid.h"
#include "../../objects/Editor/Editor.h"
#include "../../objects/frame/Frame.h"
#include "../../interfaces/ICommand/ICommand.h"

class AddFrameCommand : ICommand {
private:
    Editor& _editor;
    unique_ptr<Frame> _frame;
    size_t _index;
public:
    AddFrameCommand(Editor& _editor, std::unique_ptr<Frame> frame, size_t index);
    ~AddFrameCommand();
    void execute() override;
    void undo() override;
};

#endif