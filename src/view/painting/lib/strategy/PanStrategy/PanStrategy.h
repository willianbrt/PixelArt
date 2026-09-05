#ifndef PANSTRATEGY_H
#define PANSTRATEGY_H

#include "../../app/AppContext/AppContext.h"

#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"

#include "../../context/ToolRuntimeContext/ToolRuntimeContext.h"

#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/Pixel/Pixel.h"


class PanStrategy : public IPressedStrategy{
private:
    Point _from;
    CursorContext _cursorContext;
    
    ToolRuntimeContext _toolRuntimeContext;

    bool _initialized = false;
public:
    PanStrategy();
    ~PanStrategy();

    void onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext) override;
    void onTracking(int x, int y) override;
    void onRelease() override;
    bool isInitialized() override;
    void done() override;
    void abort() override;
    CursorContext* getCursorContext() override;

    void moveTo(Point p);
    Point getInitialPosition();
};
#endif