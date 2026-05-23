#ifndef SLECTIONCOMPOSER_H
#define SLECTIONCOMPOSER_H


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

class SelectionComposer{
public:
    SelectionComposer();
    void draw(SelectContext* selection, SymmetryContext* symmetryContext, ToolRuntimeContext& toolRuntimeContext);
    void clear(Bounding flagBounding, SelectContext* selection, SymmetryContext* symmetryContext, ToolRuntimeContext& toolRuntimeContext);
    void putMirroredPixel(int x, int y, unsigned int color, SymmetryContext* symmetryContext, ToolRuntimeContext& toolRuntimeContext);
};
#endif
