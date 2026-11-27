#ifndef DRAWTOOL_H
#define DRAWTOOL_H

#include "../../context/DrawingContext/DrawingContext.h"
#include "../../context/ToolRuntimeContext/ToolRuntimeContext.h"

#include "../../rasterize/StampRasterize/StampRasterize.h"

class DrawTool {
private:
    DrawingContext* _drawingContext = nullptr;
    ToolRuntimeContext* _toolRuntimeContext = nullptr;
public:
    DrawTool(DrawingContext* context, ToolRuntimeContext* _toolRuntimeContext);

    void shape(const int& x, const int& y);
    void pattern(const int& x, const int& y);
};
#endif