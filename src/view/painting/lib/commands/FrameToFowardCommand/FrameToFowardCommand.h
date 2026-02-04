
#ifndef IFRAMETFOWARDCOMMAND_H
#define IFRAMETFOWARDCOMMAND_H

#include <vector>
#include "../../app/Editor/Editor.h"
#include "../../interfaces/ICommand/ICommand.h"

class FrameToFowardCommand : ICommand {
private:
public:
    FrameToFowardCommand(Editor& editor);
    ~FrameToFowardCommand();
    void execute() override;
    void undo() override;
};

#endif