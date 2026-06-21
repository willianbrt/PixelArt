#ifndef STAMPRASTERIZE_H
#define STAMPRASTERIZE_H

#include "../../graphics/Pixel/Pixel.h"
#include "../../interfaces/ISurface/ISurface.h"
#include "../../graphics/Transformation/Transformation.h"

class StampRasterize {
private:
    Point _current;
    Point _center, src;
    bool _hasNext;
    Bounding bounding;
    float halfW, halfH;
    float dx, dy;
    Transformation _transformation;
    const PointF* _scale;
public:
StampRasterize(const Point& center, const Point& sizePattern, const Point& crop, const Transformation&transformation);
~StampRasterize();

bool hasNext() const;
Point next();
Point getSrcPoint();
};
#endif