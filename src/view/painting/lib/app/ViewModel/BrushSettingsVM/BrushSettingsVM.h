
#ifndef BRUSHSETTINGSVM_H
#define BRUSHSETTINGSVM_H

#include <emscripten/val.h>
#include "../../AppContext/AppContext.h"
#include "../../../objects/ToolManager/ToolManager.h"
#include "../../../rasterize/StampRasterize/StampRasterize.h"
#include "../../../rasterize/CircleRasterize/CircleRasterize.h"
#include "../../../rasterize/LineRasterize/LineRasterize.h"
#include "../../../rasterize/LineTip/LineTip.h"
#include "../../../rasterize/SquareTip/SquareTip.h"
#include "../../../rasterize/CircleTip/CircleTip.h"
#include "../../../rasterize/CustomTip/CustomTip.h"
#include "../../../interfaces/IToolObserver/IToolObserver.h"
#include "../../dto/BrushDTO/BrushDTO.h"
#include "../ObserverViewModel/ObserverViewModel.h"

class BrushSettingsVM : public IToolObserver<BrushDTO>  {
private:
    ToolManager* _toolManager;
    ToolSettings* _toolSettings;
    ObserverViewModel observable;

    void notify(std::string eventType) override;

public:
    BrushSettingsVM();
    ~BrushSettingsVM();

    void setSize(int size);
    void setHardness(float hardness);
    void setColor(int r, int g, int b);
    void setStabilizer(float stabilizer);
    void setSpacing(int spacing);
    void setPixelPerfect(bool isEnable);
    void isSolidColor(bool isEnable);

    int getSize();
    float getHardness();
    unsigned int getColor();
    int getSpacing();
    float getStabilizer();
    bool getPixelPerfect();
    
    void setAlignMode(string alignMode);
    void setFillMode(string fillMode);
    
    void setPattern(uintptr_t ptr, int size, int factor);
    void setShape(uintptr_t ptr, int size, int factor);
    void setShape(string shape);
        
    void enablePattern(bool enable);
    BrushDTO getBrush();
    void registerEvent(std::string eventType, emscripten::val action) override;
};

#endif