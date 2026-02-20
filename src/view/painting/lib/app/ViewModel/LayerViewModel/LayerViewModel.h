
#ifndef LAYERVIEWMODEL_H
#define LAYERVIEWMODEL_H

#include "../../../objects/Editor/Editor.h"
#include "../../../objects/frame/Frame.h"
#include "../../../objects/layer/Layers.h"
// #include "../../../interfaces/IObservableList/IObservableList.h"
// #include "../../../commands/HistoryCommand/HistoryCommand.h"
#include "../../../commands/LayerOpacityCommand/LayerOpacityCommand.h"
#include "../../../commands/LayerVisibilityCommand/LayerVisibilityCommand.h"
#include "../../../commands/LayerLockCommand/LayerLockCommand.h"


class LayerViewModel{
private:

public:
    Layer& _layer;
    LayerViewModel(Layer& layer);
    ~LayerViewModel();
    
    vector<Frame*>& getAllFrames();
    // // void registerEvent(EDITOR_EVENT_TYPE eventType, std::function<void(EditorEvent)> callback);

    void setOpacity(float opacity);
    void setIsVisible(bool isVisible);
    void setIsLock(bool isLock);
};

#endif