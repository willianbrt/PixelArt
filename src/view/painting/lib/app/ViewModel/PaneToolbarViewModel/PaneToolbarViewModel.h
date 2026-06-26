
#ifndef PANETOOLBARVIEWMODEL_H
#define PANETOOLBARVIEWMODEL_H

#include "../../AppContext/AppContext.h"

#include "../../../objects/Editor/Editor.h"
#include "../../../objects/frame/Frame.h"
#include "../../../objects/layer/Layers.h"
#include "../../../objects/ToolManager/ToolManager.h"

#include "../../../strategy/BrushStrategy/BrushStrategy.h"
#include "../../../strategy/NonePressedEvent/NonePressedEvent.h"
#include "../../../strategy/SelectStrategy/SelectStrategy.h"
#include "../../../strategy/LineStrategy/LineStrategy.h"
#include "../../../strategy/EraserStrategy/EraserStrategy.h"
#include "../../../strategy/BucketStrategy/BucketStrategy.h"


class PaneToolbarViewModel  {
private:
    ToolManager* _toolManager;
    ToolSettings* _toolSettings;

public:
    PaneToolbarViewModel();
    ~PaneToolbarViewModel();
    
    void setPressedTool(std::string tool);
    
};

#endif