#ifndef EMPTY_SCROLL_EVENT
#define EMPTY_SCROLL_EVENT

#include "../../interfaces/IScroll/IScroll.h"
#include "../../helpers/Pixel/Pixel.h"

class EmptyScroll : public IScroll
{
private:
public:
    EmptyScroll();
    ~EmptyScroll();
    void onScroll(float deltaX, float deltaY, Point targetMouse);
};
#endif