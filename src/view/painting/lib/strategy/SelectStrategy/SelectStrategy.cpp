#include "SelectStrategy.h"

SelectStrategy::SelectStrategy(SelectContext* selectContext, SymmetryContext* symmetryContext){
    cursorContext = new CursorContext();
    _symmetryContext = symmetryContext;
    _selectContext = selectContext;
    _mode = ENUM_MODE::SELECT;
}
SelectStrategy::~SelectStrategy(){
    free(_selectContext->data);
}

void SelectStrategy::onPressed(int x, int y, ToolRuntimeContext toolRuntimeContext){
    cursorContext->enable = false;
    _toolRuntimeContext = toolRuntimeContext;

    Point to = _toolRuntimeContext.viewport->cursorToCanvas(x, y);
    _from = to;

    // if(!_originalBounding.has_value()){
    if(_mode == ENUM_MODE::SELECT || !_selectContext->corners.isInsideRotatedBounding(to)){
        _mode = ENUM_MODE::SELECT;
        if(to.x > _toolRuntimeContext.screenWidth || to.y > _toolRuntimeContext.screenHeight) return;
        
        _toolRuntimeContext.preview->clear();
        // if(_selectContext->data != nullptr)
        //     free(_selectContext->data);

        _originalBounding = Bounding(
            to,
            Point(_from.x+1, _from.y+1)
        );
        _selectContext->corners = Corners(*_originalBounding);

        return;
    }

    if(_mode != ENUM_MODE::SELECTED) return;

    if(_selectContext->corners.isInsideRotatedBounding(to)){
        _mode = ENUM_MODE::TRANSLATE;
        return;
    }

    if(insideCornerHitbox(to, _selectContext->corners.topLeft)){
        _mode = ENUM_MODE::RESIZE_TOP_LEFT;
        return;
    }
    if(insideCornerHitbox(to, _selectContext->corners.bottomLeft)){
        _mode = ENUM_MODE::RESIZE_BOTTOM_LEFT;
        return;
    }
    if(insideCornerHitbox(to, _selectContext->corners.topRight)){
        _mode = ENUM_MODE::RESIZE_TOP_RIGHT;
        return;
    }
    if(insideCornerHitbox(to, _selectContext->corners.bottomRight)){
        _mode = ENUM_MODE::RESIZE_BOTTOM_RIGHT;
        return;
    }
}
void SelectStrategy::onTracking(int x, int y){
    Point to = _toolRuntimeContext.viewport->cursorToCanvas(x, y);
    if (to.x == _from.x && to.y == _from.y) return;

    if(_mode == ENUM_MODE::SELECT){
        _originalBounding->end.x = to.x + 1;
        _originalBounding->end.y = to.y + 1;
        
        _selectContext->corners = Corners(*_originalBounding);
    }

    if(_mode == ENUM_MODE::TRANSLATE){
        translate(to.x - _from.x, to.y - _from.y);
    }

    if(_mode == ENUM_MODE::RESIZE_TOP_LEFT){

    }
    if(_mode == ENUM_MODE::RESIZE_TOP_RIGHT){

    }
    if(_mode == ENUM_MODE::RESIZE_BOTTOM_LEFT){

    }
    if(_mode == ENUM_MODE::RESIZE_BOTTOM_RIGHT){

    }
    
    if(_mode == ENUM_MODE::ROTATE_TOP_LEFT){
    }
    
    if(_mode == ENUM_MODE::ROTATE_TOP_RIGHT){
    }
    
    if(_mode == ENUM_MODE::ROTATE_BOTTOM_LEFT){
    }
    
    if(_mode == ENUM_MODE::ROTATE_BOTTOM_RIGHT){
    }

    _from = to;
}
void SelectStrategy::onRelease(int x, int y){
    cursorContext->enable = true;

    if(_mode == ENUM_MODE::SELECT){
        if(_originalBounding->start.x >= _originalBounding->end.x){
            std::swap(_originalBounding->start.x, _originalBounding->end.x);
            _originalBounding->start.x -= 1;
            _originalBounding->end.x += 1;
        }
        if(_originalBounding->start.y >= _originalBounding->end.y){
            std::swap(_originalBounding->start.y, _originalBounding->end.y);
            _originalBounding->start.y -= 1;
            _originalBounding->end.y += 1;
        }

        start();
    }

    if(_mode == ENUM_MODE::TRANSLATE){
    }
    
    if(_mode == ENUM_MODE::RESIZE_TOP_LEFT){

    }
    if(_mode == ENUM_MODE::RESIZE_TOP_RIGHT){

    }
    if(_mode == ENUM_MODE::RESIZE_BOTTOM_LEFT){

    }
    if(_mode == ENUM_MODE::RESIZE_BOTTOM_RIGHT){

    }
    
    if(_mode == ENUM_MODE::ROTATE_TOP_LEFT){
    }
    
    if(_mode == ENUM_MODE::ROTATE_TOP_RIGHT){
    }
    
    if(_mode == ENUM_MODE::ROTATE_BOTTOM_LEFT){
    }
    
    if(_mode == ENUM_MODE::ROTATE_BOTTOM_RIGHT){
    }
    
    _mode = ENUM_MODE::SELECTED;
    // translate(0.0f, -3.0f);
    // draw();
    // rotate(45.0f*M_PI / 180.0f);
    // resize(0, -20.0f, -10.0f);

}

