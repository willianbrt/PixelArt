#ifndef IBRUSHTIP_H
#define IBRUSHTIP_H

#include "../../graphics/surface/Surface.h"


enum TYPE_BRUSH_TIP{
    GEOMTRIC = 0,
    IMAGE = 1,
};
class IBrushTip {
private:
    
public:
    IBrushTip() = default;
    virtual ~IBrushTip() = default;
    virtual TYPE_BRUSH_TIP getType() = 0;
    virtual Surface* getMask(int& size) = 0;
};

#endif