#ifndef STAMPRASTERIZE_H
#define STAMPRASTERIZE_H

#include "../../graphics/Pixel/Pixel.h"
#include "../../interfaces/ISurface/ISurface.h"
#include "../../graphics/Transformation/Transformation.h"

class StampRasterize {
private:
    Point _current;
    Bounding _limit;
    Point sizePattern, sizeLimit;
    bool _hasNext;
    Bounding boundingStamp;
    PointF start, end;
    Transformation _transformation;
public:
StampRasterize(const Point& center, const Point& sizePattern, const Point& crop, const Transformation&transformation);
~StampRasterize();

bool hasNext() const;
Point next();
};
#endif