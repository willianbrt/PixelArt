#ifndef NONETIP_H
#define NONETIP_H

#include "../../interfaces/IBrushTip/IBrushTip.h"

class NoneTip : public IBrushTip {
private:
    float _rad;
    int _size;
    Transformation _transform;

    Surface* _surface;

public:
    NoneTip();
    ~NoneTip();
    TYPE_BRUSH_TIP getType() override;
    Surface* getMask(int& size) override;
    
    void setSize(int& size) override;
    void setRotate(float& rad) override;
    int getSize() override;
    float getRotate() override;
    Transformation getTransformation() override;
};
#endif