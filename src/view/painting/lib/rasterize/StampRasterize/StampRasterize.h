#ifndef STAMPRASTERIZE_H
#define STAMPRASTERIZE_H

#include "../../graphics/Pixel/Pixel.h"
#include "../../interfaces/ISurface/ISurface.h"
#include "../../graphics/Transformation/Transformation.h"

class StampRasterize {
private:
    Transformation _transformation;
    Point _current, _currentSrc;
    bool _hasNext;
    Bounding bounding;
    PointF src, delta;
public:
StampRasterize(const Point& center, const Point& sizePattern, const Point& crop, const Transformation&transformation);
~StampRasterize();

bool hasNext() const;
Point next();
Point getSrcPoint();
};
#endif