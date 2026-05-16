
#ifndef IHANDLE_H
#define IHANDLE_H
#include "../../objects/Viewport/Viewport.h"
#include "../../graphics/SelectionBox/SelectionBox.h"


class IHandle{
    public:
        PointF getPosition(Viewport* viewport);
        bool isActive();
        bool hitTest(Point point, Viewport* viewport);

};
#endif