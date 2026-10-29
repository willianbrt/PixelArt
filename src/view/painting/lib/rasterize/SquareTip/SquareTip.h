#ifndef SQUARETIP_H
#define SQUARETIP_H

#include "../../interfaces/IBrushTip/IBrushTip.h"
#include "../SquareRasterize/SquareRasterize.h"

class SquareTip : public IBrushTip, public IDraw {
private:
    Surface* surface;
    void plot(const int& x, const int& y) override;
public:
    ~SquareTip();
    SquareTip();
    TYPE_BRUSH_TIP getType() override;
    Surface* getMask(int& size) override;
};
#endif