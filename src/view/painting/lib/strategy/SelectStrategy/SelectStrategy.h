#ifndef SELECTSTRATEGY_H
#define SELECTSTRATEGY_H
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
#include "../../graphics/SelectionBox/SelectionBox.h"
#include "../../objects/preview/Preview.h"
#include "../../objects/layer/Layers.h"
#include "../../strategy/ResizeSession/ResizeSession.h"
#include "../../strategy/RotateSession/RotateSession.h"

#include <optional>

class SelectStrategy : public IPressedStrategy {
private:
    enum ENUM_MODE {
        SELECT = 0,
        SELECTED = 1,
        TRANSLATE = 2,
        RESIZE = 3,
        ROTATE = 4,
    };

    Point _pressed, _from;
    SymmetryContext* _symmetryContext;
    SelectContext* _selectContext;
    ToolRuntimeContext _toolRuntimeContext;
    ResizeSession* _resizeSession;
    RotateSession* _rotateSession;

    CursorContext* cursorContext;

    int sizeHitbox = 1;
    ENUM_MODE _mode;
    ENUM_MARKER _activeMarker;


    void putMirroredPixel(int x, int y, unsigned int color);
    void draw();

    void translate(float deltaX, float deltaY);
    Surface* copy();
    void paste(Surface& surface);

    void start();

public:
    SelectStrategy(SelectContext* selectContext, SymmetryContext* context);
    ~SelectStrategy();

    void onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext) override;
    void onTracking(int x, int y) override;
    void onRelease() override;

    CursorContext* getCursorContext() override;
    void done() override;
    void abort() override;

    
    PointF _dstCenter, _srcCenter;
    PointF _delta;
    PointF _scale = {1.0f, 1.0f};
    PointF _resized = {1.0f, 1.0f};
    PointF _origCenter;
    bool _cutting;

    std::optional<Bounding> _originalBounding;
    SelectionBox _originalSelectionBox;
    
    SelectionBox _flagSelectionBox;
    Bounding flagBounding;
};
#endif