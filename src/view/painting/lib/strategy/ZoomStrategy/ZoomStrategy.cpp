#include "./ZoomStrategy.h"

ZoomStrategy::ZoomStrategy(){
    
}

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
    int height = _toolRuntimeContext.editor->getHeight();

    if(width*scale <= _toolRuntimeContext.viewport->width){
        x = (_toolRuntimeContext.viewport->width - width*scale) * 0.5;
    }else{
        int currentWidth = width*transform->scale;
        int endOfAxisX = transform->pan.x + currentWidth;
        int zoomPointX = std::min(endOfAxisX, std::max(transform->pan.x, positionCursor.x));
        x = zoomPointX - (zoomPointX - transform->pan.x) * (scale / transform->scale);
    }

    if(height*scale <= _toolRuntimeContext.viewport->height){
        y = (_toolRuntimeContext.viewport->height - height*scale) * 0.5;
    } else {
        int currentHeight = height*transform->scale;
        int endOfAxisY = transform->pan.y + currentHeight;
        int zoomPointY = std::min(endOfAxisY, std::max(transform->pan.y, positionCursor.y));
        y = zoomPointY - (zoomPointY - transform->pan.y) * (scale / transform->scale);
    }


    transform->scale = scale;
    moveTo({x,y});
}


void ZoomStrategy::moveTo(Point p){
    CanvasTransform* transform = &_toolRuntimeContext.canvasSettings->canvasTransform;
    
    int viewportWidth  = _toolRuntimeContext.viewport->width;
    int viewportHeight = _toolRuntimeContext.viewport->height;    

    Point initialPosition = getInitialPosition();

    int currentWidth = _toolRuntimeContext.editor->getWidth()*transform->scale;
    int currentHeight= _toolRuntimeContext.editor->getHeight()*transform->scale;

    int minLeftOffset, minTopOffset;
    int maxLeftOffset , maxTopOffset;

    if(currentWidth <= viewportWidth){
        minLeftOffset = 0;
        maxLeftOffset = _toolRuntimeContext.viewport->width - currentWidth;
    } else {
        maxLeftOffset = initialPosition.x;
        minLeftOffset = _toolRuntimeContext.viewport->width - currentWidth - maxLeftOffset;
    }

    if(currentHeight <= viewportHeight){
        minTopOffset = 0;
        maxTopOffset = _toolRuntimeContext.viewport->height - currentHeight;
    } else {
        maxTopOffset = initialPosition.y;
        minTopOffset = _toolRuntimeContext.viewport->height - currentHeight - maxTopOffset;
    }

    transform->pan.x = std::max(minLeftOffset, std::min(maxLeftOffset, p.x));
    transform->pan.y  =  std::max(minTopOffset, std::min(maxTopOffset, p.y));
}
Point ZoomStrategy::getInitialPosition(){
    int viewportWidth  = _toolRuntimeContext.viewport->width;
    int viewportHeight = _toolRuntimeContext.viewport->height;

    Point p = {0,0};
    int minScale = std::floor(std::min(viewportWidth /  _toolRuntimeContext.editor->getWidth(), viewportHeight / _toolRuntimeContext.editor->getHeight()));

    p.x = std::floor((viewportWidth - ( _toolRuntimeContext.editor->getWidth()*minScale)) / 2);
    p.y = std::floor((viewportHeight - (_toolRuntimeContext.editor->getHeight()*minScale)) / 2);
    return p;
}