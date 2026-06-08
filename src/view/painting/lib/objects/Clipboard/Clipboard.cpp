#include "Clipboard.h"

    
Clipboard::Clipboard(){}
Surface* Clipboard::copy(SelectContext* select){
    Bounding destBounding = select->selectionBox.getBounding();
    if(_clipboard) delete _clipboard;
    _clipboard = new Surface(destBounding.getWidth(), destBounding.getHeight());

    // Surface* _clipboard = new Surface(destBounding.getWidth(), destBounding.getHeight());

    const PointF* scale = select->transformation.getScale();
    PointF _dstCenter = select->selectionBox.getCenter();
    float halfW = (select->srcArea.getWidth()) * 0.5f;
    float halfH = (select->srcArea.getHeight()) * 0.5f;

    for (int dy = destBounding.start.y; dy < destBounding.end.y; dy++){
        for (int dx = destBounding.start.x; dx < destBounding.end.x; dx++) {
            PointF src = select->transformation.unrotate({dx  + 0.5f - _dstCenter.x, dy + 0.5f - _dstCenter.y});
            src.x = std::floor(src.x  / scale->x + halfW);
            src.y = std::floor(src.y  / scale->y + halfH);

            if (!select->data->isInsideSkecth(src.x, src.y)) {
                continue;
            }

            unsigned int color = select->data->getPixel(src.x, src.y);

            if((color & 0xFF) == 0) { continue; }
            _clipboard->putPixel(dx-destBounding.start.x , dy-destBounding.start.y, color);
        }
    }

    return _clipboard;
}
void Clipboard::paste( Editor* editor){
    SelectContext* select = editor->getSelectContext();
    
    if(select->data){ delete select->data; }
    
    select->data = new Surface(_clipboard->getWidth(), _clipboard->getHeight());
    
    
    printf("[");
    for (size_t i = 0; i < _clipboard->getLength(); i++) {
        printf("%X, ", _clipboard->getPixel(i));
    }
    printf("]\n");
    
    Preview* preview = editor->preview();
    for (int x = 0; x < _clipboard->getWidth(); x++) {
        for (int y = 0; y < _clipboard->getHeight(); y++) {
            preview->putPixel(x, y, _clipboard->getPixel(x, y));
            select->data->putPixel(x,y,_clipboard->getPixel(x, y));
        }
    }
    
    select->srcArea = Bounding({0,0}, {_clipboard->getWidth(), _clipboard->getHeight()});
    select->selectionBox = SelectionBox(select->srcArea);
    select->transformation = Transformation();
    select->enabled = true;
}