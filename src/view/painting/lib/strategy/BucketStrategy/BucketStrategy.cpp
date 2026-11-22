#include "./BucketStrategy.h"
BucketStrategy::~BucketStrategy(){}


BucketStrategy::BucketStrategy(BucketContext* bucketContext,  DrawingContext* drawingContext) :
_bucketContext(bucketContext),
_drawingContext(drawingContext)
{
}
void BucketStrategy::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
    _cursorContext.enable = false;
    _initialized = true;
    _toolRuntimeContext.drawingSession->begin(_toolRuntimeContext.layer);

    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);

    draw(to);
    done();
    
    _from = to;
}
void BucketStrategy::onTracking(int x, int y){}
void BucketStrategy::onRelease(){  }

void BucketStrategy::draw(const Point& pixel){
    if(pixel.x >= _toolRuntimeContext.drawingAreaSize.x || pixel.y >= _toolRuntimeContext.drawingAreaSize.y || pixel.x < 0 || pixel.y < 0)
        return;
    
    int canvasW = _toolRuntimeContext.editor->getWidth();
    int canvasH = _toolRuntimeContext.editor->getHeight();

    Point _current;
    _current.x = GraphicsEngine::clampedTilePoint(pixel.x, canvasW);
    _current.y = GraphicsEngine::clampedTilePoint(pixel.y, canvasH);


    Color fillColor = _drawingContext->color;
    fillColor.alpha *= _drawingContext->hardness;

    Color targetColor = _toolRuntimeContext.drawingSession->getModifiedPixel(_current.x, _current.y);

    if(targetColor.red == fillColor.red &&
        targetColor.green == fillColor.green &&
        targetColor.blue == fillColor.blue &&
        targetColor.alpha == fillColor.alpha)
        return;

    vector<Point> queue;
    queue.push_back(pixel);
    while(queue.size() != 0) {
        Point point = queue.back();
        queue.pop_back();

        if(point.x >= _toolRuntimeContext.drawingAreaSize.x || point.y >= _toolRuntimeContext.drawingAreaSize.y || point.x < 0 || point.y < 0) continue;
        
        _current.x = GraphicsEngine::clampedTilePoint(point.x, canvasW);
        _current.y = GraphicsEngine::clampedTilePoint(point.y, canvasH);
        
        Color canvasColor = _toolRuntimeContext.drawingSession->getModifiedPixel(_current.x, _current.y);
        if(!(targetColor.red == canvasColor.red &&
            targetColor.green == canvasColor.green &&
            targetColor.blue == canvasColor.blue &&
            targetColor.alpha == canvasColor.alpha)
            ||  _toolRuntimeContext.drawingSession->isModified(_current.x, _current.y)
        )
            continue;

        _toolRuntimeContext.drawingSession->putPixel(_current.x, _current.y, fillColor, &WriteMode::normal);

        queue.push_back(Point(point.x+1, point.y));
        queue.push_back(Point(point.x-1, point.y));
        queue.push_back(Point(point.x,   point.y+1));
        queue.push_back(Point(point.x,   point.y-1));
        
    }
}
bool BucketStrategy::isInitialized(){
    return _initialized;
}
void BucketStrategy::done() {
    _toolRuntimeContext.drawingSession->commit();
    _initialized = false;
    _cursorContext.enable = true;
}
void BucketStrategy::abort(){
    _toolRuntimeContext.drawingSession->clear();
    _initialized = false;
    _cursorContext.enable = true;
}
CursorContext* BucketStrategy::getCursorContext(){
    return &_cursorContext;
}