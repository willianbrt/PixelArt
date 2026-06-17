#include "./LineRasterize.h"

LineRasterize::LineRasterize(const Point& from, const Point& to){
    _to = to;
    _current.x = from.x;
    _current.y = from.y;
    _hasNext = !(_current.x == _to.x && _current.y == _to.y);

    _delta.x = std::abs(_to.x - _current.x);
    _delta.y = -std::abs(_to.y - _current.y);

    _dir.x = (_current.x < _to.x) ? 1 : (_current.x > _to.x ? -1 : 0);
    _dir.y = (_current.y < _to.y) ? 1 : (_current.y > _to.y ? -1 : 0);

    _err = _delta.x + _delta.y;
}
LineRasterize::~LineRasterize(){}

bool LineRasterize::hasNext() const {
    return _hasNext;
}
Point LineRasterize::next() {
    Point p = _current;

    if (_current.x == _to.x && _current.y == _to.y) {
        _hasNext = false;
        return p;
    }

    int e2 = 2 * _err;

    if (e2 >= _delta.y) {
        _err += _delta.y;
        _current.x += _dir.x;
    }

    if (e2 <= _delta.x) {
        _err += _delta.x;
        _current.y += _dir.y;
    }
    
    _hasNext = true;
    return p;
}