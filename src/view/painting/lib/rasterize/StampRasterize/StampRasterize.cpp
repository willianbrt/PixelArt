#include "./StampRasterize.h"

StampRasterize::StampRasterize(const PointF& center, const Point& sizePattern, const Point& crop, const Transformation&transformation){
    _transformation = transformation;
    _transformation.setDelta({ sizePattern.x * 0.5f, sizePattern.y * 0.5f });


    _transformation.transform(bounding, sizePattern, center);

    if(bounding.start.x > bounding.end.x) std::swap(bounding.start.x, bounding.end.x);
    if(bounding.start.y > bounding.end.y) std::swap(bounding.start.y, bounding.end.y);

    bounding.start.x = bounding.start.x < 0 ? 0 : bounding.start.x;
    bounding.start.y = bounding.start.y < 0 ? 0 : bounding.start.y;
    bounding.end.x = bounding.end.x >= crop.x ? crop.x : bounding.end.x;
    bounding.end.y = bounding.end.y >= crop.y ? crop.y : bounding.end.y;

    printf("b: %i, %i - %i, %i\n", bounding.start.x,bounding.start.y, bounding.end.x, bounding.end.y);

    _current =  bounding.start;
    _hasNext = bounding.start.x < bounding.end.x && bounding.start.y < bounding.end.y;
    
    delta.x = (_current.x + 0.5f - (float)center.x);
    delta.y = (_current.y + 0.5f - (float)center.y);
    _transformation.untransform(src, delta);
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
    delta.x++;
    
    if (_current.x >=  bounding.end.x) {
        _current.x =  bounding.start.x;
        _current.y++;
        delta.x -= bounding.end.x - bounding.start.x;
        delta.y++;
        
    }
    

    _transformation.untransform(src, delta);
    return point;
}
Point StampRasterize::getSrcPoint(){
    return _currentSrc;
}