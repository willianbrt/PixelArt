#ifndef SQUARESTRATEGY_H
#define SQUARESTRATEGY_H

#include "../../app/AppContext/AppContext.h"
#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"
#include "../../context/ToolRuntimeContext/ToolRuntimeContext.h"
#include "../../context/CursorContext/CursorContext.h"
#include "../../context/DrawingContext/DrawingContext.h"
#include "../../context/LineContext/LineContext.h"
#include "../../context/SquareContext/SquareContext.h"
#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/Pixel/Pixel.h"
#include "../../objects/preview/Preview.h"
#include "../../objects/layer/Layers.h"
#include "../../objects/DrawingSession/DrawingSession.h"
#include "../../rasterize/LineRasterize/LineRasterize.h"
#include "../../rasterize/SquareRasterize/SquareRasterize.h"


class SquareStrategy : public IPressedStrategy, public IDraw {
private:
    Point _to, pivotPoint;
    DrawingContext* _drawingContext = nullptr;
    SquareContext* _squareContext = nullptr;
    CursorContext _cursorContext;
    
    ToolRuntimeContext _toolRuntimeContext;

    bool _initialized = false;
    void draw();
    void plot(const int& x, const int& y) override;
public:
    SquareStrategy(SquareContext* squareContext, DrawingContext* context);
    ~SquareStrategy();

    void onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext) override;
    void onTracking(int x, int y) override;
    void onRelease() override;
    bool isInitialized() override;
    void done() override;
    void abort() override;
    CursorContext* getCursorContext() override;
};
#endif