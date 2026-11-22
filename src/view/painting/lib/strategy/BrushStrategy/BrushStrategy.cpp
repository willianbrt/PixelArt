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

    draw(_startPoint);

    _stabilized = {static_cast<float>(_startPoint.x), static_cast<float>(_startPoint.y)};
    _dirX = false, _dirY = false;
    _change = false;
    _accSpacing = 0;
    _from = _startPoint;
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
    
    ISurface* shape = _drawingContext->shape->getMask();
    ISurface* pattern = _drawingContext->pattern == nullptr ? shape : _drawingContext->pattern->getMask();
    
    Point origem = { 0, 0 };
    if(_drawingContext->alignMode == ALIGN_MODE::LOCAL){
        origem = {
            _startPoint.x - (pattern->getWidth() >> 1),
            _startPoint.y - (pattern->getHeight() >> 1)
        };
    } else if(_drawingContext->alignMode == ALIGN_MODE::SOURCE){
        origem = { 0, 0 };
    } else if(_drawingContext->alignMode == ALIGN_MODE::BRUSHING){
        origem = { 0, 0 };
    }

    StampRasterize stamp({(float)pixel.x + 0.5f, (float)pixel.y + 0.5f},
        {shape->getWidth(), shape->getHeight()},
        _toolRuntimeContext.drawingAreaSize,
        _drawingContext->shape->getTransformation());

    while(stamp.hasNext()){
        Point it = stamp.next();
        Point src = stamp.getSrcPoint();

        if(src.x < 0 || src.y < 0 || src.x >= shape->getWidth() || src.y >= shape->getHeight()) continue;
        
        float mask = 1.0f;
        float shapeMask = (shape->getPixel(src.x, src.y) & 0xFF) / 255.0f;
        if(shapeMask == 0.0f) continue;
        
        Color color;
        if(_drawingContext->shape->getType() == TYPE_BRUSH_TIP::GEOMTRIC && _drawingContext->pattern == nullptr)
            color = _drawingContext->color;
        else if(_drawingContext->enablePattern){
            Point srcPattern = {
                GraphicsEngine::clampedTilePoint((it.x - origem.x) % pattern->getWidth(), pattern->getWidth()),
                GraphicsEngine::clampedTilePoint((it.y - origem.y) % pattern->getHeight(), pattern->getHeight())
            };
            color = _drawingContext->isSolidColor ? _drawingContext->color : pattern->getPixel(srcPattern.x, srcPattern.y);
            
            Color shapeColor = pattern->getPixel(srcPattern.x, srcPattern.y);
            mask = shapeColor.alpha;

            if(_drawingContext->pattern == nullptr) shapeMask = 1.0f;
        } else {
            color = _drawingContext->isSolidColor ? _drawingContext->color : shape->getPixel(src.x, src.y);
        }

        color.alpha = shapeMask * mask * _drawingContext->hardness;

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