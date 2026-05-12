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

void SelectStrategy::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    cursorContext->enable = false;
    _toolRuntimeContext = toolRuntimeContext;

    _pressed = _toolRuntimeContext.viewport->cursorToCanvas(x, y);
    _from = _pressed;

    if(_mode == ENUM_MODE::SELECTED){
        Point world = Point(x,y);
        if(insideCornerHitbox(world, _selectContext->corners.topLeft)){
            _mode = ENUM_MODE::RESIZE;
            _activeMarker = ENUM_MARKER::TOP_LEFT;
            return;
        }
        if(insideCornerHitbox(world, _selectContext->corners.bottomLeft)){
            _mode = ENUM_MODE::RESIZE;
            _activeMarker = ENUM_MARKER::BOTTOM_LEFT;
            return;
        }
        if(insideCornerHitbox(world, _selectContext->corners.topRight)){
            _mode = ENUM_MODE::RESIZE;
            _activeMarker = ENUM_MARKER::TOP_RIGHT;
            return;
        }
        if(insideCornerHitbox(world, _selectContext->corners.bottomRight)){
            _mode = ENUM_MODE::RESIZE;
            _activeMarker = ENUM_MARKER::BOTTOM_RIGHT;
            return;
        }


        if(insideCornerRotateHitbox(world, _selectContext->corners.topLeft)){
            _mode = ENUM_MODE::ROTATE;
            _activeMarker = ENUM_MARKER::TOP_LEFT;
            return;
        }
        if(insideCornerRotateHitbox(world, _selectContext->corners.bottomLeft)){
            _mode = ENUM_MODE::ROTATE;
            _activeMarker = ENUM_MARKER::BOTTOM_LEFT;
            return;
        }
        if(insideCornerRotateHitbox(world, _selectContext->corners.topRight)){
            _mode = ENUM_MODE::ROTATE;
            _activeMarker = ENUM_MARKER::TOP_RIGHT;
            return;
        }
        if(insideCornerRotateHitbox(world, _selectContext->corners.bottomRight)){
            _mode = ENUM_MODE::ROTATE;
            _activeMarker = ENUM_MARKER::BOTTOM_RIGHT;
            return;
        }
        
        _activeMarker = ENUM_MARKER::UNKNOW;
        if(_selectContext->corners.isInsideRotatedBounding(_pressed)){
            _mode = ENUM_MODE::TRANSLATE;
            return;
        }

        done();
    }

    _mode = ENUM_MODE::SELECT;
    
    abort();
    if(_selectContext->data != nullptr)
        free(_selectContext->data);

    _originalBounding = Bounding(
        _pressed,
        Point(_from.x+1, _from.y+1)
    );
    _selectContext->corners = Corners(*_originalBounding);

}
void SelectStrategy::onTracking(int x, int y){
    Point to = _toolRuntimeContext.viewport->cursorToCanvas(x, y);
    if (to.x == _from.x && to.y == _from.y) return;
    
    if(_mode == ENUM_MODE::SELECT){
        _originalBounding->end.x = to.x + 1;
        _originalBounding->end.y = to.y + 1;
        
        _selectContext->corners = Corners(*_originalBounding);
        // printf("(%f,%f) - (%f,%f) - (%f,%f) - (%f,%f)\n",
        //     _selectContext->corners.topLeft.x, _selectContext->corners.topLeft.y,
        //     _selectContext->corners.topRight.x, _selectContext->corners.topRight.y,
        //     _selectContext->corners.bottomRight.x, _selectContext->corners.bottomRight.y,
        //     _selectContext->corners.bottomLeft.x, _selectContext->corners.bottomLeft.y
        // );
    }

    if(_mode == ENUM_MODE::TRANSLATE){
        translate(to.x - _from.x, to.y - _from.y);
    }

    if(_mode == ENUM_MODE::RESIZE){
        resize(_activeMarker, to.x, to.y);
    }
    
    if(_mode == ENUM_MODE::ROTATE){
        float radBefore = std::atan2(_from.y - _dstCenter.y, _from.x - _dstCenter.x);
        float radAfter  = std::atan2(to.y - _dstCenter.y, to.x - _dstCenter.x);

        rotate(radAfter - radBefore);
    }

    _from = to;
}
void SelectStrategy::onRelease(){
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
        _selectContext->corners = Corners(*_originalBounding);

        start();
    }

    _mode = ENUM_MODE::SELECTED;
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

    _originalBounding->start.y = std::max(_originalBounding->start.y,0);
    _originalBounding->start.x = std::max(_originalBounding->start.x,0);
    _originalBounding->end.y = std::min(_originalBounding->end.y, _toolRuntimeContext.screenHeight);
    _originalBounding->end.x = std::min(_originalBounding->end.x, _toolRuntimeContext.screenWidth);
    
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
        _originalCorners = Corners(*_originalBounding);
        _selectContext->corners = _originalCorners;
        return;
    }
    delimit.end.x++;
    delimit.end.y++;
    
    _originalBounding = delimit;
    _origCenter = _originalBounding->getCenter();
    _originalCorners = Corners(*_originalBounding);
    _selectContext->corners = _originalCorners;
    _resized = {(float)_originalBounding->getWidth(), (float)_originalBounding->getHeight()};
    _srcCenter =  _originalBounding->getCenter();

    _selectContext->data = new Surface(_originalBounding->getWidth(), _originalBounding->getHeight());
    for (int y = 0; y < _originalBounding->getHeight(); ++y) {
        Point p;
        p.y = GraphicsEngine::clampedTilePoint(_originalBounding->start.y + y, _toolRuntimeContext.layer->getHeight());
        for (int x = 0; x < _originalBounding->getWidth(); ++x){
            p.x = GraphicsEngine::clampedTilePoint(_originalBounding->start.x + x, _toolRuntimeContext.layer->getWidth());
            
            _selectContext->data->putPixel(x, y, _toolRuntimeContext.layer->getPixel(p.x , p.y));
        }
    }

    _selectContext->transformation.setRad(0);

    _dstCenter = _selectContext->corners.getCenter();

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

    _dstCenter.x += deltaX;
    _dstCenter.y += deltaY;

    _delta.x += deltaX;
    _delta.y += deltaY;

    draw();
}

