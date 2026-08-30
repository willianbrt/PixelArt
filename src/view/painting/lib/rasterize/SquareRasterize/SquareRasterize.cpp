#include "./SquareRasterize.h"
SquareRasterize::SquareRasterize(SquareRasterize& square){
    _diameter = square._diameter;
    _current = square._current;
    _radius = square._radius;
    
    _hasNext = square._hasNext;
    
    offsetX = square.offsetX;
    offsetY = square.offsetY;
}


SquareRasterize::SquareRasterize(const Point& diameter, const Point& center){
    _diameter = diameter;
    _center = center;
    _radius = { 
        diameter.x >> 1, 
        diameter.y >> 1
    };
    _current = { 0, _radius.y };

    offsetX = diameter.x %2;
    offsetY = diameter.y %2;
    _hasNext = true;
}
SquareRasterize::~SquareRasterize(){}

bool SquareRasterize::hasNext() const {
    return _hasNext;
}

void SquareRasterize::thinkenss(int& thinkenss) { _thinkenss = thinkenss; }
void SquareRasterize::filled(bool& isFilled) { _isFilled = isFilled; }
Point SquareRasterize::next() {
    Point p = _current;


    _hasNext = false;
    return p;
}

void SquareRasterize::draw(IDraw& callback) {
    for(int x = 0; x <= _radius.x-_cornerRadius; x++){
        for(int t = 0; t < _thinkenss; t++){
            if(_radius.y - t < 0) break;
            putSymmetric(x, _radius.y - t, callback);
        }
    }

    for(int y = 0; y <= _radius.y-_thinkenss-_cornerRadius; y++){
        for(int t = 0; t < _thinkenss; t++){
            if(_radius.x - t < 0) break;
            putSymmetric(_radius.x - t, y, callback);
        }
    }
    
    if(_isFilled){
        for(int y = 0; y <= _radius.y-_thinkenss; y++){
            for(int x = 0; x <= _radius.x-_thinkenss; x++){
                putSymmetric(x, y, callback);
            }
        }
    }
}
void SquareRasterize::putSymmetric(const int& x, const int& y, IDraw& callback){
    if(!(x == 0 && offsetX == 0)){
        if(!(y == 0 && offsetY == 0)){
            callback.plot(_center.x + x + offsetX, _center.y + y + offsetY);
        }
        callback.plot(_center.x + x + offsetX, _center.y - y);
    }
    if(!(y == 0 && offsetY == 0)){
        callback.plot(_center.x - x, _center.y + y + offsetY);
    }
   callback.plot(_center.x - x, _center.y - y);
}