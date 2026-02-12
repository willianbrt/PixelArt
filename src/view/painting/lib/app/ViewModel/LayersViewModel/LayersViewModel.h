
#ifndef LAYERSVIEWMODEL_H
#define LAYERSVIEWMODEL_H

#include "../../../objects/frame/Frame.h"
#include "../../../objects/layer/Layers.h"

// #include "../../../commands/HistoryCommand/HistoryCommand.h"
#include "../../../commands/AddLayerCommand/AddLayerCommand.h"
#include "../../../commands/CloneLayerCommand/CloneLayerCommand.h"
#include "../../../commands/MoveLayerToCommand/MoveLayerToCommand.h"
#include "../../../commands/RemoveLayerCommand/RemoveLayerCommand.h"


class LayersViewModel{
private:

public:
    Frame& _frame;
    LayersViewModel(Frame& frame);
    ~LayersViewModel();
    
    vector<Layer*> getAllLayers();
    void registerEvent(FRAME_EVENT_TYPE eventType, std::function<void(FrameEvent)> callback);

    void onChangeActiveLayer(Guid id);
    void onAddLayer();
    void onDuplicateLayer();
    void onMoveLayerTo(Guid id, int index);
    void onMoveDownLayer();
    void onMoveUpLayer();
    void onRemoveLayer();
    void onFlipXLayer();
    void onFlipYLayer();

};

#endif