void SelectStrategy::rotate(float rotateRad){
    flagBounding = _selectContext->corners.getBounding();

    _selectContext->transformation.setRad(_selectContext->transformation.getRad() + rotateRad);

    PointF axisX = _selectContext->transformation.rotate({_resized.x * 0.5f, 0});
    PointF axisY = _selectContext->transformation.rotate({0, _resized.y * 0.5f});

    _selectContext->corners.bottomLeft.x = _dstCenter.x +
                                                    _selectContext->corners.sign[ENUM_MARKER::BOTTOM_LEFT].x * axisX.x +
                                                    _selectContext->corners.sign[ENUM_MARKER::BOTTOM_LEFT].y * axisY.x;
    _selectContext->corners.bottomLeft.y = _dstCenter.y +
                                                    _selectContext->corners.sign[ENUM_MARKER::BOTTOM_LEFT].x * axisX.y +
                                                    _selectContext->corners.sign[ENUM_MARKER::BOTTOM_LEFT].y * axisY.y;
    
    
    _selectContext->corners.topLeft.x = _dstCenter.x +
                                                    _selectContext->corners.sign[ENUM_MARKER::TOP_LEFT].x * axisX.x +
                                                    _selectContext->corners.sign[ENUM_MARKER::TOP_LEFT].y * axisY.x;
    _selectContext->corners.topLeft.y = _dstCenter.y +
                                                    _selectContext->corners.sign[ENUM_MARKER::TOP_LEFT].x * axisX.y +
                                                    _selectContext->corners.sign[ENUM_MARKER::TOP_LEFT].y * axisY.y;
    
    
    _selectContext->corners.bottomRight.x = _dstCenter.x +
                                                    _selectContext->corners.sign[ENUM_MARKER::BOTTOM_RIGHT].x * axisX.x +
                                                    _selectContext->corners.sign[ENUM_MARKER::BOTTOM_RIGHT].y * axisY.x;
    _selectContext->corners.bottomRight.y = _dstCenter.y +
                                                    _selectContext->corners.sign[ENUM_MARKER::BOTTOM_RIGHT].x * axisX.y +
                                                    _selectContext->corners.sign[ENUM_MARKER::BOTTOM_RIGHT].y * axisY.y;
    
    
    _selectContext->corners.topRight.x = _dstCenter.x +
                                                    _selectContext->corners.sign[ENUM_MARKER::TOP_RIGHT].x * axisX.x +
                                                    _selectContext->corners.sign[ENUM_MARKER::TOP_RIGHT].y * axisY.x;
    _selectContext->corners.topRight.y = _dstCenter.y +
                                                    _selectContext->corners.sign[ENUM_MARKER::TOP_RIGHT].x * axisX.y +
                                                    _selectContext->corners.sign[ENUM_MARKER::TOP_RIGHT].y * axisY.y;
    
    draw();
}
void SelectStrategy::resize(int marker, float deltaX, float deltaY){
    flagBounding = _selectContext->corners.getBounding();

    PointF pixel = PointF(deltaX, deltaY);

    PointF* pivot = nullptr;
    PointF* dragged = nullptr;
    PointF* cornerH = nullptr;
    PointF* cornerW = nullptr;

    switch ((ENUM_MARKER)marker) {
        case ENUM_MARKER::TOP_LEFT:
            pivot   = &_selectContext->corners.bottomRight;
            dragged = &_selectContext->corners.topLeft;
            cornerH = &_selectContext->corners.topRight;
            cornerW = &_selectContext->corners.bottomLeft;
            break;

        case ENUM_MARKER::TOP_RIGHT:
            pivot   = &_selectContext->corners.bottomLeft;
            dragged = &_selectContext->corners.topRight;
            cornerH = &_selectContext->corners.topLeft;
            cornerW = &_selectContext->corners.bottomRight;
            break;

        case ENUM_MARKER::BOTTOM_RIGHT:
            pivot   = &_selectContext->corners.topLeft;
            dragged = &_selectContext->corners.bottomRight;
            cornerH = &_selectContext->corners.bottomLeft;
            cornerW = &_selectContext->corners.topRight;
            break;

        case ENUM_MARKER::BOTTOM_LEFT:
            pivot   = &_selectContext->corners.topRight;
            dragged = &_selectContext->corners.bottomLeft;
            cornerH = &_selectContext->corners.bottomRight;
            cornerW = &_selectContext->corners.topLeft;
            break;
    default:
        throw std::runtime_error("Corner inválido");
    }

    PointF unrotate = _selectContext->transformation.unrotate(_selectContext->transformation.distance(pixel, *pivot));

    _resized.x = _selectContext->corners.sign[marker].x *  unrotate.x;
    _resized.y = _selectContext->corners.sign[marker].y * unrotate.y;

    *cornerW = _selectContext->transformation.fromWidth(_selectContext->corners.sign[marker].x * _resized.x, *pivot);
    *cornerH = _selectContext->transformation.fromHeight(_selectContext->corners.sign[marker].y * _resized.y, *pivot);
    (*dragged) = {
        cornerH->x + cornerW->x - pivot->x,
        cornerH->y + cornerW->y - pivot->y
    };

    _scale.x =  _resized.x / (float)_originalBounding->getWidth();
    _scale.y =  _resized.y / (float)_originalBounding->getHeight();
    _dstCenter = _selectContext->corners.getCenter();
    
    draw();
}

