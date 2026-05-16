#include "SelectContext.h"

SelectContext::SelectContext()
{

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