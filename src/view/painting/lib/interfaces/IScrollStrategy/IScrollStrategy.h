#ifndef ISCROLLDSTRATEGY_H
#define ISCROLLDSTRATEGY_H

#include "../../context/ToolRuntimeContext/ToolRuntimeContext.h"
#include "../../context/CursorContext/CursorContext.h"
#include "../../context/BrushContext/BrushContext.h"


class IScrollStrategy  {
protected:
public:
    virtual ~IScrollStrategy() = default;
    virtual void onScrollDown(int x, int y, const ToolRuntimeContext& toolRuntimeContext) = 0;
    virtual void onScrollUp(int x, int y, const ToolRuntimeContext& toolRuntimeContext) = 0;
};
#endif
