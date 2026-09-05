#include "./ZoomStrategy.h"

ZoomStrategy::ZoomStrategy(){}

ZoomStrategy::~ZoomStrategy()
{
}
void ZoomStrategy::onScrollDown(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;

    CanvasTransform* transform = &toolRuntimeContext.canvasSettings->canvasTransform;
    zoom(transform->scale/1.1f, {x,y});
}
void ZoomStrategy::onScrollUp(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
    
    CanvasTransform* transform = &toolRuntimeContext.canvasSettings->canvasTransform;
    zoom(transform->scale*1.1f, {x,y});
}
void ZoomStrategy::zoom(float scale, Point positionCursor){
    CanvasTransform* transform = &_toolRuntimeContext.canvasSettings->canvasTransform;
    if(scale < 1) return;

    int x, y;
    int width = _toolRuntimeContext.editor->getWidth();
    int height = _toolRuntimeContext.editor->getWidth();

    if(width*scale <= _toolRuntimeContext.drawingAreaSize.x){
        x = (_toolRuntimeContext.drawingAreaSize.x - width*scale) * 0.5;
    }else{
        int currentWidth = width*transform->scale;
        int endOfAxisX = transform->pan.x + currentWidth;
        int zoomPointX = std::min(endOfAxisX, std::max(transform->pan.x, positionCursor.x));
        x = zoomPointX - (zoomPointX - transform->pan.x) * (scale / transform->scale);
    }

    if(height*scale <= _toolRuntimeContext.drawingAreaSize.y){
        y = (_toolRuntimeContext.drawingAreaSize.y - height*scale) * 0.5;
    } else {
        int currentHeight = height*transform->scale;
        int endOfAxisY = transform->pan.y + currentHeight;
        int zoomPointY = std::min(endOfAxisY, std::max(transform->pan.y, positionCursor.y));
        y = zoomPointY - (zoomPointY - transform->pan.y) * (scale / transform->scale);
    }


    transform->scale = scale;
    transform->pan = {x,y};
}