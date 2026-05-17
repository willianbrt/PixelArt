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
#include "../../strategy/TranslateSession/TranslateSession.h"
#include "../../strategy/SelectSession/SelectSession.h"

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
    TranslateSession* _translateSession;
    SelectSession* _selectSession;

    CursorContext* cursorContext;

    ENUM_MODE _mode;


    void putMirroredPixel(int x, int y, unsigned int color);
    void draw();

public:
    SelectStrategy(SelectContext* selectContext, SymmetryContext* context);
    ~SelectStrategy();

    void onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext) override;
    void onTracking(int x, int y) override;
    void onRelease() override;

    CursorContext* getCursorContext() override;
    void done() override;
    void abort() override;
    Bounding flagBounding;
    
    bool _cutting;
    
    SelectionBox _flagSelectionBox;
};
#endif