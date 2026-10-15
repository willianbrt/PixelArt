#ifndef LINETIP_H
#define LINETIP_H

#include "../../interfaces/IBrushTip/IBrushTip.h"
#include "../LineRasterize/LineRasterize.h"
#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"

class LineTip : public IBrushTip, public IDraw {
private:
    Surface* surface;
    void plot(const int& x, const int& y) override;
public:
    LineTip();
    ~LineTip();
    Surface* getMask(int& size) override;
    
};
#endif