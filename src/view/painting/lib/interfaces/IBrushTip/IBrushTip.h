#ifndef IBRUSHTIP_H
#define IBRUSHTIP_H

#include "../../graphics/surface/Surface.h"

class IBrushTip {
private:
    
public:
    IBrushTip() = default;
    virtual ~IBrushTip() = default;
    virtual Surface* getMask(int& size) = 0;
};

#endif