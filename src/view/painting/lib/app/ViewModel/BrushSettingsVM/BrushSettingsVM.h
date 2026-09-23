
#ifndef BRUSHSETTINGSVM_H
#define BRUSHSETTINGSVM_H

#include <emscripten/val.h>
#include "../../AppContext/AppContext.h"
#include "../../../objects/ToolManager/ToolManager.h"
#include "../../../rasterize/CircleRasterize/CircleRasterize.h"
#include "../../../rasterize/LineRasterize/LineRasterize.h"

class DrawBuffer : public IDraw{
private:
    Surface* surface;
public:
    DrawBuffer(int width, int height){
        surface = new Surface(width, height);
    }
    ~DrawBuffer(){
        free(surface);
    }
    void plot(const int& x, const int& y) override{
        surface->putPixel(x,y,0xFF);
    }
    Surface* getSurface(){
        return surface;
    }
};
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
    emscripten::val getShape(string shape);
};

#endif