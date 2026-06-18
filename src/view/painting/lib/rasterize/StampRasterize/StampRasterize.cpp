#include "./StampRasterize.h"

StampRasterize::StampRasterize(const Point& point, const Point& sizePattern, const Point& crop){


    Point startPixel = {
        point.x - (sizePattern.x >> 1),
        point.y - (sizePattern.y >> 1)
    };

    if(startPixel.x >= crop.x || startPixel.y >= crop.y) return;
    if(startPixel.x < -sizePattern.x || startPixel.y < -sizePattern.y) return;


    
    boundingStamp.start.x = startPixel.x < 0 ? 0 : startPixel.x;
    boundingStamp.start.y = startPixel.y < 0 ? 0 : startPixel.y;
    boundingStamp.end.x = startPixel.x + sizePattern.x >= crop.x ? crop.x : startPixel.x + sizePattern.x;  
    boundingStamp.end.y = startPixel.y + sizePattern.y >= crop.y ? crop.y : startPixel.y + sizePattern.y;
    
    
    _hasNext =  !(point.y == boundingStamp.end.x && point.y == boundingStamp.end.y);

    _current = boundingStamp.start;
    // int startSrcX = startPixel.x < 0 ? -startPixel.x  / _drawingContext->size : 0;
    // int startErrX = startPixel.x < 0 ? startPixel.x % sizePattern.x : 0;
    
    // int srcY =  startPixel.y < 0 ? -startPixel.y  / _drawingContext->size : 0;
    // int errY = startPixel.y < 0 ? startPixel.y % sizePattern.y : 0;
    

}
StampRasterize::~StampRasterize(){}

bool StampRasterize::hasNext() const {
    return _current.y < boundingStamp.end.y;
}
Point StampRasterize::next() {
    if(!(_current.y < boundingStamp.end.y)){
        return _current;
    }

    Point point = _current;
    
    _current.x++;
    
    if (_current.x >=  boundingStamp.end.x) {
        _current.x = boundingStamp.start.x;
        _current.y++;
    }


    return point;

}