
#ifndef IMOVELAYERTOCOMMAND_H
#define IMOVELAYERTOCOMMAND_H

#include <vector>
#include "../../interfaces/ICommand/ICommand.h"
#include "../../objects/frame/Frame.h"
#include "../../objects/layer/Layers.h"

class MoveLayerToCommand : ICommand {
private:
    Frame& _frame;
    int originalIndex, toIndex;
public:
    MoveLayerToCommand(Frame& frame, Guid layerId, int index);
    ~MoveLayerToCommand();
    void execute() override;
    void undo() override;
};

#endif