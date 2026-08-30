#include "SelectStrategy.h"

SelectStrategy::SelectStrategy(SelectContext* selectContext){
    _selectContext = selectContext;
    _mode = ENUM_MODE::SELECT;

    _resizeSession = new ResizeSession(_selectContext);
    _rotateSession = new RotateSession(_selectContext);
    _translateSession = new TranslateSession(_selectContext);
    _selectSession = new SelectSession(_selectContext);

    _initialized = selectContext->enabled;
}
SelectStrategy::~SelectStrategy(){
    delete _selectContext->data;
}

void SelectStrategy::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    Point world = {x,y};
    _toolRuntimeContext = toolRuntimeContext;

    _cursorContext.enable = false;
    _cursorContext.scale = 1.0f;

    _pressed = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    _from = _pressed;

    if(_initialized){
        if(_resizeSession->begin(world, _toolRuntimeContext)){
            _mode = ENUM_MODE::RESIZE;
            return;
        }
        if(_rotateSession->begin(world, _toolRuntimeContext)){
            _mode = ENUM_MODE::ROTATE;
            return;
        }       
        if(_translateSession->begin(world, _toolRuntimeContext)){
            _mode = ENUM_MODE::TRANSLATE;
            return;
        }

        done();
    }

    _mode = ENUM_MODE::SELECT;
    _selectSession->begin(world, _toolRuntimeContext);
    _cutting = true;
    _initialized = true;
}
void SelectStrategy::onTracking(int x, int y){
    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    if (to.x == _from.x && to.y == _from.y) return;
    
    if(_mode == ENUM_MODE::SELECT){
        _selectSession->update(to);
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

    if(_mode != ENUM_MODE::SELECT) {
        draw();
    }

    _from = to;
}
void SelectStrategy::onRelease(){
    _cursorContext.enable = true;

    if(_mode == ENUM_MODE::SELECT){
        _selectSession->end();

        if(_selectContext->enabled){
             draw();
        }
        _initialized = _selectContext->enabled;
    }
    _mode = ENUM_MODE::SELECTED;
}

void SelectStrategy::clear(){
    for (int y = _selectContext->srcArea.start.y; y < _selectContext->srcArea.end.y; ++y) {
        for (int x = _selectContext->srcArea.start.x; x < _selectContext->srcArea.end.x; ++x) {
            _toolRuntimeContext.drawingSession->putPixel(x, y, 0x0);
        }
    }
}
void SelectStrategy::draw(){
    _toolRuntimeContext.drawingSession->clear();

    if(_cutting) clear();
    
    PointF _dstCenter = _selectContext->selectionBox.getCenter();
    StampRasterize stamp(_dstCenter,
        {_selectContext->srcArea.getWidth(), _selectContext->srcArea.getHeight()},
        {_toolRuntimeContext.screenWidth, _toolRuntimeContext.screenHeight},
        _selectContext->transformation);


    // printf("c(%f, %f)\n",_dstCenter.x,_dstCenter.y );
    // printf("sz(%i, %i)\n",_selectContext->srcArea.getWidth(),_selectContext->srcArea.getHeight() );
        
    while(stamp.hasNext()){
        Point it = stamp.next();
        Point src = stamp.getSrcPoint();
        // printf("i(%i, %i), src(%i, %i)\n",it.x, it.y ,src.x, src.y);
        if(src.x < 0 || src.y < 0 || src.x > _selectContext->srcArea.getWidth() || src.y > _selectContext->srcArea.getHeight()) continue;

        if (!_selectContext->data->isInsideSkecth(src.x, src.y)) { continue; }
        
        unsigned int color = _selectContext->data->getPixel(src.x, src.y);
        
        if((color >> 24 & 0xFF) == 0) { continue; }
        
        _toolRuntimeContext.drawingSession->blendMirroredPixel(it.x, it.y, color);
            
    }
}

bool SelectStrategy::isInitialized(){
    return _initialized;
}
void SelectStrategy::done() {
    _toolRuntimeContext.drawingSession->commit();

    if (_selectContext->data) {
        delete _selectContext->data;
        _selectContext->data = nullptr;
    }

    _selectContext->selectionBox = SelectionBox();
    _selectContext->transformation = Transformation();
    _selectContext->srcArea = Bounding();
    _selectContext->enabled = false;

    _mode = ENUM_MODE::SELECT;
    _cutting = false;
    _initialized = false;
    _cursorContext.enable = true;
}
void SelectStrategy::abort(){
    _toolRuntimeContext.drawingSession->clear();

    _selectContext->selectionBox = SelectionBox();
    _selectContext->transformation = Transformation();
    _selectContext->srcArea = Bounding();
    _selectContext->enabled = false;

    _cutting = false;
    _mode = ENUM_MODE::SELECT;
    _initialized = false;
    _cursorContext.enable = true;
}
CursorContext* SelectStrategy::getCursorContext() {return &_cursorContext;};