#include "SelectionBox.h"

SelectionBox::SelectionBox(){
}

SelectionBox::SelectionBox(Bounding bounding){
    corners[ENUM_MARKER::TOP_LEFT]     = PointF(bounding.start.x, bounding.start.y);
    corners[ENUM_MARKER::TOP_RIGHT]    = PointF(bounding.end.x + 1, bounding.start.y);
    corners[ENUM_MARKER::BOTTOM_RIGHT] = PointF(bounding.end.x + 1, bounding.end.y + 1);
    corners[ENUM_MARKER::BOTTOM_LEFT]  = PointF(bounding.start.x, bounding.end.y + 1);

        // printf("teste: %f,%f, ", corners[0].x, corners[0].y);
        // printf("%f,%f, ", corners[1].x, corners[1].y);
        // printf("%f,%f, ", corners[2].x, corners[2].y);
        // printf("%f,%f\n", corners[3].x, corners[3].y);
}

SelectionBox::SelectionBox(PointF top_left, PointF bottom_left, PointF top_right, PointF bottom_right){
    corners[ENUM_MARKER::TOP_LEFT] = top_left;
    corners[ENUM_MARKER::BOTTOM_LEFT] = bottom_left;
    corners[ENUM_MARKER::TOP_RIGHT] = top_right;
    corners[ENUM_MARKER::BOTTOM_RIGHT] = bottom_right;
}

Bounding SelectionBox::getBounding(){
    float startX = std::min({corners[ENUM_MARKER::TOP_LEFT].x, corners[ENUM_MARKER::TOP_RIGHT].x, corners[ENUM_MARKER::BOTTOM_LEFT].x, corners[ENUM_MARKER::BOTTOM_RIGHT].x});
    float endX = std::max({corners[ENUM_MARKER::TOP_LEFT].x, corners[ENUM_MARKER::TOP_RIGHT].x, corners[ENUM_MARKER::BOTTOM_LEFT].x, corners[ENUM_MARKER::BOTTOM_RIGHT].x});
    float startY = std::min({corners[ENUM_MARKER::TOP_LEFT].y, corners[ENUM_MARKER::TOP_RIGHT].y, corners[ENUM_MARKER::BOTTOM_LEFT].y, corners[ENUM_MARKER::BOTTOM_RIGHT].y});
    float endY = std::max({corners[ENUM_MARKER::TOP_LEFT].y, corners[ENUM_MARKER::TOP_RIGHT].y, corners[ENUM_MARKER::BOTTOM_LEFT].y, corners[ENUM_MARKER::BOTTOM_RIGHT].y});
    
    Point start = Point((int)std::floor(startX), (int)std::floor(startY));
    Point end = Point((int)std::ceil(endX), (int)std::ceil(endY));
    return Bounding(start, end);
}

PointF SelectionBox::getCenter(){
    // return {
    //     (corners[ENUM_MARKER::TOP_LEFT].x + corners[ENUM_MARKER::TOP_RIGHT].x + corners[ENUM_MARKER::BOTTOM_LEFT].x + corners[ENUM_MARKER::BOTTOM_RIGHT].x) * 0.25f,
    //     (corners[ENUM_MARKER::TOP_LEFT].y + corners[ENUM_MARKER::TOP_RIGHT].y + corners[ENUM_MARKER::BOTTOM_LEFT].y + corners[ENUM_MARKER::BOTTOM_RIGHT].y) * 0.25f
    // };
    Bounding bounding = getBounding();
    return bounding.getCenter();
}
float SelectionBox::cross(PointF p1, PointF p2, PointF p3){
    return (p1.x - p2.x) * (p3.y - p2.y) - (p1.y - p2.y) * (p3.x - p2.x);
}
bool SelectionBox::isInsideRotatedBounding(Point point){
    PointF pointF = {(float) point.x, (float) point.y};
    bool b1 = cross(pointF, corners[ENUM_MARKER::TOP_LEFT], corners[ENUM_MARKER::TOP_RIGHT]) > 0.0f;
    bool b2 = cross(pointF, corners[ENUM_MARKER::TOP_RIGHT], corners[ENUM_MARKER::BOTTOM_RIGHT]) > 0.0f;
    bool b3 = cross(pointF, corners[ENUM_MARKER::BOTTOM_RIGHT], corners[ENUM_MARKER::BOTTOM_LEFT]) > 0.0f;
    bool b4 = cross(pointF, corners[ENUM_MARKER::BOTTOM_LEFT], corners[ENUM_MARKER::TOP_LEFT]) > 0.0f;

    return (b1 && b2 && b3 && b4) || (!b1 && !b2 && !b3 && !b4);
}