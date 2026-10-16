
#ifndef BRUSHSETTINGSVM_H
#define BRUSHSETTINGSVM_H

#include <emscripten/val.h>
#include "../../AppContext/AppContext.h"
#include "../../../objects/ToolManager/ToolManager.h"
#include "../../../rasterize/CircleRasterize/CircleRasterize.h"
#include "../../../rasterize/LineRasterize/LineRasterize.h"
#include "../../../rasterize/LineTip/LineTip.h"
#include "../../../rasterize/SquareTip/SquareTip.h"
#include "../../../rasterize/CircleTip/CircleTip.h"
#include "../../../rasterize/CustomTip/CustomTip.h"
#include "../../dto/BrushDTO/BrushDTO.h"

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
    
    void setPattern(uintptr_t ptr, int size, int factor);
    void setShape(uintptr_t ptr, int size, int factor);
    void setShape(string shape);
    BrushDTO getBrush();
};

#endif