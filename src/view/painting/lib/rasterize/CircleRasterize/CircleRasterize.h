#ifndef CIRCLERASTERIZE_H
#define CIRCLERASTERIZE_H

#include "../../graphics/Pixel/Pixel.h"
#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"
#include <functional>

class CircleRasterize {
private:
    Point _current;

    Point _diameter, _center;
    Point _radius;
    Point _radius2;
    int _twoRx2;
    int _twoRy2;
    Point _outter;
    int _decisionParam;
    Point _delta;
    int _thinkenss = 1;
    bool _isFilled = false;
    
    int offsetX;
    int offsetY;
    

    bool _horizontal;
    bool _hasNext;

    void putSymmetric(const int& x, const int& y, IDraw& callback);
public:
CircleRasterize(CircleRasterize& circle);
CircleRasterize(const Point& diameter, const Point& center);
~CircleRasterize();

bool hasNext() const;
Point next();
void nextHorizontal();
void nextVertical();
void draw(IDraw& callback);

void thinkenss(int& thinkenss);
void filled(bool& isFilled);
};
#endif