#include "Selection.h"

Selection::Selection(int from_start_x, int from_start_y, int to_start_x, int to_start_y){
    _originalBounding = Bounding(Point(from_start_x,from_start_y), Point(to_start_x+1, to_start_y+1));

    _destBounding = _originalBounding;
    _origCenterX = (_originalBounding.start.x + _originalBounding.end.x) * 0.5f;
    _origCenterY = (_originalBounding.start.y + _originalBounding.end.y) * 0.5f;

    _angleRad = 0;
    _cosA = std::cos(_angleRad);
    _sinA = std::sin(_angleRad);

    _dstCenterX = _origCenterX;
    _dstCenterY = _origCenterY;

    _resizedWidth = _originalBounding.getWidth() ;
    _resizedHeight = _originalBounding.getHeight() ;

    _corners = Corners();

    _corners.topLeft     = Point(_originalBounding.start.x, _originalBounding.start.y);
    _corners.bottomRight = Point(_originalBounding.end.x, _originalBounding.end.y);
    _corners.topRight    = Point(_originalBounding.end.x, _originalBounding.start.y);
    _corners.bottomLeft  = Point(_originalBounding.start.x, _originalBounding.end.y);
}

void Selection::translate(float deltaX, float deltaY){
    _corners.topLeft.x += deltaX;
    _corners.topLeft.y += deltaY;

    _corners.bottomRight.x += deltaX;
    _corners.bottomRight.y += deltaY;

    _corners.topRight.x += deltaX;
    _corners.topRight.y += deltaY;

    _corners.bottomLeft.x += deltaX;
    _corners.bottomLeft.y += deltaY;
    
    _dstCenterX += deltaX;
    _dstCenterY += deltaY;
}

void Selection::rotate(float rotateRad){
    _angleRad += rotateRad;
    _cosA = std::cos(_angleRad);
    _sinA = std::sin(_angleRad);

    float halfWidth =  _resizedWidth * 0.5f;
    float halfHeight = _resizedHeight * 0.5f;

    float widthX =  _cosA * halfWidth;
    float widthY =  _sinA * halfWidth;

    float heightX = -_sinA * halfHeight;
    float heightY =  _cosA * halfHeight;
    _corners.topLeft.x = std::round(_dstCenterX - widthX - heightX);
    _corners.topLeft.y = std::round(_dstCenterY - widthY - heightY);
    
    _corners.topRight.x = std::round(_dstCenterX + widthX - heightX);
    _corners.topRight.y = std::round(_dstCenterY + widthY - heightY);

    _corners.bottomRight.x = std::round(_dstCenterX + widthX + heightX);
    _corners.bottomRight.y = std::round(_dstCenterY + widthY + heightY);

    _corners.bottomLeft.x = std::round(_dstCenterX - widthX + heightX);
    _corners.bottomLeft.y = std::round(_dstCenterY - widthY + heightY);
    
    // _angleRad = std::atan2(_corners.topRight.x - _corners.topLeft.x, _corners.topRight.y - _corners.topLeft.y);
    // _cosA = std::cos(_angleRad);
    // _sinA = std::sin(_angleRad);
}
void Selection::resize(int marker, float deltaX, float deltaY){
    Point pixel = Point(deltaX, deltaY);

    auto calcWidth = [&](Point& P, Point& C){
        Point AP = Point(P.x - C.x, P.y - C.y);
        return (float)AP.x * _cosA  + (float)AP.y * _sinA;
    };

    auto calcHeight = [&](Point& P, Point& C){
        Point AP = Point(P.x - C.x, P.y - C.y);
        return (float)AP.x * -_sinA  + (float)AP.y * _cosA;
    };
    
    auto projectHeight = [&](Point& P, Point& C){
        Point AP = Point(P.x - C.x, P.y - C.y);
        float t = (float)AP.x * _cosA  + (float)AP.y * _sinA;
        
        return Point(static_cast<int>(std::round(C.x + t * _cosA)), static_cast<int>(std::round(C.y + t * _sinA)));
    };
    auto projectWidth = [&](Point& P, Point& C){
        Point AP = Point(P.x - C.x, P.y - C.y);
        float t = (float)AP.x * -_sinA  + (float)AP.y * _cosA;
        
        return Point(static_cast<int>(std::round(C.x + t * -_sinA)), static_cast<int>(std::round(C.y + t * _cosA)));
    };

    Point pivot;
    if((ENUM_MARKER)marker ==  ENUM_MARKER::TOP_LEFT){
        pivot = _corners.bottomRight;

        _corners.topLeft = pixel;
        _corners.topRight = projectWidth(pixel, pivot);
        _corners.bottomLeft = projectHeight(pixel, pivot);
        
        _resizedWidth = -calcWidth(pixel, pivot);
        _resizedHeight = -calcHeight(pixel, pivot);
    } else if((ENUM_MARKER)marker ==ENUM_MARKER::TOP_RIGHT){
        pivot = _corners.bottomLeft;

        _corners.topRight = pixel;
        _corners.topLeft   = projectWidth(pixel, pivot);
        _corners.bottomRight  = projectHeight(pixel, pivot);

        _resizedWidth = calcWidth(pixel, pivot);
        _resizedHeight = -calcHeight(pixel, pivot);
    } else if((ENUM_MARKER)marker ==ENUM_MARKER::BOTTOM_RIGHT){
        pivot = _corners.topLeft;

        _corners.bottomRight = pixel;
        _corners.bottomLeft = projectWidth(pixel, pivot);
        _corners.topRight = projectHeight(pixel, pivot);

        _resizedWidth = calcWidth(pixel, pivot);
        _resizedHeight = calcHeight(pixel, pivot);
    } else if((ENUM_MARKER)marker ==ENUM_MARKER::BOTTOM_LEFT){
        pivot = _corners.topRight;

        _corners.bottomLeft = pixel;
        _corners.bottomRight = projectWidth(pixel, pivot);
        _corners.topLeft = projectHeight(pixel, pivot);
            
        _resizedWidth = -calcWidth(pixel, pivot);
        _resizedHeight = calcHeight(pixel, pivot);
    } else {
        std::runtime_error("Corner inválido.");
        return;
    }


    _scaleX =  _resizedWidth / (float)_originalBounding.getWidth();
    _scaleY =  _resizedHeight / (float)_originalBounding.getHeight();

    _dstCenterX = (_corners.topLeft.x + _corners.topRight.x + _corners.bottomLeft.x + _corners.bottomRight.x) * 0.25f;
    _dstCenterY = (_corners.topLeft.y + _corners.topRight.y + _corners.bottomLeft.y + _corners.bottomRight.y) * 0.25f; 
}
Point Selection::getCenter(){
    return Point(_dstCenterX, _dstCenterY);
}

