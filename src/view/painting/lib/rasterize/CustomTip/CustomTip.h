#ifndef CUSTOMTIP_H
#define CUSTOMTIP_H

#include "../../interfaces/IBrushTip/IBrushTip.h"

class CustomTip : public IBrushTip {
private:
    Surface* _surface;

    float _rad;
    int _size;

    Transformation _transform;
public:
    CustomTip(unsigned int* buffer, int width, int height);
    ~CustomTip();
    TYPE_BRUSH_TIP getType() override;
    Surface* getMask() override;

    void setSize(int& size) override;
    void setRotate(float& rad) override;
    int getSize() override;
    float getRotate() override;
    Transformation getTransformation() override;
};
#endif