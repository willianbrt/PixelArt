#ifndef SELECTIONBOX_H
#define SELECTIONBOX_H

#include "../Pixel/Pixel.h"

enum ENUM_MARKER {
    TOP_LEFT = 0,
    TOP_RIGHT = 1,
    BOTTOM_RIGHT = 2,
    BOTTOM_LEFT = 3,
    UNKNOW = -1,
};
struct SelectionBox{
public:
    PointF corners[4] = {
        {0,0},
        {0,0},
        {0,0},
        {0,0}
    };
    Point sign[4] = {
        {-1,-1},
        {1,-1},
        {1,1},
        {-1,1}
    };

    SelectionBox();
    SelectionBox(Bounding bounding);
    SelectionBox(PointF top_left, PointF bottom_left, PointF top_right, PointF bottom_right);
    Bounding getBounding();

    PointF getCenter();
    float cross(PointF p1, PointF p2, PointF p3);
    bool isInsideRotatedBounding(Point point);
};
#endif