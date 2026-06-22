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
    _invScale = {1.0f/_scale.x, 1.0f/_scale.y}; 
}
const PointF* Transformation::getScale() const {
    return &_scale;
}
const PointF* Transformation::getInvScale() const {
    return &_invScale;
}

void Transformation::setDelta(PointF delta){
    _delta = delta;
}
const PointF* Transformation::getDelta() const {
    return &_delta;
}


float Transformation::getRad() const {
    return _angleRad;
}
void Transformation::setRad(const float rad){
    _angleRad = rad;
    _cos = std::cos(_angleRad);
    _sin = std::sin(_angleRad);
    absCos = std::abs(_cos);
    absSin = std::abs(_sin);
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
PointF Transformation::scale(const PointF& ap) {
    return  {
        ap.x * _scale.x,
        ap.y * _scale.y
    };
};
PointF Transformation::unrotate(const PointF& ap) const{
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
void Transformation::transform(Bounding& bounding, const Point& size, const Point& c) const {
    float halfW = (size.x * _scale.x *0.5f);
    float halfH = (size.y * _scale.y *0.5f);

    float extX = halfW * absCos + halfH * absSin;
    float extY = halfW * absSin + halfH * absCos;

    bounding.start.x = (int)std::floor((c.x + 0.5f) - extX);
    bounding.start.y = (int)std::floor((c.y + 0.5f) - extY);

    bounding.end.x   = (int)std::ceil((c.x + 0.5f) + extX);
    bounding.end.y   = (int)std::ceil((c.y + 0.5f) + extY);
};