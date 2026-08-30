#ifndef CIRCLERASTERIZE_H
#define CIRCLERASTERIZE_H

#include "../../graphics/Pixel/Pixel.h"
#include <functional>

class CircleRasterize {
private:
    Point _current;

    Point _diameter;
    Point _radius;
    Point _radius2;
    int _twoRx2;
    int _twoRy2;
    Point _outter;
    int _decisionParam;
    Point _delta;

    
    int offsetX;
    int offsetY;
    

    bool _horizontal;
    bool _hasNext;

    void putSymmetric(const int& x, const int& y, const unsigned int& color, std::function<void (const int&, const int&, const unsigned int&)> callback);
public:
CircleRasterize(CircleRasterize& circle);
CircleRasterize(const Point& diameter);
~CircleRasterize();

bool hasNext() const;
Point next();
void nextHorizontal();
void nextVertical();
void draw(std::function<void (const int&, const int&, const unsigned int&)> callback);

};
#endif