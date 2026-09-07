
#ifndef BRUSHSETTINGSVM_H
#define BRUSHSETTINGSVM_H

#include "../../AppContext/AppContext.h"
#include "../../../objects/ToolManager/ToolManager.h"
#include "../../../rasterize/CircleRasterize/CircleRasterize.h"

class BrushSettingsVM  {
private:
    ToolManager* _toolManager;
    ToolSettings* _toolSettings;

public:
    BrushSettingsVM();
    ~BrushSettingsVM();

    void setSize(int size);
    void setHardness(float hardness);
    void setColor(unsigned int color);
    
    void setPattern(string shape);
};

#endif