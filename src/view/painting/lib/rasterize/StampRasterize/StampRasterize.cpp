#include "./StampRasterize.h"

StampRasterize::StampRasterize(const Point& center, const Point& sizePattern, const Point& crop, const Transformation&transformation){
    _transformation = transformation;

    // Point resizedPattern = _transformation.scale(sizePattern);
    
    const PointF* scale = _transformation.getScale();
    float halfW = (sizePattern.x * scale->x *0.5f);
    float halfH = (sizePattern.y * scale->y *0.5f);

    start = {
        center.x - halfW,
        center.y - halfH
    };

    end = {
        center.x + halfW,
        center.y + halfH
    };
    start = _transformation.rotate(start);
    end = _transformation.rotate(end);


    start.x = start.x < 0.0f ? 0.0f : start.x;
    start.y = start.y < 0.0f ? 0.0f : start.y;
    end.x = end.x >= crop.x ? crop.x : end.x;  
    end.y = end.y >= crop.y ? crop.y : end.y;

    // start.x = start.x < 0.0f ? 0.0f : start.x;
    // start.y = start.y < 0.0f ? 0.0f : start.y;
    // end.x = start.x + sizePattern.x >= crop.x ? crop.x : start.x + sizePattern.x;  
    // end.y = start.y + sizePattern.y >= crop.y ? crop.y : start.y + sizePattern.y;

    _current = {(int)std::floor(start.x+0.5f), (int)std::floor(start.y+0.5f)};
    _hasNext = (start.x < end.x && start.y < end.y);
    
if(_hasNext)
    printf("sx %i, sy %i - ex %f, ey %f\n", _current.x, _current.y, end.x, end.y);
}
 

StampRasterize::~StampRasterize(){}

bool StampRasterize::hasNext() const {
    printf("py %i, ey %f\n", _current.y, end.y);
    return _current.y < end.y;
}
Point StampRasterize::next() {
    if(!(_current.y < end.y)){
        return _current;
    }    
    
    Point point = _current;


    _current.x++;
    
    if (_current.x >=  end.x) {
        _current.x = start.x;
        _current.y++;
    }

    return point;
}