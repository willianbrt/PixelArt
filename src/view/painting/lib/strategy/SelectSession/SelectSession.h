#ifndef SELECTSESSION_H
#define SELECTSESSION_H
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
#include "../../graphics/SelectionBox/SelectionBox.h"
#include "../../context/SelectContext/SelectContext.h"
#include "../../objects/RotateHandle/RotateHandle.h"
#include <array>

class SelectSession {
    private:
    ToolRuntimeContext _toolRuntimeContext;
    SelectContext* _selectionContext;
    Point _startPoint;

    public:
    SelectSession(SelectContext* selection);
    bool begin(Point point, const ToolRuntimeContext& toolRuntimeContext);
    void update(const Point& mouse);
    void end();
    void shrinkToTheDrawing();
    void initSelectData();
};
#endif