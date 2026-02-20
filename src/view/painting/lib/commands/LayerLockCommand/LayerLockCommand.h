
#ifndef ILAYERLOCKCOMMAND_H
#define ILAYERLOCKCOMMAND_H

#include <vector>
#include "../../interfaces/ICommand/ICommand.h"
#include "../../objects/frame/Frame.h"
#include "../../objects/layer/Layers.h"

class LayerLockCommand : ICommand {
private:
    Layer& _layer;
    bool _newIsLock;
    bool _oldIsLock;
public:
    LayerLockCommand(Layer& layer, bool newIsLock);
    ~LayerLockCommand();
    void execute() override;
    void undo() override;
};

#endif