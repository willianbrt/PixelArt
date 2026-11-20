#ifndef LINETIP_H
#define LINETIP_H

#include "../../interfaces/IBrushTip/IBrushTip.h"
#include "../LineRasterize/LineRasterize.h"
#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"

class LineTip : public IBrushTip, public IDraw {
private:
    float _rad;
    int _size;
    Transformation _transform;

    Surface* _surface;
    void plot(const int& x, const int& y) override;
public:
    LineTip();
    ~LineTip();
    TYPE_BRUSH_TIP getType() override;
    Surface* getMask(int& size) override;
    
    void setSize(int& size) override;
    void setRotate(float& rad) override;
    int getSize() override;
    float getRotate() override;
    Transformation getTransformation() override;
};
#endif