#ifndef PRESSED_EVENT
#define PRESSED_EVENT

#include "../../helpers/Pixel/Pixel.h"

class IPressed
{
private:
public:
    virtual ~IPressed() = default;
    virtual void onPressed(Point point) = 0 ;
    virtual void onTracking(Point point) = 0;
    virtual void onRelease(Point point) = 0;
};

#endif