#ifndef RESIZESESSION_H
#define RESIZESESSION_H
#include "../../app/AppContext/AppContext.h"
#include "../../objects/Viewport/Viewport.h"
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
#include "../../graphics/SelectionBox/SelectionBox.h"
#include "../../context/SelectContext/SelectContext.h"
#include "../../objects/ResizeHandle/ResizeHandle.h"
#include <array>

class ResizeSession {
    private:
    ToolRuntimeContext _toolRuntimeContext;
    SelectContext* _selectionContext;
    Point _startPoint;
    ENUM_MARKER _activeMarker;

    PointF* pivot = nullptr;
    PointF* dragged = nullptr;
    PointF* cornerH = nullptr;
    PointF* cornerW = nullptr;

    public:
    ResizeSession(SelectContext* selection);
    ENUM_MARKER hitTest(Point point, Viewport* viewport);
    bool begin(Point point, const ToolRuntimeContext& toolRuntimeContext);
    void update(const Point& mouse);
};
#endif