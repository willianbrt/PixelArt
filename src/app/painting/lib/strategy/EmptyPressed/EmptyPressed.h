#ifndef EMPTY_PRESSED_EVENT
#define EMPTY_PRESSED_EVENT

#include "../../interfaces/IPressed/IPressed.h"
#include "../../helpers/Pixel/Pixel.h"

class EmptyPressed : public IPressed
{
private:
public:
    EmptyPressed();
    ~EmptyPressed();
    void onPressed(Point point);
    void onTracking(Point point);
    void onRelease(Point point);
};

#endif