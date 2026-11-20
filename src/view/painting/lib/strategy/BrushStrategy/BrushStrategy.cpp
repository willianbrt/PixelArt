#include "BrushStrategy.h"

BrushStrategy::BrushStrategy(BrushContext* brushContext, DrawingContext* drawingContext) :
_brushContext(brushContext),
_drawingContext(drawingContext)
{}
void BrushStrategy::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
    _cursorContext.enable = false;
    _initialized = true;
    _toolRuntimeContext.drawingSession->begin(_toolRuntimeContext.layer);

    _startPoint = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);

    draw(_startPoint);

    _stabilized = {static_cast<float>(_startPoint.x), static_cast<float>(_startPoint.y)};
    _dirX = false, _dirY = false;
    _change = false;
    _accSpacing = 0;
    _from = _startPoint;
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
    
    ISurface* shape = _drawingContext->getShape();
    ISurface* pattern = _drawingContext->getPattern() == nullptr ? shape : _drawingContext->getPattern();
    
    Point origem = { 0, 0 };
    if(_drawingContext->getAlignMode() == ALIGN_MODE::LOCAL){
        origem = {
            _startPoint.x - (pattern->getWidth() >> 1),
            _startPoint.y - (pattern->getHeight() >> 1)
        };
    } else if(_drawingContext->getAlignMode() == ALIGN_MODE::SOURCE){
        origem = { 0, 0 };
    } else if(_drawingContext->getAlignMode() == ALIGN_MODE::BRUSHING){
        origem = { 0, 0 };
    }

    StampRasterize stamp({(float)pixel.x + 0.5f, (float)pixel.y + 0.5f},
        {shape->getWidth(), shape->getHeight()},
        _toolRuntimeContext.drawingAreaSize,
        *_drawingContext->getTransformation());

    while(stamp.hasNext()){
        Point it = stamp.next();
        Point src = stamp.getSrcPoint();

        if(src.x < 0 || src.y < 0 || src.x >= shape->getWidth() || src.y >= shape->getHeight()) continue;
        
        float mask = 1.0f;
        float shapeMask = (shape->getPixel(src.x, src.y) & 0xFF) / 255.0f;
        if(shapeMask == 0.0f) continue;
        
        unsigned int color;
        if((_drawingContext->getTypeBrushTip() == TYPE_BRUSH_TIP::GEOMTRIC && _drawingContext->getPattern() == nullptr))
            color = _drawingContext->getColor();
        else if(_drawingContext->isPatternEnable()){
            Point srcPattern = {
                GraphicsEngine::clampedTilePoint((it.x - origem.x) % pattern->getWidth(), pattern->getWidth()),
                GraphicsEngine::clampedTilePoint((it.y - origem.y) % pattern->getHeight(), pattern->getHeight())
            };
            color = _drawingContext->isSolidColor() ?
                    _drawingContext->getColor() :
                    pattern->getPixel(srcPattern.x, srcPattern.y);

            mask = (pattern->getPixel(srcPattern.x, srcPattern.y) & 0xFF) / 255.0f;

            if(_drawingContext->getPattern() == nullptr) shapeMask = 1.0f;
        } else {
            color = _drawingContext->isSolidColor()  ? 
                    _drawingContext->getColor() : 
                    shape->getPixel(src.x, src.y);
        }


        color = (color & 0xFFFFFF00) | static_cast<int>(shapeMask * mask * _drawingContext->getHardness() * 255.0f);

        _toolRuntimeContext.drawingSession->putPixel(it.x, it.y, color, &WriteMode::normal);
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