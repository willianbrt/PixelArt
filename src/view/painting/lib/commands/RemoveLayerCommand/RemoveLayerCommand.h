
#ifndef IREMOVELAYERCOMMAND_H
#define IREMOVELAYERCOMMAND_H

#include <vector>
#include "../../interfaces/ICommand/ICommand.h"
#include "../../objects/frame/Frame.h"
#include "../../objects/layer/Layers.h"

class RemoveLayerCommand : ICommand {
private:
    Frame& _frame;
    Guid _layerId;
    size_t _index;
    std::unique_ptr<Layer> _layerRemoved;
    Guid _layerAddedID;
    bool _isAdd = false;
public:
    RemoveLayerCommand(Frame& frame, Guid layerId);
    ~RemoveLayerCommand();
    void execute() override;
    void undo() override;
};

#endif