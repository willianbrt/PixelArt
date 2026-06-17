#ifndef LINERASTERIZE_H
#define LINERASTERIZE_H

#include "../../graphics/Pixel/Pixel.h"

class LineRasterize {
private:
    Point _current, _to, _dir, _delta;
    bool _hasNext;
    int _err;
public:
LineRasterize(const Point& from, const Point& to);
~LineRasterize();

bool hasNext() const;
Point next();

};
#endif