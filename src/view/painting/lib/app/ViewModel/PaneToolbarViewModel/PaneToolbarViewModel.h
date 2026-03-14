
#ifndef PANETOOLBARVIEWMODEL_H
#define PANETOOLBARVIEWMODEL_H

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
#include "../../../commands/LayerOpacityCommand/LayerOpacityCommand.h"


class PaneToolbarViewModel  {
private:
    
    Frame* getActiveFrame();
    Frame* _frame;
public:
    PaneToolbarViewModel();
    ~PaneToolbarViewModel();
    
    void change();
    
};

#endif