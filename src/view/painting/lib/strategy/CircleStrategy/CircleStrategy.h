#ifndef CIRCLESTRATEGY_H
#define CIRCLESTRATEGY_H

#include "../../app/AppContext/AppContext.h"
#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"
#include "../../context/ToolRuntimeContext/ToolRuntimeContext.h"
#include "../../context/CursorContext/CursorContext.h"
#include "../../context/DrawingContext/DrawingContext.h"
#include "../../context/LineContext/LineContext.h"
#include "../../context/CircleContext/CircleContext.h"
#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/Pixel/Pixel.h"
#include "../../objects/preview/Preview.h"
#include "../../objects/layer/Layers.h"
#include "../../objects/DrawingSession/DrawingSession.h"
#include "../../rasterize/LineRasterize/LineRasterize.h"
#include "../../rasterize/CircleRasterize/CircleRasterize.h"

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

class CircleStrategy : public IPressedStrategy {
private:
    Point _flagPoint, pivotPoint;
    DrawingContext* _drawingContext = nullptr;
    CircleContext* _circleContext = nullptr;
    CursorContext _cursorContext;
    
    ToolRuntimeContext _toolRuntimeContext;

    bool _initialized = false;
    
    void draw(const Point& pixel);
    void traceSymetricOutline(const Point& from, const Point& to, const Point& center);
    void traceFilled(const Point& from, const Point& center);
public:
    CircleStrategy(CircleContext* circleContext, DrawingContext* context);
    ~CircleStrategy();

    void onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext) override;
    void onTracking(int x, int y) override;
    void onRelease() override;
    bool isInitialized() override;
    void done() override;
    void abort() override;
    CursorContext* getCursorContext() override;
    void fillEllipseStrokeDistance(
    int cx, int cy,
    float rx, float ry,
    float thickness,
    float dashLength,   // tamanho do traço
    float gapLength     // espaço
);
};
#endif