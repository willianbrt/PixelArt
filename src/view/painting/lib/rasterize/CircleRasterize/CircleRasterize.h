#ifndef CIRCLERASTERIZE_H
#define CIRCLERASTERIZE_H

#include "../../graphics/Pixel/Pixel.h"

class CircleRasterize {
private:
    Point _current;

    Point _radius;
    Point _radius2;
    int _twoRx2;
    int _twoRy2;
    Point _outter;
    int _decisionParam;
    Point _delta;

    bool _horizontal;
    bool _hasNext;

public:
CircleRasterize(CircleRasterize& circle);
CircleRasterize(const Point& radius);
~CircleRasterize();

bool hasNext() const;
Point next();

};
#endif