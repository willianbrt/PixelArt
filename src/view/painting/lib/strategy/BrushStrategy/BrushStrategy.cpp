#include "BrushStrategy.h"

BrushStrategy::BrushStrategy(BrushContext* brushContext, DrawingContext* drawingContext) :
_brushContext(brushContext),
_drawingContext(drawingContext)
{
    _brushContext->transformation.setScale({1.0f, 1.0f});
    _brushContext->transformation.setRad(0  * M_PI / 180);
}
void BrushStrategy::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
    _cursorContext.enable = false;
    _initialized = true;
    _toolRuntimeContext.drawingSession->begin(_toolRuntimeContext.layer);

    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);

    draw(to);

    _stabilized = {static_cast<float>(to.x), static_cast<float>(to.y)};
    _dirX = false, _dirY = false;
    _change = false;
    _accSpacing = 0;
    _from = to;
}
void BrushStrategy::onTracking(int x, int y){
    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    
    float sx = _stabilized.x + (to.x - _stabilized.x) * (1.0f - _brushContext->getStabilizer());
    float sy = _stabilized.y + (to.y - _stabilized.y) * (1.0f - _brushContext->getStabilizer());

    _stabilized = {sx, sy};

    Point drawPoint = {static_cast<int>(sx), static_cast<int>(sy)};
    if (drawPoint == _from) {return;}


    LineRasterize line(_from,  drawPoint);
    while(line.hasNext()){
        Point p = line.next();
        if (p == _from) {continue;}

        bool dirX, dirY;
        dirX = (_from.x == p.x), dirY = (_from.y == p.y);
        
        if (_accSpacing >= _brushContext->getSpacing()) {
            if(_brushContext->getPixelPerfect()){
                if(dirX && _dirY && !_change){
                    _change = true;
                    _toolRuntimeContext.drawingSession->putPixel(_from.x, _from.y, _previousColor);
                }
                else if(_dirX && dirY && ! _change) {
                    _change = true;
                    _toolRuntimeContext.drawingSession->putPixel(_from.x, _from.y, _previousColor);
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
    _previousColor = _toolRuntimeContext.drawingSession->getPixel(pixel.x, pixel.y);
    ISurface* shape = _brushContext->getShape();
    StampRasterize stamp({(float)pixel.x + 0.5f, (float)pixel.y + 0.5f},
        {shape->getWidth(), shape->getHeight()},
        _toolRuntimeContext.drawingAreaSize,
        _brushContext->transformation);

    while(stamp.hasNext()){
        Point it = stamp.next();
        Point src = stamp.getSrcPoint();

        if(src.x < 0 || src.y < 0 || src.x >= shape->getWidth() || src.y >= shape->getHeight()) continue;
        
        unsigned int topColor = _brushContext->getColor();
        GraphicsEngine::setOpacity(topColor, (shape->getPixel(src.x, src.y) & 0xFF) / 255.0f);

        _toolRuntimeContext.drawingSession->blendMirroredPixel(it.x, it.y, topColor);
    }
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