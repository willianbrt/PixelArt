#include "./ZoomStrategy.h"

ZoomStrategy::ZoomStrategy(){
    
}

ZoomStrategy::~ZoomStrategy()
{
}
void ZoomStrategy::onScrollDown(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;

    CanvasTransform* transform = &_toolRuntimeContext.canvasSettings->canvasTransform;
    _toolRuntimeContext.canvasSettings->zoom(transform->scale/1.1f, {x,y},
        _toolRuntimeContext.viewport, _toolRuntimeContext.editor->getSurface());
}
void ZoomStrategy::onScrollUp(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
        
    CanvasTransform* transform = &_toolRuntimeContext.canvasSettings->canvasTransform;
    _toolRuntimeContext.canvasSettings->zoom(transform->scale*1.1f, {x,y},
        _toolRuntimeContext.viewport, _toolRuntimeContext.editor->getSurface());
}