int Selection::getResizedWidth(){
    return _resizedWidth;
}
int Selection::getResizedHeight(){
    return _resizedHeight;
}

Bounding Selection::getBounding(){
    return _originalBounding;
}

Corners Selection::getDestinationCorners(){
    return _corners;
}
float Selection::getRotateRad(){
    return _angleRad;
}
void Selection::draw(Layer& layer){
    Layer* temp = new Layer(layer);

    for (int y = _originalBounding.start.y; y < _originalBounding.end.y; ++y) {
        for (int x = _originalBounding.start.x; x < _originalBounding.end.x; ++x) {
            layer.putPixel((int)x, (int)y, 0x0);
        }
    }

    float hw = _resizedWidth * 0.5f;
    float hh = _resizedHeight * 0.5f;
    
    if (_resizedWidth < 0) hw = -hw;
    if (_resizedHeight < 0) hh = -hh;

    Bounding destBounding = _corners.getBounding();

    // printf("======================== \n");
    // printf("orig_inicio: (%i, %i), orig_fim: (%i, %i) \n",  _originalBounding.start.x,  _originalBounding.start.y, _originalBounding.end.x, _originalBounding.end.y);
    // printf("dest_inicio: (%i, %i), dest_fim: (%i, %i) \n",  destBounding.start.x,  destBounding.start.y, destBounding.end.x, destBounding.end.y);
    // printf("centerOrigem: (%f, %f), centerDest: (%f, %f) \n",  _origCenterX, _origCenterY,  _dstCenterX, _dstCenterY);
    // printf("scale: (%f, %f) \n",  _scaleX, _scaleY);
    // printf("angleRad: (%f) \n",  _angleRad);
    
    for (int dy = destBounding.start.y; dy < destBounding.end.y; dy++){
        Point src;
        float _dy = dy + 0.5f - _dstCenterY;

        for (int dx = destBounding.start.x; dx < destBounding.end.x; dx++) {
            float _dx = dx + 0.5f - _dstCenterX;
            
            float localX = _cosA * _dx + _sinA * _dy;
            float localY = -_sinA * _dx + _cosA * _dy;

            // printf("(%f,%f) - orig center(%f,%f) - dest center(%f,%f)\n",
            //      std::floor(localX), std::floor(localY),
            //      _origCenterX, _origCenterY,
            //      _dstCenterX, _dstCenterY
            //     );
            
            if(localX < -hw || localX > hw || localY < -hh || localY > hh){
                continue;
            }

            src.x = std::floor(localX / _scaleX + _origCenterX);
            src.y = std::floor(localY / _scaleY + _origCenterY);

            unsigned int color = temp->getPixel(src.x, src.y);

            if((color & 0xFF) == 0) { continue; }
            layer.putPixel(dx, dy, color);
        }
    }
        
    // printf("======================== \n");
}


void Selection::rotatePoint(Point& p, Point center){
    float dx = p.x - center.x;
    float dy = p.y - center.y;

    float rx = dx * _cosA + dy * -_sinA;
    float ry = dx * _sinA + dy * _cosA;

    p.x = static_cast<int>(std::round(rx + center.x));
    p.y = static_cast<int>(std::round(ry + center.y));
};


using namespace emscripten;

EMSCRIPTEN_BINDINGS(selection_module){
    class_<Selection, base<IGraphic>>("Selection")
        .constructor<int, int, int, int>()
        .smart_ptr<std::shared_ptr<Selection>>("shared_ptr<Selection>")
        .function("draw", &Selection::draw)
        .function("getBounding", &Selection::getBounding)
        .function("getDestinationCorners", &Selection::getDestinationCorners)
        .function("translate", &Selection::translate)
        .function("rotate", &Selection::rotate)
        .function("getRotateRad", &Selection::getRotateRad)
        .function("getCenter", &Selection::getCenter)
        .function("getResizedWidth", &Selection::getResizedWidth)
        .function("getResizedHeight", &Selection::getResizedHeight)
        .function("resize", &Selection::resize)
        ;
};
