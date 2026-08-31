
#ifndef DRAWINGSETTINGSVM_H
#define DRAWINGSETTINGSVM_H

#include "../../AppContext/AppContext.h"
#include "../../../objects/ToolManager/ToolManager.h"

class DrawingSettingsVM  {
private:
    ToolManager* _toolManager;
    ToolSettings* _toolSettings;

public:
    DrawingSettingsVM();
    ~DrawingSettingsVM();

    void setSize(int size);
    void setHardness(float hardness);
    void setColor(unsigned int color);
    
};

#endif