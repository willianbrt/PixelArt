#include "./CircleRasterize.h"
CircleRasterize::CircleRasterize(CircleRasterize& circle){
    _diameter = circle._diameter;
    _current = circle._current;
    _radius = circle._radius;
    _radius2 = circle._radius2;
    _twoRx2 = circle._twoRx2;
    _twoRy2 = circle._twoRy2;
    _outter = circle._outter;
    _decisionParam = circle._decisionParam;
    _delta = circle._delta;
    
    _horizontal = circle._horizontal;
    _hasNext = circle._hasNext;
    
    offsetX = circle.offsetX;
    offsetY = circle.offsetY;
}


CircleRasterize::CircleRasterize(const Point& diameter, const Point& center){
    _diameter = diameter;
    _center = center;
    _radius = { 
        diameter.x >> 1, 
        diameter.y >> 1
    };

    _radius2 = { _radius.x * _radius.x, _radius.y * _radius.y };
    _twoRx2 = _radius2.x << 1;
    _twoRy2 = _radius2.y << 1;

    _current = { 0, _radius.y };

    _decisionParam = _radius2.y - (_radius2.x * _radius.y) + (_radius2.x >> 2);

    _delta = { 0, _twoRx2 * _current.y };

    offsetX = diameter.x %2;
    offsetY = diameter.y %2;

    _horizontal = false;
    _hasNext = true;
}
CircleRasterize::~CircleRasterize(){}

bool CircleRasterize::hasNext() const {
    return _hasNext;
}

void CircleRasterize::thinkenss(int& thinkenss) { _thinkenss = thinkenss; }
void CircleRasterize::filled(bool& isFilled) { _isFilled = isFilled; }
Point CircleRasterize::next() {
    Point p = _current;

    if(!_horizontal && _delta.x < _delta.y){
        _current.x++;
        _delta.x += _twoRy2;

        if (_decisionParam < 0) {
            _decisionParam += _radius2.y + _delta.x;
        } else {
            _current.y--;
            _delta.y -= _twoRx2;
            _decisionParam += _radius2.y + _delta.x - _delta.y;
        }

        _hasNext = true;
        return p;
    }

    if(!_horizontal){
        _horizontal = true;

        _decisionParam  = _radius2.y * (_current.x*_current.x) +
                            _radius2.x *( _current.y*(_current.y - 1)) +
                            (_radius2.x  >> 2) -
                            _radius2.x*_radius2.y;
    }
    
    if(_current.y >= 0){
        _current.y--;
        _delta.y -= _twoRx2;

        if (_decisionParam > 0) {
            _decisionParam += _radius2.x - _delta.y;
        } else {
            _current.x++;
            _delta.x += _twoRy2;
            _decisionParam += _radius2.x - _delta.y + _delta.x;
        }

        _hasNext = _current.y >= 0;
        return p;
    }

    _hasNext = false;
    return p;
}

void CircleRasterize::nextHorizontal() {
    _current.x++;
    _delta.x += _twoRy2;

    if (_decisionParam < 0) {
        _decisionParam += _radius2.y + _delta.x;
    } else {
        _current.y--;
        _delta.y -= _twoRx2;
        _decisionParam += _radius2.y + _delta.x - _delta.y;
    }
}

void CircleRasterize::nextVertical() {
    _current.y--;
    _delta.y -= _twoRx2;

    if (_decisionParam > 0) {
        _decisionParam += _radius2.x - _delta.y;
    } else {
        _current.x++;
        _delta.x += _twoRy2;
        _decisionParam += _radius2.x - _delta.y + _delta.x;
    }
}
void CircleRasterize::draw(IDraw& callback) {
    Point innerDiameter = (_isFilled) ? Point(0, 0) : 
        Point(std::max((_radius.x - _thinkenss + 1) << 1, 0), std::max((_radius.y - _thinkenss + 1) << 1, 0));
    CircleRasterize icircle(innerDiameter, {0,0});

    Point lStart = {INT_MAX, INT_MAX};
    
    while(_delta.x < _delta.y){
        for(int y = icircle._current.y; y <= _current.y; y++){
            putSymmetric(_current.x, y, callback);
        }

        lStart = icircle._current;
                    
        if(icircle._delta.x < icircle._delta.y){
            icircle.nextHorizontal();
        }
        nextHorizontal();
    }
    
    _horizontal = true;

    _decisionParam  = _radius2.y * (_current.x*_current.x) +
    _radius2.x *( _current.y*(_current.y - 1)) +
    (_radius2.x  >> 2) -
    _radius2.x*_radius2.y;
    
    icircle._decisionParam  = icircle._radius2.y * (icircle._current.x*icircle._current.x) +
    icircle._radius2.x *(icircle._current.y*(icircle._current.y - 1)) +
    (icircle._radius2.x  >> 2) -
    icircle._radius2.x*icircle._radius2.y;
    
    while(_current.y >= 0){
        if(lStart.y <= _current.y && lStart.x != _current.x){
            for(int y = lStart.y; y <= _current.y; y++){
                putSymmetric(_current.x, y, callback);
            }
            lStart.x = _current.x;
        }else if(lStart.y > _current.y){
            for(int x = icircle._current.x; x <= _current.x; x++){
                putSymmetric(x, _current.y, callback);
            }
        }

        if(icircle._current.y > 0 && icircle._current.y == _current.y)
            icircle.nextVertical();
        nextVertical();
        
    }
    _hasNext = false;
}
void CircleRasterize::putSymmetric(const int& x, const int& y, IDraw& callback){
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