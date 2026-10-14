#ifndef LINETIP_H
#define LINETIP_H

#include "../../interfaces/IBrushTip/IBrushTip.h"
#include "../LineRasterize/LineRasterize.h"

class LineTip : public IBrushTip {
public:
    LineTip();
    ~LineTip();
    Surface* getMask(int& size) override;
};
#endif