void SelectStrategy::draw(){
    flagBounding.start.y = std::max(flagBounding.start.y,0);
    flagBounding.start.x = std::max(flagBounding.start.x,0);
    flagBounding.end.y = std::min(flagBounding.end.y, _toolRuntimeContext.screenHeight);
    flagBounding.end.x = std::min(flagBounding.end.x, _toolRuntimeContext.screenWidth);
    
    for (int y = flagBounding.start.y; y < flagBounding.end.y; ++y) {
        Point p;
        p.y = GraphicsEngine::clampedTilePoint(y, _toolRuntimeContext.layer->getHeight());
        for (int x = flagBounding.start.x; x < flagBounding.end.x; ++x) {
            p.x = GraphicsEngine::clampedTilePoint(x, _toolRuntimeContext.layer->getWidth());

            if((p.x >= _originalBounding->start.x && p.x < _originalBounding->end.x) &&
                (p.y >= _originalBounding->start.y && p.y < _originalBounding->end.y)){
                _toolRuntimeContext.preview->putPixel(p.x, p.y, 0x0);
                continue;
            }
            _toolRuntimeContext.preview->uncommit(p.x, p.y);
        }
    }


    Bounding destBounding = _selectContext->corners.getBounding();
    destBounding.start.y = std::max(destBounding.start.y,0);
    destBounding.start.x = std::max(destBounding.start.x,0);
    destBounding.end.y = std::min(destBounding.end.y, _toolRuntimeContext.screenHeight);
    destBounding.end.x = std::min(destBounding.end.x, _toolRuntimeContext.screenWidth);


    for (int dy = destBounding.start.y; dy < destBounding.end.y; dy++){
        PointF src;
        float _dy = dy  + 0.5f - _dstCenter.y;

        for (int dx = destBounding.start.x; dx < destBounding.end.x; dx++) {
            float _dx = dx  + 0.5f - _dstCenter.x;

            src = _selectContext->transformation.unrotate({_dx, _dy});
            src.x = std::floor(src.x / _scale.x + _srcCenter.x - _originalBounding->start.x);
            src.y = std::floor(src.y / _scale.y + _srcCenter.y - _originalBounding->start.y);

            if (!_selectContext->data->isInsideSkecth(src.x, src.y)) {
                continue;
            }
            unsigned int color = _selectContext->data->getPixel(src.x, src.y);

            if((color >> 24 & 0xFF) == 0) { continue; }

            Point clampedPoint = {
                GraphicsEngine::clampedTilePoint(dx, _toolRuntimeContext.layer->getWidth()),
                GraphicsEngine::clampedTilePoint(dy, _toolRuntimeContext.layer->getHeight())
            };
            putMirroredPixel(clampedPoint.x, clampedPoint.y, color);
        }
    }
}
Surface* SelectStrategy::copy(){
    Bounding destBounding = _selectContext->corners.getBounding();
    Surface* surface = new Surface(destBounding.getWidth(), destBounding.getHeight());

    for (int dy = destBounding.start.y; dy < destBounding.end.y; dy++){
        PointF src;
        float _dy = dy  + 0.5f- _dstCenter.y;

        for (int dx = destBounding.start.x; dx < destBounding.end.x; dx++) {
            float _dx = dx  + 0.5f - _dstCenter.x;

            src = _selectContext->transformation.unrotate({_dx, _dy});
            src.x = std::floor(src.x / _scale.x + _srcCenter.x - _originalBounding->start.x);
            src.y = std::floor(src.y / _scale.y + _srcCenter.y - _originalBounding->start.y);

            if (!_selectContext->data->isInsideSkecth(src.x, src.y)) {
                continue;
            }

            unsigned int color = _selectContext->data->getPixel(src.x, src.y);

            if((color & 0xFF) == 0) { continue; }
            surface->putPixel(dx-destBounding.start.x , dy-destBounding.start.y, color);
        }
    }

    return surface;
}

