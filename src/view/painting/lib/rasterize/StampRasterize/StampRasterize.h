#ifndef STAMPRASTERIZE_H
#define STAMPRASTERIZE_H

#include "../../graphics/Pixel/Pixel.h"
#include "../../interfaces/ISurface/ISurface.h"

class StampRasterize {
private:
    Point _current;
    Bounding _limit;
    Point sizePattern, sizeLimit; 
    bool _hasNext;
    Bounding boundingStamp;

public:
StampRasterize(const Point& point, const Point& sizePattern, const Point& crop);
~StampRasterize();

bool hasNext() const;
Point next();
};
#endif