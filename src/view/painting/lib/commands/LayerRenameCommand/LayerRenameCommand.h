
#ifndef LAYERRENAMECOMMAND_H
#define LAYERRENAMECOMMAND_H

#include <vector>
#include "../../interfaces/ICommand/ICommand.h"
#include "../../objects/layer/Layers.h"

class LayerRenameCommand : ICommand {
private:
    Layer& _layer;
    string _newName;
    string _oldName;
public:
    LayerRenameCommand(Layer& layer, string newName);
    ~LayerRenameCommand();
    void execute() override;
    void undo() override;
};

#endif