#ifndef LINESTRATEGY_H
#define LINESTRATEGY_H
#include "../../app/AppContext/AppContext.h"
#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"
#include "../../context/ToolRuntimeContext/ToolRuntimeContext.h"
#include "../../context/CursorContext/CursorContext.h"
#include "../../context/DrawingContext/DrawingContext.h"
#include "../../context/LineContext/LineContext.h"
#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/Pixel/Pixel.h"
#include "../../objects/preview/Preview.h"
#include "../../objects/layer/Layers.h"
#include "../../objects/DrawingSession/DrawingSession.h"
#include "../../rasterize/LineRasterize/LineRasterize.h"
#include "../../rasterize/StampRasterize/StampRasterize.h"

class LineStrategy : public IPressedStrategy {
private:
    Point _from;
    DrawingContext* _drawingContext = nullptr;
    LineContext* _lineContext = nullptr;
    CursorContext _cursorContext;
    
    ToolRuntimeContext _toolRuntimeContext;

    int _heightPattern;
    int _widthPattern;
    bool _initialized = false;
    
    void drawHorizontalBrush(Point to, Point from);
    void drawVerticalBrush(Point to, Point from);
    void draw(const Point& pixel);

public:
    LineStrategy(LineContext* brushContext, DrawingContext* context);

    void onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext) override;
    void onTracking(int x, int y) override;
    void onRelease() override;
    bool isInitialized() override;
    void done() override;
    void abort() override;
    CursorContext* getCursorContext() override;
};
#endif