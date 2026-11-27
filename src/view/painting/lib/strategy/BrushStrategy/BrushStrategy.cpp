#include "BrushStrategy.h"

BrushStrategy::BrushStrategy(DrawingContext* drawingContext) :
_drawingContext(drawingContext)
{}
void BrushStrategy::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
    _cursorContext.enable = false;
    _initialized = true;
    _toolRuntimeContext.drawingSession->begin(_toolRuntimeContext.layer);

    _startPoint = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    

    _stabilized = {static_cast<float>(_startPoint.x), static_cast<float>(_startPoint.y)};
    _dirX = false, _dirY = false;
    _change = false;
    _accSpacing = 0;
    _from = _startPoint;

    if(_drawTool)
        delete _drawTool;
    _drawTool = new DrawTool(_drawingContext, &_toolRuntimeContext);

    draw(_startPoint);
}
void BrushStrategy::onTracking(int x, int y){
    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    
    float sx = _stabilized.x + (to.x - _stabilized.x) * (1.0f - _drawingContext->stabilizer);
    float sy = _stabilized.y + (to.y - _stabilized.y) * (1.0f - _drawingContext->stabilizer);

    _stabilized = {sx, sy};

    Point drawPoint = {static_cast<int>(sx), static_cast<int>(sy)};
    if (drawPoint == _from) {return;}


    LineRasterize line(_from,  drawPoint);
    while(line.hasNext()){
        Point p = line.next();
        if (p == _from) {continue;}

        bool dirX, dirY;
        dirX = (_from.x == p.x), dirY = (_from.y == p.y);
        
        if (_accSpacing >= _drawingContext->spacing) {
            if(_drawingContext->pixelPerfect){
                if(dirX && _dirY && !_change){
                    _change = true;
                    _toolRuntimeContext.drawingSession->putPixel(_from.x, _from.y, _previousColor, &WriteMode::normal);
                }
                else if(_dirX && dirY && ! _change) {
                    _change = true;
                    _toolRuntimeContext.drawingSession->putPixel(_from.x, _from.y, _previousColor, &WriteMode::normal);
                } else{
                    _change = false;
                }
            }

            draw(p);
            _accSpacing = 0;
        } else {
            if(dirX xor dirY)
                _accSpacing+=2;
            else 
                _accSpacing++;
        }
        _dirX = dirX;
        _dirY = dirY;
        _from = p;
    }
}
void BrushStrategy::onRelease(){
    done();
}

void BrushStrategy::draw(const Point& pixel){
    _previousColor = _toolRuntimeContext.drawingSession->getModifiedPixel(pixel.x, pixel.y);
    _drawTool->shape(pixel.x, pixel.y);
}

bool BrushStrategy::isInitialized(){
    return _initialized;
}
void BrushStrategy::done() {
    _toolRuntimeContext.drawingSession->commit();
    _initialized = false;
    _cursorContext.enable = true;
}
void BrushStrategy::abort(){
    _toolRuntimeContext.drawingSession->clear();
    _initialized = false;
    _cursorContext.enable = true;
}
CursorContext* BrushStrategy::getCursorContext(){
    return &_cursorContext;
}