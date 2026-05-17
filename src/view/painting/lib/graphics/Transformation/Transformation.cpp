#include "Transformation.h"


Transformation::Transformation(){
    _angleRad = 0;
    _cos = std::cos(_angleRad);
    _sin = std::sin(_angleRad);
    _scale = {1.0f,1.0f};
    _delta = {0.0f,0.0f};
}
void Transformation::setScale(PointF scale){
    _scale = scale;
}
const PointF* Transformation::getScale() const {
    return &_scale;
}

void Transformation::setDelta(PointF delta){
    _delta = delta;
}
const PointF* Transformation::getDelta() const {
    return &_delta;
}


float Transformation::getRad(){
    return _angleRad;
}
void Transformation::setRad(const float rad){
    _angleRad = rad;
    _cos = std::cos(_angleRad);
    _sin = std::sin(_angleRad);
}
PointF Transformation::distance(const PointF& p, const PointF& c) {
    return PointF(p.x - c.x, p.y - c.y);
};
Point Transformation::dir(const PointF& a, const PointF& b) {
    return {
        (a.x > b.x) ? -1 : 1,
        (a.y > b.y) ? -1 : 1
    };
};
PointF Transformation::translate(const PointF& p, const PointF delta) {
    return  {
       p.x +  delta.x,
       p.y +  delta.y
    };
};
PointF Transformation::rotate(const PointF& ap) {
    return  {
        ap.x * _cos - ap.y * _sin,
        ap.x * _sin + ap.y * _cos
    };
};
PointF Transformation::unrotate(const PointF& ap) {
    return  {
        ap.x * _cos + ap.y * _sin,
        -ap.x * _sin + ap.y * _cos
    };
};
PointF Transformation::fromWidth(float t, const PointF& c) {
    return {
        c.x + t * _cos,
        c.y + t * _sin
    };
};
PointF Transformation::fromHeight(float t, const PointF& c) {
    return {
        c.x - t * _sin,
        c.y + t * _cos
    };
};