#include "./StampRasterize.h"

StampRasterize::StampRasterize(const Point& center, const Point& sizePattern, const Point& crop, const Transformation&transformation){
    _transformation = transformation;

    _scale = transformation.getInvScale();
    halfW = sizePattern.y * 0.5f;
    halfH = sizePattern.x * 0.5f;

    transformation.transform(bounding, sizePattern, center);
    printf("sx %i, sy %i - ex %i, ey %i\n", bounding.start.x, bounding.start.y, bounding.end.x, bounding.end.y);
    
    bounding.start.x = bounding.start.x < 0 ? 0 : bounding.start.x;
    bounding.start.y = bounding.start.y < 0 ? 0 : bounding.start.y;
    bounding.end.x = bounding.end.x >= crop.x ? crop.x : bounding.end.x;
    bounding.end.y = bounding.end.y >= crop.y ? crop.y : bounding.end.y;


    _current =  bounding.start;
    _hasNext = bounding.start.x < bounding.end.x && bounding.start.y < bounding.end.y;
    _center = center;

    dx = _current.x - _center.x + 0.5f;
    dy = _current.y - _center.y + 0.5f;
    PointF srcf = _transformation.unrotate({dx,  dy});
    src.x = std::floor(srcf.x * _scale->x + halfW);
    src.y = std::floor(srcf.y * _scale->y + halfH);
    
    // printf("sx %i, sy %i - ex %f, ey %f\n", _current.x, _current.y, end.x, end.y);
}
 

StampRasterize::~StampRasterize(){}

bool StampRasterize::hasNext() const {
    //  printf("sx %i, sy %i - ex %i, ey %i\n", _current.x, _current.y, bounding.end.x, bounding.end.y);
    return _current.y <  bounding.end.y;
}
Point StampRasterize::next() {
    if(!(_current.y < bounding.end.y)){
        return _current;
    }    
    
    Point point = _current;
      

    _current.x++;

    dx = (_current.x - _center.x  + 0.5f) ;
    
    if (_current.x >=  bounding.end.x) {
        _current.x =  bounding.start.x;
        _current.y++;

        dy = (_current.y - _center.y + 0.5f) ;
    }

    PointF srcf = _transformation.unrotate({dx,  dy});
    src.x = std::floor(srcf.x * _scale->x + halfW);
    src.y = std::floor(srcf.y * _scale->y + halfH);

    return point;
}
Point StampRasterize::getSrcPoint(){
    // return _transformation.unrotate({dx,  dy});
    return src;
}