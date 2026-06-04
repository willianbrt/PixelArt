#ifndef BRUSHSTRATEGY_H
#define BRUSHSTRATEGY_H
#include "../../app/AppContext/AppContext.h"
#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"
#include "../../context/ToolRuntimeContext/ToolRuntimeContext.h"
#include "../../context/CursorContext/CursorContext.h"
#include "../../context/DrawingContext/DrawingContext.h"
#include "../../context/BrushContext/BrushContext.h"
#include "../../context/SymmetryContext/SymmetryContext.h"
#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/Pixel/Pixel.h"
#include "../../objects/preview/Preview.h"
#include "../../objects/layer/Layers.h"

class BrushStrategy : public IPressedStrategy {
private:
    Point _from;
    DrawingContext* _drawingContext;
    BrushContext* _brushContext;
    CursorContext _cursorContext;
    SymmetryContext* _symmetryContext;
    
    ToolRuntimeContext _toolRuntimeContext;


    int _heightPattern;
    int _widthPattern;
    
    void drawHorizontalBrush(Point to, Point from);
    void drawVerticalBrush(Point to, Point from);
    void stamp(Point pixel);
    void putMirroredPixel(int x, int y, unsigned int color);

public:
    BrushStrategy(BrushContext* brushContext, DrawingContext* context, SymmetryContext* symmetryContext);

    void onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext) override;
    void onTracking(int x, int y) override;
    void onRelease() override;
    void done() override;
    void abort() override;
    CursorContext* getCursorContext() override;
};
#endif