void SelectStrategy::putMirroredPixel(int x, int y, unsigned int color){
    _toolRuntimeContext.preview->putPixel(x, y,  GraphicsEngine::blendColors(_toolRuntimeContext.preview->getPixel(x, y), color));

    int toMirrorX = _symmetryContext->pointMirrored(x, _toolRuntimeContext.layer->getWidth());
    int toMirrorY = _symmetryContext->pointMirrored(y, _toolRuntimeContext.layer->getHeight());

    if(_symmetryContext->isMirrorX){
        _toolRuntimeContext.preview->putPixel(toMirrorX, y, GraphicsEngine::blendColors(_toolRuntimeContext.preview->getPixel(toMirrorX, y), color));
    }
    if(_symmetryContext->isMirrorY){
        _toolRuntimeContext.preview->putPixel(x, toMirrorY, GraphicsEngine::blendColors(_toolRuntimeContext.preview->getPixel(x, toMirrorY), color));
    }
    if(_symmetryContext->isMirrorX && _symmetryContext->isMirrorY){
        _toolRuntimeContext.preview->putPixel(toMirrorX, toMirrorY, GraphicsEngine::blendColors(_toolRuntimeContext.preview->getPixel(toMirrorX, toMirrorY), color));
    }
}

CursorContext* SelectStrategy::getCursorContext(){
    return cursorContext;
}


void SelectStrategy::start()
{
    Bounding delimit;
    delimit.start.x = INT_MAX;
    delimit.start.y = INT_MAX;
    delimit.end.x = INT_MIN;
    delimit.end.y = INT_MIN;

    for (int y = _originalBounding->start.y; y < _originalBounding->end.y; ++y) {
        Point p;
        p.y = GraphicsEngine::clampedTilePoint(y, _toolRuntimeContext.layer->getHeight());
        for (int x = _originalBounding->start.x; x < _originalBounding->end.x; ++x){
            p.x = GraphicsEngine::clampedTilePoint(x, _toolRuntimeContext.layer->getWidth());
            if((_toolRuntimeContext.layer->getPixel(p.x, p.y) >> 24 & 0xFF) == 0) continue;

            if(x < delimit.start.x)  delimit.start.x = x;
            if(y < delimit.start.y)  delimit.start.y = y;
            if(x > delimit.end.x)    delimit.end.x = x;
            if(y > delimit.end.y)    delimit.end.y = y;
        }
    }
    if(
        delimit.start.x == INT_MAX || delimit.start.y == INT_MAX ||
        delimit.end.x == INT_MIN || delimit.end.y == INT_MIN
    ){
        _originalBounding = Bounding();
        _selectContext->corners = Corners(*_originalBounding);
        return;
    }
    delimit.end.x++;
    delimit.end.y++;
    
    _originalBounding = delimit;
    _selectContext->corners = Corners(*_originalBounding);


    _selectContext->data = new Surface(_originalBounding->getWidth(), _originalBounding->getHeight());
    for (int y = 0; y < _originalBounding->getHeight(); ++y) {
        Point p;
        p.y = GraphicsEngine::clampedTilePoint(_originalBounding->start.y + y, _toolRuntimeContext.layer->getHeight());
        for (int x = 0; x < _originalBounding->getWidth(); ++x){
            p.x = GraphicsEngine::clampedTilePoint(_originalBounding->start.x + x, _toolRuntimeContext.layer->getWidth());
            
            _selectContext->data->putPixel(x, y, _toolRuntimeContext.layer->getPixel(p.x , p.y));
        }
    }


    _origCenterX = (_originalBounding->start.x + _originalBounding->end.x) * 0.5f;
    _origCenterY = (_originalBounding->start.y + _originalBounding->end.y) * 0.5f;

    _angleRad = 0;
    _cosA = std::cos(_angleRad);
    _sinA = std::sin(_angleRad);

    _dstCenterX = _origCenterX;
    _dstCenterY = _origCenterY;

    _resizedWidth = _originalBounding->getWidth();
    _resizedHeight = _originalBounding->getHeight() ;

    _mode = ENUM_MODE::SELECTED;
    draw();
}
void SelectStrategy::translate(float deltaX, float deltaY){
    flagBounding = _selectContext->corners.getBounding();

    _selectContext->corners.topLeft.x += deltaX;
    _selectContext->corners.topLeft.y += deltaY;

    _selectContext->corners.bottomRight.x += deltaX;
    _selectContext->corners.bottomRight.y += deltaY;

    _selectContext->corners.topRight.x += deltaX;
    _selectContext->corners.topRight.y += deltaY;

    _selectContext->corners.bottomLeft.x += deltaX;
    _selectContext->corners.bottomLeft.y += deltaY;

    _dstCenterX += deltaX;
    _dstCenterY += deltaY;

    draw();
}

