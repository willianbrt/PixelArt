
#ifndef ILAYERVISIBILITYCOMMAND_H
#define ILAYERVISIBILITYCOMMAND_H

#include <vector>
#include "../../interfaces/ICommand/ICommand.h"
#include "../../objects/layer/Layers.h"

class LayerVisibilityCommand : ICommand {
private:
    Layer& _layer;
    bool _newIsVisible;
    bool _oldIsVisible;
public:
    LayerVisibilityCommand(Layer& layer, bool newIsVisible);
    ~LayerVisibilityCommand();
    void execute() override;
    void undo() override;
};

#endif