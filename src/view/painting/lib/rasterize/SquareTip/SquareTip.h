#ifndef SQUARETIP_H
#define SQUARETIP_H

#include "../../interfaces/IBrushTip/IBrushTip.h"
#include "../SquareRasterize/SquareRasterize.h"

class SquareTip : public IBrushTip {
public:
    ~SquareTip();
    SquareTip();
    Surface* getMask(int& size) override;
};
#endif