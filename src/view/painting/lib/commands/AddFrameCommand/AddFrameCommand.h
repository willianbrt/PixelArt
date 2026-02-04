
#ifndef IADDFRAMECOMMAND_H
#define IADDFRAMECOMMAND_H

#include <vector>
#include "../../app/Editor/Editor.h"
#include "../../interfaces/ICommand/ICommand.h"

class AddFrameCommand : ICommand {
private:
public:
    AddFrameCommand(Editor& editor);
    ~AddFrameCommand();
    void execute() override;
    void undo() override;
};

#endif