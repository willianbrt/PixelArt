
#ifndef ZOOM_SCROLL
#define ZOOM_SCROLL

#include "../../interfaces/IScroll/IScroll.h"
#include "../../helpers/Pixel/Pixel.h"
#include "../../PixelEditor.h"

using namespace std;

class ZoomScroll : public IScroll
{
private:
    PixelEditor* _editor = nullptr;
public:
    ZoomScroll(PixelEditor* editor);
    ~ZoomScroll();
    void onScroll(float deltaX, float deltaY, Point mouseTarget);
};

#endif
