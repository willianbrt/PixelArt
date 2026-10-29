#ifndef CIRCLETIP_H
#define CIRCLETIP_H

#include "../../interfaces/IBrushTip/IBrushTip.h"
#include "../CircleRasterize/CircleRasterize.h"

class CircleTip : public IBrushTip, public IDraw {
private:
    Surface* surface;
    void plot(const int& x, const int& y) override;
public:
    CircleTip();
    ~CircleTip();
    TYPE_BRUSH_TIP getType() override;
    Surface* getMask(int& size) override;
};
#endif