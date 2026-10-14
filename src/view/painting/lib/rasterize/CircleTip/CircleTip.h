#ifndef CIRCLETIP_H
#define CIRCLETIP_H

#include "../../interfaces/IBrushTip/IBrushTip.h"
#include "../CircleRasterize/CircleRasterize.h"

class CircleTip : public IBrushTip {
public:
    CircleTip();
    ~CircleTip();
    Surface* getMask(int& size) override;
};
#endif