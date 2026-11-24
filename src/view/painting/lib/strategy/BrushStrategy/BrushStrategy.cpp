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
    

    StampRasterize stamp({(float)pixel.x + 0.5f, (float)pixel.y + 0.5f},
        {shape->getWidth(), shape->getHeight()},
        _toolRuntimeContext.drawingAreaSize,
        _drawingContext->shape->getTransformation());

    while(stamp.hasNext()){
        Point it = stamp.next();
        Point src = stamp.getSrcPoint();

        if(src.x < 0 || src.y < 0 || src.x >= shape->getWidth() || src.y >= shape->getHeight()) continue;
        
        Color shapeColor = shape->getPixel(src.x, src.y);
        if(shapeColor.alpha == 0.0f) continue;
        
        Color color = _drawingContext->color;
        if(_drawingContext->alignMode == ALIGN_MODE::BRUSHING){
            if(_drawingContext->shape->getType() == TYPE_BRUSH_TIP::IMAGE){                
                if(_drawingContext->isSolidColor){
                    color.alpha *= shapeColor.alpha;
                } else {
                    color = shapeColor;
                }
            }
        } else {
            color = getFillColor(it.x, it.y);
        }

        color.alpha *= _drawingContext->hardness;

        _toolRuntimeContext.drawingSession->putPixel(it.x, it.y, color, &WriteMode::normal);
    }
}

Color BrushStrategy::getFillColor(const int& x, const int& y){
    Color color = _drawingContext->color;

    ISurface* pattern =
        (_drawingContext->pattern && _drawingContext->enablePattern) ? _drawingContext->pattern->getMask() : 
        // _drawingContext->shape->getType() == TYPE_BRUSH_TIP::IMAGE ? _drawingContext->shape->getMask() :
        nullptr;
    

    if(pattern != nullptr){
        if(_drawingContext->alignMode == ALIGN_MODE::LOCAL){
            origem = {
                _startPoint.x - (pattern->getWidth() >> 1),
                _startPoint.y - (pattern->getHeight() >> 1)
            };
        } else if(_drawingContext->alignMode == ALIGN_MODE::SOURCE){
            origem = { 0, 0 };
        } 
        Point srcPattern = {
            GraphicsEngine::clampedTilePoint((x - origem.x) % pattern->getWidth(), pattern->getWidth()),
            GraphicsEngine::clampedTilePoint((y - origem.y) % pattern->getHeight(), pattern->getHeight())
        };
        
        Color patternColor = pattern->getPixel(srcPattern.x, srcPattern.y);

        if(_drawingContext->isSolidColor){
            color.alpha *= patternColor.alpha;
        } else {
            color = patternColor;
        }
    }

    // color.alpha *= _drawingContext->hardness;

    return color;
}

/*
    preserve_alpha: color.alpha *= shapeMask; 
    solid_color: color.alpha *= shapeMask; 

*/

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