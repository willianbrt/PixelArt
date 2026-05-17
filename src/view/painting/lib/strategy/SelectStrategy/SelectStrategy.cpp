#include "SelectStrategy.h"

SelectStrategy::SelectStrategy(SelectContext* selectContext, SymmetryContext* symmetryContext){
    cursorContext = new CursorContext();
    _symmetryContext = symmetryContext;
    _selectContext = selectContext;
    _mode = ENUM_MODE::SELECT;

    _resizeSession = new ResizeSession(_selectContext);
    _rotateSession = new RotateSession(_selectContext);
    _translateSession = new TranslateSession(_selectContext);
}
SelectStrategy::~SelectStrategy(){
    delete _selectContext->data;
}

void SelectStrategy::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    Point world = {x,y};
    cursorContext->enable = false;
    _toolRuntimeContext = toolRuntimeContext;

    _pressed = _toolRuntimeContext.viewport->cursorToCanvas(x, y);
    _from = _pressed;

    if(_mode == ENUM_MODE::SELECTED){
        
        if(_resizeSession->begin(world, _toolRuntimeContext.viewport)){
            _mode = ENUM_MODE::RESIZE;
            return;
        }
        if(_rotateSession->begin(world, _toolRuntimeContext.viewport)){
            _mode = ENUM_MODE::ROTATE;
            return;
        }       
        if(_translateSession->begin(world, _toolRuntimeContext.viewport)){
            _mode = ENUM_MODE::TRANSLATE;
            return;
        }

        done();
    }

    _mode = ENUM_MODE::SELECT;
    
    _selectContext->srcArea = Bounding(
        _pressed,
        Point(_from.x+1, _from.y+1)
    );
    _selectContext->selectionBox = SelectionBox(_selectContext->srcArea);

}
void SelectStrategy::onTracking(int x, int y){
    Point to = _toolRuntimeContext.viewport->cursorToCanvas(x, y);
    if (to.x == _from.x && to.y == _from.y) return;

    flagBounding = _selectContext->selectionBox.getBounding();
    
    if(_mode == ENUM_MODE::SELECT){
        _selectContext->srcArea.end.x = to.x + 1;
        _selectContext->srcArea.end.y = to.y + 1;
        
        _selectContext->selectionBox = SelectionBox(_selectContext->srcArea);
    }

    if(_mode == ENUM_MODE::TRANSLATE){
        _translateSession->update(to);
    }

    if(_mode == ENUM_MODE::RESIZE){
        _resizeSession->update(to);
    }
    
    if(_mode == ENUM_MODE::ROTATE){
        _rotateSession->update(to);
    }

    if(_mode != ENUM_MODE::SELECT)
        draw();

    _from = to;
}
void SelectStrategy::onRelease(){
    cursorContext->enable = true;

    if(_mode == ENUM_MODE::SELECT){
        if(_selectContext->srcArea.start.x >= _selectContext->srcArea.end.x){
            std::swap(_selectContext->srcArea.start.x, _selectContext->srcArea.end.x);
            _selectContext->srcArea.start.x -= 1;
            _selectContext->srcArea.end.x += 1;
        }
        if(_selectContext->srcArea.start.y >= _selectContext->srcArea.end.y){
            std::swap(_selectContext->srcArea.start.y, _selectContext->srcArea.end.y);
            _selectContext->srcArea.start.y -= 1;
            _selectContext->srcArea.end.y += 1;
        }
        _selectContext->selectionBox = SelectionBox(_selectContext->srcArea);
        
        start();
        return;
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

    _toolRuntimeContext.clampBounding(_selectContext->srcArea);
    for (int y = _selectContext->srcArea.start.y; y < _selectContext->srcArea.end.y; ++y) {
        Point p;
        p.y = GraphicsEngine::clampedTilePoint(y, _toolRuntimeContext.layer->getHeight());
        for (int x = _selectContext->srcArea.start.x; x < _selectContext->srcArea.end.x; ++x){
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
        abort();
        return;
    }
    delimit.end.x++;
    delimit.end.y++;
    
    _selectContext->srcArea = delimit;
    _selectContext->selectionBox = SelectionBox(_selectContext->srcArea);
    
    _selectContext->data = new Surface(_selectContext->srcArea.getWidth(), _selectContext->srcArea.getHeight());
    for (int y = 0; y < _selectContext->srcArea.getHeight(); ++y) {
        Point p;
        p.y = GraphicsEngine::clampedTilePoint(_selectContext->srcArea.start.y + y, _toolRuntimeContext.layer->getHeight());
        for (int x = 0; x < _selectContext->srcArea.getWidth(); ++x){
            p.x = GraphicsEngine::clampedTilePoint(_selectContext->srcArea.start.x + x, _toolRuntimeContext.layer->getWidth());
            
            _selectContext->data->putPixel(x, y, _toolRuntimeContext.layer->getPixel(p.x , p.y));
        }
    }

    _mode = ENUM_MODE::SELECTED;
    draw();
}
void SelectStrategy::draw(){
    _toolRuntimeContext.clampBounding(flagBounding);
    for (int y = flagBounding.start.y; y < flagBounding.end.y; ++y) {
        Point p;
        p.y = GraphicsEngine::clampedTilePoint(y, _toolRuntimeContext.layer->getHeight());
        for (int x = flagBounding.start.x; x < flagBounding.end.x; ++x) {
            p.x = GraphicsEngine::clampedTilePoint(x, _toolRuntimeContext.layer->getWidth());

            if((p.x >= _selectContext->srcArea.start.x && p.x < _selectContext->srcArea.end.x) &&
                (p.y >= _selectContext->srcArea.start.y && p.y < _selectContext->srcArea.end.y)){
                _toolRuntimeContext.preview->putPixel(p.x, p.y, 0x0);
                continue;
            }
            _toolRuntimeContext.preview->uncommit(p.x, p.y);
        }
    }


    Bounding destBounding = _selectContext->selectionBox.getBounding();
    _toolRuntimeContext.clampBounding(destBounding);

    const PointF* scale = _selectContext->transformation.getScale();
    PointF _dstCenter = _selectContext->selectionBox.getCenter();
    PointF _srcCenter = _selectContext->srcArea.getCenter();
    float halfW = (_selectContext->srcArea.getWidth()) * 0.5f;
    float halfH = (_selectContext->srcArea.getHeight()) * 0.5f;
        
    for (int dy = destBounding.start.y; dy < destBounding.end.y; dy++){
        for (int dx = destBounding.start.x; dx < destBounding.end.x; dx++) {
            PointF src = _selectContext->transformation.unrotate({dx  + 0.5f - _dstCenter.x, dy + 0.5f - _dstCenter.y});
            src.x = std::floor(src.x / scale->x + halfW);
            src.y = std::floor(src.y / scale->y + halfH);


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
    Bounding destBounding = _selectContext->selectionBox.getBounding();
    Surface* surface = new Surface(destBounding.getWidth(), destBounding.getHeight());

    const PointF* scale = _selectContext->transformation.getScale();
    PointF _dstCenter = _selectContext->selectionBox.getCenter();
    float halfW = (_selectContext->srcArea.getWidth()) * 0.5f;
    float halfH = (_selectContext->srcArea.getHeight()) * 0.5f;

    for (int dy = destBounding.start.y; dy < destBounding.end.y; dy++){
        for (int dx = destBounding.start.x; dx < destBounding.end.x; dx++) {
            PointF src = _selectContext->transformation.unrotate({dx  + 0.5f - _dstCenter.x, dy + 0.5f - _dstCenter.y});
            src.x = std::floor(src.x  / scale->x + halfH);
            src.y = std::floor(src.y  / scale->y + halfW);

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


void SelectStrategy::done() {
    _toolRuntimeContext.preview->commit();
    abort();
}
void SelectStrategy::abort(){
    _toolRuntimeContext.preview->clear();

    if (_selectContext->data) {
        delete _selectContext->data;
        _selectContext->data = nullptr;
    }

    _selectContext->selectionBox = SelectionBox();
    _selectContext->transformation = Transformation();
    _selectContext->srcArea = Bounding();

    _cutting = false;
    _mode = ENUM_MODE::SELECT;
}