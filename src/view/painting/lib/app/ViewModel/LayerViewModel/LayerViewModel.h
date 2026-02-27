
#ifndef LAYERVIEWMODEL_H
#define LAYERVIEWMODEL_H

#include "../../AppContext/AppContext.h"

#include "../../../objects/Editor/Editor.h"
#include "../../../objects/frame/Frame.h"
#include "../../../objects/layer/Layers.h"
#include "../../../interfaces/ILayerObserver/ILayerObserver.h"
// #include "../../../interfaces/IObservableList/IObservableList.h"
// #include "../../../commands/HistoryCommand/HistoryCommand.h"
#include "../../../commands/LayerOpacityCommand/LayerOpacityCommand.h"
#include "../../../commands/LayerVisibilityCommand/LayerVisibilityCommand.h"
#include "../../../commands/LayerLockCommand/LayerLockCommand.h"


class LayerViewModel : ILayerObserver {
private:
    Layer* _layer;

    unordered_map<LAYER_EVENT_TYPE, emscripten::val> observable;
    void onIsVisibleLayer() override;
    void onIsLockLayer() override;
    void onOpacityLayer() override;
    void onRenameLayer() override;
public:
    LayerViewModel(std::string layerID);
    ~LayerViewModel();
    
    void registerEvent(string eventType, emscripten::val callback);
    void setOpacity(float opacity);
    void setIsVisible(bool isVisible);
    void setIsLock(bool isLock);
};

#endif