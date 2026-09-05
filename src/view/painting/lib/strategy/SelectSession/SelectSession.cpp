#include "SelectSession.h"

SelectSession::SelectSession(SelectContext* selection)
: 
_selectionContext(selection)
{
}
bool SelectSession::begin(Point point, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
    
    _startPoint = _toolRuntimeContext.canvasSettings->cursorToCanvas(point.x, point.y);
    
    _selectionContext->selectionBox = SelectionBox();
    _selectionContext->transformation = Transformation();
    _selectionContext->srcArea = Bounding();

    _selectionContext->srcArea = Bounding(_startPoint, _startPoint);
    _selectionContext->selectionBox = SelectionBox(_selectionContext->srcArea);
     
    _selectionContext->enabled = true;
    
    return true;
}
void SelectSession::update(const Point& mouse){
    _selectionContext->srcArea.end.x = mouse.x;
    _selectionContext->srcArea.end.y = mouse.y;
    
    _selectionContext->selectionBox = SelectionBox(_selectionContext->srcArea);
    
    _startPoint = mouse;
}


void SelectSession::end(){
    if(_selectionContext->srcArea.start.x >= _selectionContext->srcArea.end.x){
        std::swap(_selectionContext->srcArea.start.x, _selectionContext->srcArea.end.x);
    }
    if(_selectionContext->srcArea.start.y >= _selectionContext->srcArea.end.y){
        std::swap(_selectionContext->srcArea.start.y, _selectionContext->srcArea.end.y);
    }
    _selectionContext->selectionBox = SelectionBox(_selectionContext->srcArea);
    
    shrinkToTheDrawing();
    initSelectData();
}

void SelectSession::shrinkToTheDrawing(){
    Bounding delimit;

    _selectionContext->srcArea.start.x = std::max(_selectionContext->srcArea.start.x,0);
    _selectionContext->srcArea.start.y = std::max(_selectionContext->srcArea.start.y,0);
    _selectionContext->srcArea.end.x = std::min(std::min(_selectionContext->srcArea.end.x, _selectionContext->srcArea.start.x+_toolRuntimeContext.layer->getWidth()-1), _toolRuntimeContext.drawingAreaSize.x);
    _selectionContext->srcArea.end.y = std::min(std::min(_selectionContext->srcArea.end.y, _selectionContext->srcArea.start.y+_toolRuntimeContext.layer->getHeight()-1), _toolRuntimeContext.drawingAreaSize.y);

    for (int y = _selectionContext->srcArea.start.y; y <= _selectionContext->srcArea.end.y; ++y) {
        Point p;
        p.y = GraphicsEngine::clampedTilePoint(y, _toolRuntimeContext.layer->getHeight());
        for (int x = _selectionContext->srcArea.start.x; x <= _selectionContext->srcArea.end.x; ++x){
            p.x = GraphicsEngine::clampedTilePoint(x, _toolRuntimeContext.layer->getWidth());
            if((_toolRuntimeContext.layer->getPixel(p.x, p.y) & 0xFF) == 0) continue;

            if(x < delimit.start.x)  delimit.start.x = x;
            if(y < delimit.start.y)  delimit.start.y = y;
            if(x > delimit.end.x)    delimit.end.x = x;
            if(y > delimit.end.y)    delimit.end.y = y;
        }
    }
    
    if(delimit.start.x == INT_MAX || delimit.start.y == INT_MAX ||
        delimit.end.x == INT_MIN || delimit.end.y == INT_MIN){
        _selectionContext->srcArea = Bounding({0,0}, {0,0});
        _selectionContext->selectionBox = SelectionBox(_selectionContext->srcArea);
        return;
    }
    
    _selectionContext->srcArea = delimit;
    _selectionContext->selectionBox = SelectionBox(_selectionContext->srcArea);
    Bounding bounding = _selectionContext->srcArea;
}

void SelectSession::initSelectData(){
    if(_selectionContext->srcArea.getWidth() <= 0 || _selectionContext->srcArea.getHeight() <= 0){
        _selectionContext->selectionBox = SelectionBox();
        _selectionContext->transformation = Transformation();
        _selectionContext->srcArea = Bounding();

        _selectionContext->enabled = false;
        return;
    }

    if (_selectionContext->data) {
        delete _selectionContext->data;
        _selectionContext->data = nullptr;
    }

    _selectionContext->data = new Surface(_selectionContext->srcArea.getWidth(), _selectionContext->srcArea.getHeight());
    for (int y = 0; y < _selectionContext->srcArea.getHeight(); ++y) {
        Point p;
        p.y = GraphicsEngine::clampedTilePoint(_selectionContext->srcArea.start.y + y, _toolRuntimeContext.layer->getHeight());
        for (int x = 0; x < _selectionContext->srcArea.getWidth(); ++x){
            p.x = GraphicsEngine::clampedTilePoint(_selectionContext->srcArea.start.x + x, _toolRuntimeContext.layer->getWidth());
            
            _selectionContext->data->putPixel(x, y, _toolRuntimeContext.layer->getPixel(p.x , p.y));
        }
    }
}