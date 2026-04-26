#include "SelectStrategy.h"

void SelectStrategy::onPressed(int x, int y){
    hoverPreview->enable = false;

    Point to = viewport->cursorToCanvas(x, y);
    
    editor = AppContext::instance().getEditorManager()->getActiveEditor();
    
    layer = editor->getActiveFrame()->getActiveLayer();
    preview = editor->preview();
    preview->setTarget(layer);

    CanvasSettings* canvasSettings = viewport->getCanvasSettings();
    screenWidth = canvasSettings->getTilesX() * editor->getWidth();
    screenHeight = canvasSettings->getTilesY() * editor->getHeight();
    
    _heightPattern = _pattern.height*_context->nTileX;
    _widthPattern = _pattern.width*_context->nTileY;
    

    _from = to;
}
void SelectStrategy::onTracking(int x, int y){
    Point to = viewport->cursorToCanvas(x, y);
    if (to.x == _from.x && to.y == _from.y) return;
    
    
    
    _from = to;
}

void SelectStrategy::onRelease(int x, int y){
    hoverPreview->enable = true;

    preview->commit();
}

void SelectStrategy::putMirroredPixel(int x, int y, unsigned int color){
    preview->putPixel(x, y,  GraphicsEngine::blendColors(preview->getPixel(x, y), color));

    int toMirrorX = GraphicsEngine::pointMirrored(x, layer->getWidth());
    int toMirrorY = GraphicsEngine::pointMirrored(y, layer->getHeight());

    if(_context->isMirrorX){
        preview->putPixel(toMirrorX, y, GraphicsEngine::blendColors(preview->getPixel(toMirrorX, y), color));
    }            
    if(_context->isMirrorY){
        preview->putPixel(x, toMirrorY, GraphicsEngine::blendColors(preview->getPixel(x, toMirrorY), color));
    }
    if(_context->isMirrorX && _context->isMirrorY){
        preview->putPixel(toMirrorX, toMirrorY, GraphicsEngine::blendColors(preview->getPixel(toMirrorX, toMirrorY), color));
    }
}

HoverPreview* SelectStrategy::getHoverPreview(){
    return hoverPreview;
}





SelectStrategy::SelectStrategy(SymmetryContext* context){
    viewport = AppContext::instance().getViewport();
    hoverPreview = new HoverPreview();
    _context = context;
    
}
void SelectStrategy::startSelection(
    int from_start_x, int from_start_y,
    int to_start_x, int to_start_y, 
    Surface& surface, 
    bool cleanTheArea)
{
    _originalBounding = Bounding(
        Point(from_start_x,from_start_y),
        Point(to_start_x+1, to_start_y+1)
    );
    
    _originalBounding.start.x = std::min(screenWidth, std::max(0 ,_originalBounding.start.x));
    _originalBounding.start.y = std::min(screenHeight, std::max(0, _originalBounding.start.y));
    _originalBounding.end.x = std::min(screenWidth, std::max(0, _originalBounding.end.x));
    _originalBounding.end.y = std::min(screenHeight, std::max(0, _originalBounding.end.y));

    _data = new Surface(_originalBounding.getWidth(), _originalBounding.getHeight());
    
    for (int y = 0; y < _originalBounding.getHeight(); ++y) {
        Point p;
        p.y = (_originalBounding.start.y + y) %surface.getHeight();
        for (int x = 0; x < _originalBounding.getWidth(); ++x){
            p.x = (_originalBounding.start.x + x)%surface.getWidth();
            _data->putPixel(x, y, surface.getPixel(p.x , p.y));
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
SelectStrategy::~SelectStrategy(){
    free(_data);
}
void SelectStrategy::translate(float deltaX, float deltaY){
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

void SelectStrategy::rotate(float rotateRad){
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
void SelectStrategy::resize(int marker, float deltaX, float deltaY){
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

Point SelectStrategy::getCenter(){
    return Point(_dstCenterX, _dstCenterY);
}

int SelectStrategy::getResizedWidth(){
    return _resizedWidth;
}
int SelectStrategy::getResizedHeight(){
    return _resizedHeight;
}

Bounding SelectStrategy::getBounding(){
    return _originalBounding;
}

Corners SelectStrategy::getDestinationCorners(){
    return _corners;
}
float SelectStrategy::getRotateRad(){
    return _angleRad;
}
void SelectStrategy::draw(){
    if(_cleanTheArea){
        for (int y = _originalBounding.start.y; y < _originalBounding.end.y; ++y) {
            for (int x = _originalBounding.start.x; x < _originalBounding.end.x; ++x) {
                layer->putPixel((int)x%layer->getWidth(), (int)y%layer->getHeight(), 0x0);
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
    destBounding.start.y = std::max(destBounding.start.y,0);
    destBounding.start.x = std::max(destBounding.start.x,0);
    destBounding.end.y = std::min(destBounding.end.y,screenHeight);
    destBounding.end.x = std::min(destBounding.end.x,screenWidth);

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


            Point clampedPoint = {
                GraphicsEngine::clampedTilePoint(dx, layer->getWidth()),
                GraphicsEngine::clampedTilePoint(dy, layer->getHeight())
            };
            clampedPoint.x = GraphicsEngine::clampedTilePoint(dx, layer->getWidth());
            clampedPoint.y = GraphicsEngine::clampedTilePoint(dy, layer->getHeight());
            putMirroredPixel(clampedPoint.x, clampedPoint.y, color);
        }
    }
}
void SelectStrategy::remove(){
    _corners.topLeft     = Point(0, 0);
    _corners.bottomRight = Point(0, 0);
    _corners.topRight    = Point(0, 0);
    _corners.bottomLeft  = Point(0, 0);
}
Surface* SelectStrategy::copy(){
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



#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<SelectStrategy>("SelectStrategy")
        .constructor<SymmetryContext*>()
        .function("onPressed", &SelectStrategy::onPressed)
        .function("onTracking", &SelectStrategy::onTracking)
        .function("onRelease", &SelectStrategy::onRelease)
        ;
};