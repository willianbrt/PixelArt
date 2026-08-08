#include "./CircleRasterize.h"
CircleRasterize::CircleRasterize(CircleRasterize& circle){
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
}
// CircleRasterize::CircleRasterize(const Point& from, const Point& to){
//      _radius = { 
//         (std::abs(to.x - from.x) + 1) >> 1, 
//         (std::abs(to.y - from.y) + 1) >> 1
//     };

//     _radius2 = { _radius.x * _radius.x, _radius.y * _radius.y };
//     _twoRx2 = _radius2.x << 1;
//     _twoRy2 = _radius2.y << 1;
//     _current = { 0, _radius.y};
//     _decisionParam = _radius2.y - (_radius2.x * _radius.y) + (_radius2.x >> 2);
//     _delta = {0, _twoRx2 * _current.y};
//     _hasNext = _delta.x < _delta.y;
// }
// CircleRasterize::~CircleRasterize(){}

// bool CircleRasterize::hasNext() const {
//     return _hasNext;
// }
// Point CircleRasterize::next() {
//     Point p = _current;

//     if(_delta.x < _delta.y && !_horizontal){
//     // if(!_horizontal && _delta.x <= _delta.y){
//         _current.x++;
//         _delta.x += _twoRy2;
        
//         if (_decisionParam < 0) {
//             _decisionParam += _radius2.y + _delta.x;
//         } else {
//             _current.y--;
//             _delta.y -= _twoRx2;
//             _decisionParam += _radius2.y + _delta.x - _delta.y;
//         }
//         _hasNext = true;
//         return p;
//     }
//     if( !_horizontal){
//         _decisionParam  = _radius2.y * (_current.x*_current.x) +
//                           _radius2.x *( _current.y*(_current.y - 1)) +
//                          (_radius2.x  >> 2) -
//                          _radius2.x*_radius2.y;
//         _horizontal = true;

//         // _decisionParam =
//         // _radius2.y * (_current.x * _current.x + _current.x + 0.25f) +
//         // _radius2.x * (_current.y * _current.y - 2 * _current.y + 1) -
//         // _radius2.x * _radius2.y;
//     }
    
//     if(_current.y >= 0 && _horizontal){

//         _current.y--;
//         _delta.y -= _twoRx2;

//         if (_decisionParam > 0) {
//             _decisionParam += _radius2.x - _delta.y;
//         } else {
//             _current.x++;
//             _delta.x += _twoRy2;
//             _decisionParam += _radius2.x - _delta.y + _delta.x;
//         }

//         _hasNext = _current.y >= 0;
//         // _hasNext = _current.y >= 0 || !_horizontal;
//         return p;
//     }
//     _hasNext = false;
//     return p;
// }




CircleRasterize::CircleRasterize(const Point& radius){
    _radius = radius;

    _radius2 = { _radius.x * _radius.x, _radius.y * _radius.y };
    _twoRx2 = _radius2.x << 1;
    _twoRy2 = _radius2.y << 1;

    _current = { 0, _radius.y };

    // Região 1
    _decisionParam = _radius2.y - (_radius2.x * _radius.y) + (_radius2.x >> 2);

    _delta = { 0, _twoRx2 * _current.y };

    _horizontal = false;
    _hasNext = true;
}
CircleRasterize::~CircleRasterize(){}

bool CircleRasterize::hasNext() const {
    return _hasNext;
}

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

        // fórmula correta do midpoint (região 2)
        _decisionParam =
            _radius2.y * (_current.x * _current.x + _current.x + 0.25f) +
            _radius2.x * (_current.y * _current.y - 2 * _current.y + 1) -
            _radius2.x * _radius2.y;
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