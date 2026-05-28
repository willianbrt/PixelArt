
#ifndef RESIZEHANDLE_H
#define RESIZEHANDLE_H
#include "../../interfaces/IHandle/IHandle.h"


class ResizeHandle :IHandle{
    private:
        int size = 1;
        ENUM_MARKER _marker;
        SelectionBox* _selectionBox;
    public:
        ResizeHandle(SelectionBox* selectionBox, ENUM_MARKER marker);
        PointF getPosition(CanvasSettings* viewport);
        bool isActive();
        bool hitTest(Point point, CanvasSettings* viewport);

};
#endif