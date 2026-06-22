#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include <emscripten/emscripten.h>
#include "../Pixel/Pixel.h"

class Transformation {
private:
    PointF _scale = {1.0f,1.0f};
    PointF _invScale = {1.0f,1.0f};

    float _angleRad;
    float _cos;
    float _sin;

    float absCos;
    float absSin;
    PointF _delta;

public:
    Transformation();

    float getRad() const;
    void setRad(const float rad);

    void setScale(PointF scale);
    const PointF* getScale() const;
    const PointF* getInvScale() const;
    
    void setDelta(PointF delta);
    const PointF* getDelta() const;


    PointF distance(const PointF& p, const PointF& c);
    Point dir(const PointF& a, const PointF& b);
    PointF translate(const PointF& p, const PointF delta);
    PointF rotate(const PointF& ap);
    PointF scale(const PointF& ap);
    PointF unrotate(const PointF& ap) const;
    PointF fromWidth(float t, const PointF& c);
    PointF fromHeight (float t, const PointF& c);
    
    void transform(Bounding& bounding, const Point& size, const Point& c) const;
    void untransform(PointF& out, const PointF& ap) const;
};


#endif