bool SelectStrategy::insideCornerHitbox(Point point, PointF cornerPosition){
    CanvasSettings* settings = _toolRuntimeContext.viewport->getCanvasSettings();
    PointF cornerWorldPosisition = _toolRuntimeContext.viewport->canvasToWorld(cornerPosition.x, cornerPosition.y);

    return (point.x >= cornerWorldPosisition.x - sizeHitbox*0.5f* settings->getScale() && point.x < cornerWorldPosisition.x + sizeHitbox*0.5* settings->getScale()) &&
        (point.y >= cornerWorldPosisition.y - sizeHitbox*0.5f* settings->getScale() && point.y < cornerWorldPosisition.y + sizeHitbox*0.5* settings->getScale());
}

bool SelectStrategy::insideCornerRotateHitbox(Point point, PointF cornerPosition){
    CanvasSettings* settings = _toolRuntimeContext.viewport->getCanvasSettings();
    
    PointF cornerPositionRotate = _selectContext->cornerRotate(_toolRuntimeContext.viewport, cornerPosition);

    return ((float)point.x >= cornerPositionRotate.x - sizeHitbox*0.5f* settings->getScale() && (float)point.x < cornerPositionRotate.x + sizeHitbox*0.5f* settings->getScale()) &&
        ((float)point.y >= cornerPositionRotate.y - sizeHitbox*0.5f* settings->getScale() && (float)point.y < cornerPositionRotate.y + sizeHitbox*0.5f* settings->getScale());
}


void SelectStrategy::done() {
    _toolRuntimeContext.preview->commit();

    _originalBounding.reset();
    _selectContext->corners = Corners();
    _selectContext->cornersRotate = Corners();

    _scale = {1.0f,1.0f};
    _selectContext->transformation.setRad(0.0f);

    _origCenter = _originalBounding->getCenter();
    _dstCenter = _selectContext->corners.getCenter();
    _resized = {(float)_originalBounding->getWidth(), (float)_originalBounding->getHeight()};

    _cutting = false;
}
void SelectStrategy::abort(){
    _toolRuntimeContext.preview->clear();

    _originalBounding.reset();
    _selectContext->corners = Corners();
    _selectContext->cornersRotate = Corners();

    _scale = {1.0f,1.0f};
    _selectContext->transformation.setRad(0.0f);

    _origCenter = _originalBounding->getCenter();
    _dstCenter = _selectContext->corners.getCenter();
    _resized = {(float)_originalBounding->getWidth(), (float)_originalBounding->getHeight()};

    _cutting = false;
}