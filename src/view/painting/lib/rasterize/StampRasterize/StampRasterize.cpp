#include "./StampRasterize.h"

StampRasterize::StampRasterize(const Point& center, const Point& sizePattern, const Point& crop, const Transformation&transformation){
    cos = std::cos(transformation.getRad());
    sin = std::sin(transformation.getRad());

    _scale = transformation.getInvScale();
    const PointF* scale = transformation.getScale();
    float halfW = sizePattern.x * 0.5f;
    float halfH = sizePattern.y * 0.5f;

    transformation.transform(bounding, sizePattern, center);
    // printf("sx %i, sy %i - ex %i, ey %i - hw %f, hh %f - cx %i, cy %i \n",
    //      bounding.start.x, bounding.start.y,
    //      bounding.end.x, bounding.end.y,
    //      halfW, halfH, center.x, center.y);
    
    bounding.start.x = bounding.start.x < 0 ? 0 : bounding.start.x;
    bounding.start.y = bounding.start.y < 0 ? 0 : bounding.start.y;
    bounding.end.x = bounding.end.x >= crop.x ? crop.x : bounding.end.x;
    bounding.end.y = bounding.end.y >= crop.y ? crop.y : bounding.end.y;


    _current =  bounding.start;
    _hasNext = bounding.start.x < bounding.end.x && bounding.start.y < bounding.end.y;
    
    initial = transformation.unrotate({ (_current.x + 0.5f - (float)center.x),  (_current.y + 0.5f - (float)center.y) });
    initial.x *= _scale->x;
    initial.y *= _scale->y;
    initial.x += halfW;
    initial.y += halfH;
    src = initial;
}
 

StampRasterize::~StampRasterize(){}

bool StampRasterize::hasNext() const {
    return _current.y <  bounding.end.y;
}
Point StampRasterize::next() {
    if(!(_current.y < bounding.end.y)){
        _currentSrc = { (int)std::floor(src.x), (int)std::floor(src.y) };
        return _current;
    }    
    
    Point point = _current;
    _currentSrc = { (int)std::floor(src.x), (int)std::floor(src.y) };      

    _current.x++;

    src.x += cos * _scale->x;
    src.y -= sin * _scale->y;

    // src.x *= _scale->x;
    // src.y *= _scale->y;
    if (_current.x >=  bounding.end.x) {
        _current.x =  bounding.start.x;
        _current.y++;

        initial.x += sin * _scale->x;
        initial.y += cos * _scale->y;
        
        src.x = initial.x;
        src.y = initial.y;
    }



    return point;
}
Point StampRasterize::getSrcPoint(){
    return _currentSrc;
}