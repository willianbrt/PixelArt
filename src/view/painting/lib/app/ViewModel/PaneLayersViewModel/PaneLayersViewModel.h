
#ifndef PANELAYERSVIEWMODEL_H
#define PANELAYERSVIEWMODEL_H

#include "../../AppContext/AppContext.h"

#include "../../../objects/Editor/Editor.h"
#include "../../../objects/frame/Frame.h"
#include "../../../objects/layer/Layers.h"

#include "../../dto/LayerDTO/LayerDTO.h"
#include "../LayerViewModel/LayerViewModel.h"

#include "../../../interfaces/IFrameObserver/IFrameObserver.h"

// #include "../../../commands/HistoryCommand/HistoryCommand.h"
#include "../../../commands/AddLayerCommand/AddLayerCommand.h"
#include "../../../commands/CloneLayerCommand/CloneLayerCommand.h"
#include "../../../commands/MoveLayerToCommand/MoveLayerToCommand.h"
#include "../../../commands/RemoveLayerCommand/RemoveLayerCommand.h"


class PaneLayersViewModel : IFrameObserver {
private:
    unordered_map<FRAME_EVENT_TYPE, emscripten::val> observable;
    
    void onChangeActiveLayer(Guid id) override;
    void onAddLayer(Layer* layer, size_t index) override;
    void onRemoveLayer(Guid id) override;
    void onMoveLayerTo(Guid id, int index) override;

    vector<LayerViewModel> layerViewModel;
    
    Frame* getActiveFrame();

public:
    Frame* _frame;
    PaneLayersViewModel();
    ~PaneLayersViewModel();
    
    void registerEvent(string eventType, emscripten::val callback);
    
    LayerDTO getLayerByIndex(size_t index);
    size_t getNumberLayers();
    void changeActiveLayer(Guid id);
    void createLayer();
    void removeActiveLayer();
    void cloneActiveLayer();
    void moveLayerTo(Guid id, int index);
    void moveDownActiveLayer();
    void moveUpActiveLayer();
    void flipXLayer();
    void flipYLayer();
};

#endif