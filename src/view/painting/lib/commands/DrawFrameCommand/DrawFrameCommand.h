
#ifndef IREMOVEFRAMECOMMAND_H
#define IREMOVEFRAMECOMMAND_H

#include <vector>
#include "../../objects/Editor/Editor.h"
#include "../../interfaces/ICommand/ICommand.h"
#include "../../interfaces/IGraphic/IGraphic.h"

class DrawFrameCommand : ICommand {
private:
    Editor& _editor;
    IGraphic& _graphic;
public:
    DrawFrameCommand(Editor& editor, Guid frameId, IGraphic& graphic);
    ~DrawFrameCommand();
    void execute() override;
    void undo() override;
};

#endif