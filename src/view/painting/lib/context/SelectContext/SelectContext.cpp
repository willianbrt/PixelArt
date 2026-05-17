#include "SelectContext.h"

SelectContext::SelectContext() :
handle{
        ResizeHandle(&selectionBox, ENUM_MARKER::TOP_LEFT),
        ResizeHandle(&selectionBox, ENUM_MARKER::TOP_RIGHT),
        ResizeHandle(&selectionBox, ENUM_MARKER::BOTTOM_RIGHT),
        ResizeHandle(&selectionBox, ENUM_MARKER::BOTTOM_LEFT)
    },
rotateHandle{
        RotateHandle(&selectionBox, ENUM_MARKER::TOP_LEFT),
        RotateHandle(&selectionBox, ENUM_MARKER::TOP_RIGHT),
        RotateHandle(&selectionBox, ENUM_MARKER::BOTTOM_RIGHT),
        RotateHandle(&selectionBox, ENUM_MARKER::BOTTOM_LEFT)
    }
{
    data = nullptr;
    srcArea = Bounding();
    transformation = Transformation();
    selectionBox  = SelectionBox();
}

std::array<float, 8> SelectContext::getAllHandle(Viewport* viewport){
    std::array<float, 8> vec; 
    for(int i = 0; i < 8; i+=2){
        PointF point = handle[i >> 1].getPosition(viewport);
        vec[i] = point.x;
        vec[i+1] = point.y;
    }
    return vec;
}
std::array<float, 8> SelectContext::getAllRotateHandle(Viewport* viewport){
    std::array<float, 8> vec; 
    for(int i = 0; i < 8; i+=2){
        PointF point = rotateHandle[i >> 1].getPosition(viewport);
        vec[i] = point.x;
        vec[i+1] = point.y;
    }
    return vec;
}