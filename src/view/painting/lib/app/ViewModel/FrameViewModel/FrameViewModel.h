
#ifndef LAYERVIEWMODEL_H
#define LAYERVIEWMODEL_H

#include "../../../objects/frame/Frame.h"
#include "../../../objects/layer/Layers.h"

// #include "../../../commands/HistoryCommand/HistoryCommand.h"
#include "../../../commands/AddLayerCommand/AddLayerCommand.h"
#include "../../../commands/CloneLayerCommand/CloneLayerCommand.h"
#include "../../../commands/MoveLayerToCommand/MoveLayerToCommand.h"
#include "../../../commands/RemoveLayerCommand/RemoveLayerCommand.h"


class FrameViewModel{
private:

public:
    Frame& _frame;
    FrameViewModel(Frame& frame);
    ~FrameViewModel();
    
    vector<Layer*> getAllLayers();
    void registerEvent(FRAME_EVENT_TYPE eventType, std::function<void(FrameEvent)> callback);

    void changeActiveLayer(Guid id);
    void addActiveLayer();
    void cloneActiveLayer();
    void moveLayerTo(Guid id, int index);
    void moveDownActiveLayer();
    void moveUpActiveLayer();
    void onRemoveLayer();

};

#endif