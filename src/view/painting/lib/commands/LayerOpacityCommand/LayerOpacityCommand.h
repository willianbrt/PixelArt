
#ifndef ILAYEROPACITYCOMMAND_H
#define ILAYEROPACITYCOMMAND_H

#include <vector>
#include "../../interfaces/ICommand/ICommand.h"
#include "../../objects/frame/Frame.h"
#include "../../objects/layer/Layers.h"

class LayerOpacityCommand : ICommand {
private:
    Layer& _layer;
    float _newOpacity;
    float _oldOpacity;
public:
    LayerOpacityCommand(Layer& layer, float oldOpacity, float newOpacity);
    ~LayerOpacityCommand();
    void execute() override;
    void undo() override;
};

#endif