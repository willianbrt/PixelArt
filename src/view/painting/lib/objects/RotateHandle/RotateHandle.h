
#ifndef ROTATEHANDLE_H
#define ROTATEHANDLE_H
#include "../../interfaces/IHandle/IHandle.h"


class RotateHandle :IHandle{
    private:
        int size = 1;
        ENUM_MARKER _marker;
        SelectionBox* _selectionBox;
    public:
        RotateHandle(SelectionBox* selectionBox, ENUM_MARKER marker);
        PointF getPosition(Viewport* viewport);
        bool isActive();
        bool hitTest(Point point, Viewport* viewport);

};
#endif