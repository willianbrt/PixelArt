#include "Selection.h"

Selection::Selection(
    int from_start_x, int from_start_y,
    int to_start_x, int to_start_y, 
    Surface& layer, 
    bool cleanTheArea,
    bool isMirrorX, bool isMirrorY, 
    int nRows, int nCols) : IGraphic(isMirrorX, isMirrorY, nRows, nCols)
{
    _originalBounding = Bounding(
        Point(from_start_x,from_start_y),
        Point(to_start_x+1, to_start_y+1)
    );
    const int screenWidth = layer.getWidth()*_nRows;
    const int screenHeight = layer.getHeight()*_nCols;
    
    _originalBounding.start.x = std::min(screenWidth, std::max(0 ,_originalBounding.start.x));
    _originalBounding.start.y = std::min(screenHeight, std::max(0, _originalBounding.start.y));
    _originalBounding.end.x = std::min(screenWidth, std::max(0, _originalBounding.end.x));
    _originalBounding.end.y = std::min(screenHeight, std::max(0, _originalBounding.end.y));

    _data = new Surface(_originalBounding.getWidth(), _originalBounding.getHeight());
    
    for (int y = 0; y < _originalBounding.getHeight(); ++y) {
        Point p;
        p.y = (_originalBounding.start.y + y) %layer.getHeight();
        for (int x = 0; x < _originalBounding.getWidth(); ++x){
            p.x = (_originalBounding.start.x + x)%layer.getWidth();
            _data->putPixel(x, y, layer.getPixel(p.x , p.y));
        }
    }
    
    _destBounding = _originalBounding;
    _origCenterX = (_originalBounding.start.x + _originalBounding.end.x) * 0.5f;
    _origCenterY = (_originalBounding.start.y + _originalBounding.end.y) * 0.5f;
    
    _cleanTheArea = cleanTheArea;

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
Selection::~Selection(){
    free(_data);
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
}
void Selection::resize(int marker, float deltaX, float deltaY){
    Point pixel = Point(deltaX, deltaY);

    auto toLocal = [&](const Point& p, const Point& c) {
        return Point(p.x - c.x, p.y - c.y);
    };
    auto projWidth = [&](const Point& ap) {
        return  ap.x * _cosA + ap.y * _sinA;
    };
    auto projHeight = [&](const Point& ap) {
        return -ap.x * _sinA + ap.y * _cosA;
    };
    auto fromWidth = [&](float t, const Point& c) {
        return Point(
            static_cast<int>(std::round(c.x + t * _cosA)),
            static_cast<int>(std::round(c.y + t * _sinA))
        );
    };
    auto fromHeight = [&](float t, const Point& c) {
        return Point(
            static_cast<int>(std::round(c.x - t * _sinA)),
            static_cast<int>(std::round(c.y + t * _cosA))
        );
    };

    Point pivot;
    float signW = 1.0f;
    float signH = 1.0f;

    Point* dragged = nullptr;
    Point* cornerH = nullptr;
    Point* cornerW = nullptr;

    switch ((ENUM_MARKER)marker) {
        case ENUM_MARKER::TOP_LEFT:
            pivot   = _corners.bottomRight;
            dragged = &_corners.topLeft;
            cornerH = &_corners.topRight;
            cornerW = &_corners.bottomLeft;
            signW = -1; signH = -1;
            break;

        case ENUM_MARKER::TOP_RIGHT:
            pivot   = _corners.bottomLeft;
            dragged = &_corners.topRight;
            cornerH = &_corners.topLeft;
            cornerW = &_corners.bottomRight;
            signW =  1; signH = -1;
            break;

        case ENUM_MARKER::BOTTOM_RIGHT:
            pivot   = _corners.topLeft;
            dragged = &_corners.bottomRight;
            cornerH = &_corners.bottomLeft;
            cornerW = &_corners.topRight;
            signW =  1; signH =  1;
            break;

        case ENUM_MARKER::BOTTOM_LEFT:
            pivot   = _corners.topRight;
            dragged = &_corners.bottomLeft;
            cornerH = &_corners.bottomRight;
            cornerW = &_corners.topLeft;
            signW = -1; signH =  1;
            break;
    default:
        throw std::runtime_error("Corner inválido");
    }

    Point ap = toLocal(pixel, pivot);

    _resizedWidth = signW * projWidth(ap);
    _resizedHeight = signH * projHeight(ap);

    *cornerW = fromWidth(signW * _resizedWidth, pivot);
    *cornerH = fromHeight(signH * _resizedHeight, pivot);

    (*dragged) = Point(
                    cornerH->x + cornerW->x - pivot.x,
                    cornerH->y + cornerW->y - pivot.y
                );
                
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
    const int screenWidth = layer.getWidth()*_nRows;
    const int screenHeight = layer.getHeight()*_nCols;

    if(_cleanTheArea){
        for (int y = _originalBounding.start.y; y < _originalBounding.end.y; ++y) {
            for (int x = _originalBounding.start.x; x < _originalBounding.end.x; ++x) {
                layer.putPixel((int)x%layer.getWidth(), (int)y%layer.getHeight(), 0x0);
            }
        }
    }

    float hw = _resizedWidth * 0.5f;
    float hh = _resizedHeight * 0.5f;
    
    if (_resizedWidth < 0) hw = -hw;
    if (_resizedHeight < 0) hh = -hh;

    const int offsetX = -_originalBounding.start.x + std::min(_originalBounding.start.x, 0);
    const int offsetY = -_originalBounding.start.y + std::min(_originalBounding.start.y, 0);

    Bounding destBounding = _corners.getBounding();
    for (int dy = destBounding.start.y; dy < destBounding.end.y; dy++){
        Point src;
        float _dy = dy + 0.5f - _dstCenterY;
        
        for (int dx = destBounding.start.x; dx < destBounding.end.x; dx++) {            
            float _dx = dx + 0.5f - _dstCenterX;
            
            float localX = _cosA * _dx + _sinA * _dy;
            float localY = -_sinA * _dx + _cosA * _dy;
            
            if(localX < -hw || localX > hw || localY < -hh || localY > hh){
                continue;
            }

            src.x = std::floor(localX / _scaleX + _origCenterX + offsetX);
            src.y = std::floor(localY / _scaleY + _origCenterY + offsetY);

            unsigned int color = _data->getPixel(src.x, src.y);

            if((color >> 24 & 0xFF) == 0) { continue; }
            
            putPixel(layer, dx, dy, color, screenWidth, screenHeight);
        }
    }
}

void Selection::putPixel(Layer& layer, int x, int y, unsigned int color, int screenWidth, int screenHeight){
    Point p;
    p.x = GraphicsEngine::clampedTilePoint(x, layer.getWidth());
    p.y = GraphicsEngine::clampedTilePoint(y, layer.getHeight());

    layer.putPixel(p.x, p.y, color);
    
    int pointMirrorX = GraphicsEngine::pointMirrored(p.x, layer.getWidth());
    int pointMirrorY = GraphicsEngine::pointMirrored(p.x, layer.getHeight());

    if(_isMirrorX){
        layer.putPixel(pointMirrorX, p.y, color);
    }            
    if(_isMirrorY){
        layer.putPixel(p.x, pointMirrorY, color);
    }
    if(_isMirrorX && _isMirrorY){
        layer.putPixel(pointMirrorX, pointMirrorY, color);
    }
}
void Selection::remove(){
    _corners.topLeft     = Point(0, 0);
    _corners.bottomRight = Point(0, 0);
    _corners.topRight    = Point(0, 0);
    _corners.bottomLeft  = Point(0, 0);
}
Surface* Selection::copy(){
    float hw = _resizedWidth * 0.5f;
    float hh = _resizedHeight * 0.5f;
    
    if (_resizedWidth < 0) hw = -hw;
    if (_resizedHeight < 0) hh = -hh;
    
    Bounding destBounding = _corners.getBounding();
    Surface* surface = new Surface(destBounding.getWidth(), destBounding.getHeight());
    
    for (int dy = destBounding.start.y; dy < destBounding.end.y; dy++){
        Point src;
        float _dy = dy + 0.5f - _dstCenterY;

        for (int dx = destBounding.start.x; dx < destBounding.end.x; dx++) {
            float _dx = dx + 0.5f - _dstCenterX;
            
            float localX = _cosA * _dx + _sinA * _dy;
            float localY = -_sinA * _dx + _cosA * _dy;
            
            if(localX < -hw || localX > hw || localY < -hh || localY > hh){
                continue;
            }

            src.x = std::floor(localX / _scaleX + _origCenterX - _originalBounding.start.x);
            src.y = std::floor(localY / _scaleY + _origCenterY - _originalBounding.start.y);
            
            unsigned int color = _data->getPixel(src.x, src.y);

            if((color & 0xFF) == 0) { continue; }
            surface->putPixel(dx-destBounding.start.x , dy-destBounding.start.y, color);
        }
    }

    return surface;
}

using namespace emscripten;

EMSCRIPTEN_BINDINGS(selection_module){
    class_<Selection, base<IGraphic>>("Selection")
        .constructor<int, int, int, int, Surface&, bool, bool, bool, int, int>()
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
        .function("remove", &Selection::remove)
        .function("copy", &Selection::copy, allow_raw_pointers())
        ;
};
