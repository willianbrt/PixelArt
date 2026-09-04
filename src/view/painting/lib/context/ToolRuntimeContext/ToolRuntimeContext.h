#ifndef TOOLRUNTIMECONTEXT_H
#define TOOLRUNTIMECONTEXT_H

#include "../../interfaces/IToolContext/IToolContext.h"
#include "../../objects/Editor/Editor.h"
#include "../../objects/layer/Layers.h"
#include "../../objects/preview/Preview.h"
#include "../../objects/CanvasSettings/CanvasSettings.h"
#include "../../context/ViewportContext/ViewportContext.h"
#include "../../context/CursorContext/CursorContext.h"

class ToolRuntimeContext : public IToolContext{
public:
    Editor* editor;
    Layer* layer;
    Preview* preview;
    DrawingSession* drawingSession;
    ViewportContext* viewport;
    CanvasSettings* canvasSettings;
    Point drawingAreaSize;
    void clampBounding(Bounding& bounding);
};

#endif
