#ifndef TRANSLATESESSION_H
#define TRANSLATESESSION_H
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
#include "../../objects/RotateHandle/RotateHandle.h"
#include <array>

class TranslateSession {
    private:
    SelectContext* _selectionContext;
    Point _startPoint;

    public:
    TranslateSession(SelectContext* selection);
    bool begin(Point point, Viewport* viewport);
    void update(const Point& mouse);
};
#endif