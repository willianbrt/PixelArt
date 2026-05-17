#include "Clipboard.h"
Surface* Clipboard::copy(SelectContext* select){
    Bounding destBounding = select->selectionBox.getBounding();
    Surface* surface = new Surface(destBounding.getWidth(), destBounding.getHeight());

    const PointF* scale = select->transformation.getScale();
    PointF _dstCenter = select->selectionBox.getCenter();
    float halfW = (select->srcArea.getWidth()) * 0.5f;
    float halfH = (select->srcArea.getHeight()) * 0.5f;

    for (int dy = destBounding.start.y; dy < destBounding.end.y; dy++){
        for (int dx = destBounding.start.x; dx < destBounding.end.x; dx++) {
            PointF src = select->transformation.unrotate({dx  + 0.5f - _dstCenter.x, dy + 0.5f - _dstCenter.y});
            src.x = std::floor(src.x  / scale->x + halfH);
            src.y = std::floor(src.y  / scale->y + halfW);

            if (!select->data->isInsideSkecth(src.x, src.y)) {
                continue;
            }

            unsigned int color = select->data->getPixel(src.x, src.y);

            if((color & 0xFF) == 0) { continue; }
            surface->putPixel(dx-destBounding.start.x , dy-destBounding.start.y, color);
        }
    }

    return surface;
}
void Clipboard::paste(Surface& surface){

}