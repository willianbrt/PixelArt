#include "Clipboard.h"

    
Clipboard::Clipboard(){}
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
void Clipboard::paste(Surface* surface, Editor* editor){
    SelectContext* select = editor->getSelectContext();
    Surface* data = select->data;
    if(data){ delete select->data; }

    data = surface;
    
    
    printf("[");
    for (size_t i = 0; i < data->getLength(); i++) {
        printf("%X, ", data->getPixel(i));
    }
    printf("]\n");

    Preview* preview = editor->preview();
    for (size_t x = 0; x < data->getWidth(); x++) {
       for (size_t y = 0; y < data->getHeight(); y++) {
            preview->putPixel(x, y, data->getPixel(x, y));
        }
    }

    select->srcArea = Bounding({0,0}, {surface->getWidth(), surface->getHeight()});
    select->selectionBox = SelectionBox(select->srcArea);
    select->transformation = Transformation();
}