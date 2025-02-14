#ifndef SCROLL_EVENT
#define SCROLL_EVENT

#include "../../helpers/Pixel/Pixel.h"

class IScroll
{
private:
public:
    virtual ~IScroll() = default;
    virtual void onScroll(float deltaX, float deltaY, Point targetMouse) = 0;
};

#endif