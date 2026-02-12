
#ifndef IADDLAYERCOMMAND_H
#define IADDLAYERCOMMAND_H

#include <vector>
#include "../../helpers/Guid/Guid.h"
#include "../../objects/frame/Frame.h"
#include "../../objects/layer/Layers.h"
#include "../../interfaces/ICommand/ICommand.h"

class AddLayerCommand : ICommand {
private:
    Frame& _frame;
    Layer _layer;
public:
    AddLayerCommand(Layer layer, Frame& _frame);
    ~AddLayerCommand();
    void execute() override;
    void undo() override;
};

#endif