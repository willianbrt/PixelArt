#ifndef ZOOMSTRATEGY_H
#define ZOOMSTRATEGY_H

#include "../../app/AppContext/AppContext.h"

#include "../../interfaces/IScrollStrategy/IScrollStrategy.h"

#include "../../context/ToolRuntimeContext/ToolRuntimeContext.h"

#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/Pixel/Pixel.h"


class ZoomStrategy : public IScrollStrategy{
private:    
    ToolRuntimeContext _toolRuntimeContext;
public:
    ZoomStrategy();
    ~ZoomStrategy();

    void onScrollUp(int x, int y, const ToolRuntimeContext& toolRuntimeContext) override;
    void onScrollDown(int x, int y, const ToolRuntimeContext& toolRuntimeContext) override;
    void zoom(float scale, Point positionCursor);
    void moveTo(Point p);
    Point getInitialPosition();
};
#endif