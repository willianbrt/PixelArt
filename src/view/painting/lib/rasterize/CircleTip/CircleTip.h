#ifndef CIRCLETIP_H
#define CIRCLETIP_H

#include "../../interfaces/IBrushTip/IBrushTip.h"
#include "../CircleRasterize/CircleRasterize.h"

class CircleTip : public IBrushTip, public IDraw {
private:
    float _rad;
    int _size;
    Transformation _transform;

    Surface* _surface;
    void plot(const int& x, const int& y) override;
public:
    CircleTip();
    ~CircleTip();
    TYPE_BRUSH_TIP getType() override;
    Surface* getMask() override;
    
    void setSize(int& size) override;
    void setRotate(float& rad) override;
    int getSize() override;
    float getRotate() override;
    Transformation getTransformation() override;
};
#endif