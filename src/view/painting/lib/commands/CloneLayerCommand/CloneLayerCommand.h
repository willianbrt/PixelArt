
#ifndef ICLONELAYERCOMMAND_H
#define ICLONELAYERCOMMAND_H

#include <vector>
#include "../../helpers/Guid/Guid.h"
#include "../../interfaces/ICommand/ICommand.h"
#include "../../objects/frame/Frame.h"
#include "../../objects/layer/Layers.h"

class CloneLayerCommand : ICommand {
private:
    Frame& _frame;
    Guid _layerId;
    Guid _layerClonedID;
    size_t _index;
public:
    CloneLayerCommand(Guid layerId, Frame& frame);
    ~CloneLayerCommand();
    void execute() override;
    void undo() override;
};

#endif