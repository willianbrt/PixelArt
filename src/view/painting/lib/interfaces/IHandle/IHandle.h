
#ifndef IHANDLE_H
#define IHANDLE_H
#include "../../objects/CanvasSettings/CanvasSettings.h"
#include "../../graphics/SelectionBox/SelectionBox.h"


class IHandle{
    public:
        PointF getPosition(CanvasSettings* viewport);
        bool isActive();
        bool hitTest(Point point, CanvasSettings* viewport);

};
#endif