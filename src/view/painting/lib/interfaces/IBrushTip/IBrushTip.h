#ifndef IBRUSHTIP_H
#define IBRUSHTIP_H

#include "../../graphics/surface/Surface.h"
#include "../../graphics/Transformation/Transformation.h"


enum TYPE_BRUSH_TIP{
    NONE = -1,
    GEOMTRIC = 0,
    IMAGE = 1,
};
class IBrushTip {
private:
    
public:
    IBrushTip() = default;
    virtual ~IBrushTip() = default;
    virtual TYPE_BRUSH_TIP getType() = 0;
    virtual Surface* getMask(int& size) = 0;

    virtual void setSize(int& size) = 0;
    virtual void setRotate(float& rad) = 0;
    virtual int getSize() = 0;
    virtual float getRotate() = 0;
    virtual Transformation getTransformation() = 0;
};

#endif