void SelectStrategy::rotate(float rotateRad){
    flagBounding = _selectContext->corners.getBounding();

    _angleRad += rotateRad;
    _cosA = std::cos(_angleRad);
    _sinA = std::sin(_angleRad);

    float halfWidth =  _resizedWidth * 0.5f;
    float halfHeight = _resizedHeight * 0.5f;

    float widthX =  _cosA * halfWidth;
    float widthY =  _sinA * halfWidth;

    float heightX = -_sinA * halfHeight;
    float heightY =  _cosA * halfHeight;
    _selectContext->corners.topLeft.x = std::round(_dstCenterX - widthX - heightX);
    _selectContext->corners.topLeft.y = std::round(_dstCenterY - widthY - heightY);

    _selectContext->corners.topRight.x = std::round(_dstCenterX + widthX - heightX);
    _selectContext->corners.topRight.y = std::round(_dstCenterY + widthY - heightY);

    _selectContext->corners.bottomRight.x = std::round(_dstCenterX + widthX + heightX);
    _selectContext->corners.bottomRight.y = std::round(_dstCenterY + widthY + heightY);

    _selectContext->corners.bottomLeft.x = std::round(_dstCenterX - widthX + heightX);
    _selectContext->corners.bottomLeft.y = std::round(_dstCenterY - widthY + heightY);

    printf("%i, %i - %i, %i - %i, %i - %i, %i\n", _selectContext->corners.topLeft.x,_selectContext->corners.topLeft.y
    , _selectContext->corners.topRight.x,_selectContext->corners.topRight.y
    , _selectContext->corners.bottomRight.x,_selectContext->corners.bottomRight.y
    , _selectContext->corners.bottomLeft.x,_selectContext->corners.bottomLeft.y);

    draw();
}
void SelectStrategy::resize(int marker, float deltaX, float deltaY){
    flagBounding = _selectContext->corners.getBounding();

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
            pivot   = _selectContext->corners.bottomRight;
            dragged = &_selectContext->corners.topLeft;
            cornerH = &_selectContext->corners.topRight;
            cornerW = &_selectContext->corners.bottomLeft;
            signW = -1; signH = -1;
            break;

        case ENUM_MARKER::TOP_RIGHT:
            pivot   = _selectContext->corners.bottomLeft;
            dragged = &_selectContext->corners.topRight;
            cornerH = &_selectContext->corners.topLeft;
            cornerW = &_selectContext->corners.bottomRight;
            signW =  1; signH = -1;
            break;

        case ENUM_MARKER::BOTTOM_RIGHT:
            pivot   = _selectContext->corners.topLeft;
            dragged = &_selectContext->corners.bottomRight;
            cornerH = &_selectContext->corners.bottomLeft;
            cornerW = &_selectContext->corners.topRight;
            signW =  1; signH =  1;
            break;

        case ENUM_MARKER::BOTTOM_LEFT:
            pivot   = _selectContext->corners.topRight;
            dragged = &_selectContext->corners.bottomLeft;
            cornerH = &_selectContext->corners.bottomRight;
            cornerW = &_selectContext->corners.topLeft;
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

    _scaleX =  _resizedWidth / (float)_originalBounding->getWidth();
    _scaleY =  _resizedHeight / (float)_originalBounding->getHeight();

    _dstCenterX = (_selectContext->corners.topLeft.x + _selectContext->corners.topRight.x + _selectContext->corners.bottomLeft.x + _selectContext->corners.bottomRight.x) * 0.25f;
    _dstCenterY = (_selectContext->corners.topLeft.y + _selectContext->corners.topRight.y + _selectContext->corners.bottomLeft.y + _selectContext->corners.bottomRight.y) * 0.25f;

    draw();
}

