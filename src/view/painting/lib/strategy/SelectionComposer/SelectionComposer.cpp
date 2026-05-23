
#include "./SelectionComposer.h"
SelectionComposer::SelectionComposer(){

}
void SelectionComposer::draw(SelectContext* selection, SymmetryContext* symmetryContext, ToolRuntimeContext& toolRuntimeContext){
    Bounding destBounding = selection->selectionBox.getBounding();
    toolRuntimeContext.clampBounding(destBounding);

    const PointF* scale = selection->transformation.getScale();
    PointF _dstCenter = selection->selectionBox.getCenter();
    PointF _srcCenter = selection->srcArea.getCenter();
    float halfW = (selection->srcArea.getWidth()) * 0.5f;
    float halfH = (selection->srcArea.getHeight()) * 0.5f;
        
    for (int dy = destBounding.start.y; dy < destBounding.end.y; dy++){
        for (int dx = destBounding.start.x; dx < destBounding.end.x; dx++) {
            PointF src = selection->transformation.unrotate({dx  + 0.5f - _dstCenter.x, dy + 0.5f - _dstCenter.y});
            src.x = std::floor(src.x / scale->x + halfW);
            src.y = std::floor(src.y / scale->y + halfH);


            if (!selection->data->isInsideSkecth(src.x, src.y)) {
                continue;
            }
            unsigned int color = selection->data->getPixel(src.x, src.y);
            
            if((color >> 24 & 0xFF) == 0) { continue; }
            
            Point clampedPoint = {
                GraphicsEngine::clampedTilePoint(dx, toolRuntimeContext.layer->getWidth()),
                GraphicsEngine::clampedTilePoint(dy, toolRuntimeContext.layer->getHeight())
            };
            putMirroredPixel(clampedPoint.x, clampedPoint.y, color, symmetryContext, toolRuntimeContext);
        }
    }
}
void SelectionComposer::clear(Bounding flagBounding, SelectContext* selection, SymmetryContext* symmetryContext, ToolRuntimeContext& toolRuntimeContext){
    toolRuntimeContext.clampBounding(flagBounding);
    for (int y = flagBounding.start.y; y < flagBounding.end.y; ++y) {
        Point p;
        p.y = GraphicsEngine::clampedTilePoint(y, toolRuntimeContext.layer->getHeight());
        for (int x = flagBounding.start.x; x < flagBounding.end.x; ++x) {
            p.x = GraphicsEngine::clampedTilePoint(x, toolRuntimeContext.layer->getWidth());

            if((p.x >= selection->srcArea.start.x && p.x < selection->srcArea.end.x) &&
                (p.y >= selection->srcArea.start.y && p.y < selection->srcArea.end.y)){
                toolRuntimeContext.preview->putPixel(p.x, p.y, 0x0);
                continue;
            }
            toolRuntimeContext.preview->uncommit(p.x, p.y);
        }
    }
}


void SelectionComposer::putMirroredPixel(int x, int y, unsigned int color, SymmetryContext* symmetryContext, ToolRuntimeContext& toolRuntimeContext){
    toolRuntimeContext.preview->putPixel(x, y,  GraphicsEngine::blendColors(toolRuntimeContext.preview->getPixel(x, y), color));

    int toMirrorX = symmetryContext->pointMirrored(x, toolRuntimeContext.layer->getWidth());
    int toMirrorY = symmetryContext->pointMirrored(y, toolRuntimeContext.layer->getHeight());

    if(symmetryContext->isMirrorX){
        toolRuntimeContext.preview->putPixel(toMirrorX, y, GraphicsEngine::blendColors(toolRuntimeContext.preview->getPixel(toMirrorX, y), color));
    }
    if(symmetryContext->isMirrorY){
        toolRuntimeContext.preview->putPixel(x, toMirrorY, GraphicsEngine::blendColors(toolRuntimeContext.preview->getPixel(x, toMirrorY), color));
    }
    if(symmetryContext->isMirrorX && symmetryContext->isMirrorY){
        toolRuntimeContext.preview->putPixel(toMirrorX, toMirrorY, GraphicsEngine::blendColors(toolRuntimeContext.preview->getPixel(toMirrorX, toMirrorY), color));
    }
}
