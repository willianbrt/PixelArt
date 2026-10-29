#ifndef CUSTOMTIP_H
#define CUSTOMTIP_H

#include "../../interfaces/IBrushTip/IBrushTip.h"

class CustomTip : public IBrushTip {
private:
    unsigned int* _buffer;
    int _width;
    int _height;
public:
    CustomTip(unsigned int* buffer, int width, int height);
    ~CustomTip();
    TYPE_BRUSH_TIP getType() override;
    Surface* getMask(int& size) override;
};
#endif