optional<Bounding> SelectStrategy::getBounding(){
    return _originalBounding;
}

Corners SelectStrategy::getDestinationCorners(){
    return _selectContext->corners;
}
void SelectStrategy::draw(){
    flagBounding.start.y = std::max(flagBounding.start.y,0);
    flagBounding.start.x = std::max(flagBounding.start.x,0);
    flagBounding.end.y = std::min(flagBounding.end.y, _toolRuntimeContext.screenHeight);
    flagBounding.end.x = std::min(flagBounding.end.x, _toolRuntimeContext.screenWidth);

    for (int y = flagBounding.start.y; y < flagBounding.end.y; ++y) {
        for (int x = flagBounding.start.x; x < flagBounding.end.x; ++x) {
            Point p = {
                GraphicsEngine::clampedTilePoint(x, _toolRuntimeContext.layer->getWidth()),
                GraphicsEngine::clampedTilePoint(y, _toolRuntimeContext.layer->getHeight())
            };
            if((p.x >= _originalBounding->start.x && p.x < _originalBounding->end.x) &&
                (p.y >= _originalBounding->start.y && p.y < _originalBounding->end.y)){
                _toolRuntimeContext.preview->putPixel(p.x, p.y, 0x0);
                continue;
            }
            _toolRuntimeContext.preview->uncommit(p.x, p.y);
        }
    }

    float hw = _resizedWidth * 0.5f;
    float hh = _resizedHeight * 0.5f;

    if (_resizedWidth < 0) hw = -hw;
    if (_resizedHeight < 0) hh = -hh;

    const int offsetX = -_originalBounding->start.x + std::min(_originalBounding->start.x, 0);
    const int offsetY = -_originalBounding->start.y + std::min(_originalBounding->start.y, 0);

    Bounding destBounding = _selectContext->corners.getBounding();
    destBounding.start.y = std::max(destBounding.start.y,0);
    destBounding.start.x = std::max(destBounding.start.x,0);
    destBounding.end.y = std::min(destBounding.end.y, _toolRuntimeContext.screenHeight);
    destBounding.end.x = std::min(destBounding.end.x, _toolRuntimeContext.screenWidth);
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

            unsigned int color = _selectContext->data->getPixel(src.x, src.y);

            if((color >> 24 & 0xFF) == 0) { continue; }

            Point clampedPoint = {
                GraphicsEngine::clampedTilePoint(dx, _toolRuntimeContext.layer->getWidth()),
                GraphicsEngine::clampedTilePoint(dy, _toolRuntimeContext.layer->getHeight())
            };
            // putMirroredPixel(clampedPoint.x, clampedPoint.y, color);
            _toolRuntimeContext.preview->putPixel(clampedPoint.x, clampedPoint.y,  GraphicsEngine::blendColors(_toolRuntimeContext.layer->getPixel(clampedPoint.x, clampedPoint.y), color));
        }
    }
}
void SelectStrategy::remove(){
    _selectContext->corners.topLeft     = Point(0, 0);
    _selectContext->corners.bottomRight = Point(0, 0);
    _selectContext->corners.topRight    = Point(0, 0);
    _selectContext->corners.bottomLeft  = Point(0, 0);
}
Surface* SelectStrategy::copy(){
    float hw = _resizedWidth * 0.5f;
    float hh = _resizedHeight * 0.5f;

    if (_resizedWidth < 0) hw = -hw;
    if (_resizedHeight < 0) hh = -hh;

    Bounding destBounding = _selectContext->corners.getBounding();
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

            src.x = std::floor(localX / _scaleX + _origCenterX - _originalBounding->start.x);
            src.y = std::floor(localY / _scaleY + _origCenterY - _originalBounding->start.y);

            unsigned int color = _selectContext->data->getPixel(src.x, src.y);

            if((color & 0xFF) == 0) { continue; }
            surface->putPixel(dx-destBounding.start.x , dy-destBounding.start.y, color);
        }
    }

    return surface;
}

bool SelectStrategy::insideCornerHitbox(Point point, Point cornerPosition){
    return (point.x >= cornerPosition.x - sizeHitbox*0.5 && point.x < cornerPosition.x + sizeHitbox*0.5) &&
        (point.y >= cornerPosition.y - sizeHitbox*0.5 && point.y < cornerPosition.y + sizeHitbox*0.5);
}