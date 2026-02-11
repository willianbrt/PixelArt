
#ifndef IADDFRAMECOMMAND_H
#define IADDFRAMECOMMAND_H

#include <vector>
#include "../../helpers/Guid/Guid.h"
#include "../../app/Editor/Editor.h"
#include "../../objects/frame/Frame.h"
#include "../../interfaces/ICommand/ICommand.h"

class AddFrameCommand : ICommand {
private:
    Editor& _editor;
    Frame _frame;
public:
    AddFrameCommand(Frame frame, Editor& _editor);
    ~AddFrameCommand();
    void execute() override;
    void undo() override;
};

#endif