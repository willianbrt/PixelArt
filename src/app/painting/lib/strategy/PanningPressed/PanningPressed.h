#ifndef PANNING_EVENT
#define PANNING_EVENT

#include "../../interfaces/IPressed/IPressed.h"
#include "../../helpers/Pixel/Pixel.h"
#include "../../PixelEditor.h"

using namespace std;

class PanningPressed : public IPressed
{
private:
    Point flagPoint;
    PixelEditor* _editor = nullptr;

public:
    PanningPressed(PixelEditor* editor);
    ~PanningPressed();

    void onPressed(Point point);
    void onTracking(Point point);
    void onRelease(Point point);
};

#endif