#ifndef SQUARERASTERIZE_H
#define SQUARERASTERIZE_H

#include "../../graphics/Pixel/Pixel.h"
#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"
#include <functional>

class SquareRasterize {
private:
    Point _current;

    Point _diameter, _center;
    Point _radius;

    int _thinkenss = 1;
    int _cornerRadius = 0;
    bool _isFilled = false;
    
    int offsetX;
    int offsetY;
    
    bool _hasNext;

    void putSymmetric(const int& x, const int& y, IDraw& callback);
public:
SquareRasterize(SquareRasterize& square);
SquareRasterize(const Point& diameter, const Point& center);
~SquareRasterize();

bool hasNext() const;
Point next();
void draw(IDraw& callback);

void thinkenss(int& thinkenss);
void filled(bool& isFilled);
};
#endif