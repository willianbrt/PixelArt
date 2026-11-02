#ifndef NONETIP_H
#define NONETIP_H

#include "../../interfaces/IBrushTip/IBrushTip.h"

class NoneTip : public IBrushTip {
private:
    Surface* surface;
public:
    NoneTip();
    ~NoneTip();
    TYPE_BRUSH_TIP getType() override;
    Surface* getMask(int& size) override;
};
#endif