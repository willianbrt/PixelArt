
#ifndef IFRAMETOBACKCOMMAND_H
#define IFRAMETOBACKCOMMAND_H

#include <vector>
#include "../../app/Editor/Editor.h"
#include "../../interfaces/ICommand/ICommand.h"

class FrameToBackCommand : ICommand {
private:
public:
    FrameToBackCommand(Editor& editor);
    ~FrameToBackCommand();
    void execute() override;
    void undo() override;
};

#endif