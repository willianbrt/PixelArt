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

    _from = to;
}
void BucketStrategy::onTracking(int x, int y){}
void BucketStrategy::onRelease(){ done(); }

void BucketStrategy::draw(const Point& pixel){
    if(pixel.x >= _toolRuntimeContext.screenWidth || pixel.y >= _toolRuntimeContext.screenHeight || pixel.x < 0 || pixel.y < 0)
        return;
    
    Point _current;
    _current.x = GraphicsEngine::clampedTilePoint(pixel.x, _toolRuntimeContext.screenHeight);
    _current.y = GraphicsEngine::clampedTilePoint(pixel.y, _toolRuntimeContext.screenWidth);

    const unsigned int startColorHEX = _toolRuntimeContext.preview->getPixel(_current.x, _current.y);

    if(startColorHEX == _drawingContext->color)
        return;

    vector<Point> queue;
    queue.push_back(pixel);
    
    while(queue.size() != 0) {
        Point point = queue.back();
        queue.pop_back();
        
        if(point.x >= _toolRuntimeContext.screenWidth || point.y >= _toolRuntimeContext.screenHeight || point.x < 0 || point.y < 0) continue;

        _current.x = GraphicsEngine::clampedTilePoint(point.x, _toolRuntimeContext.screenHeight);
        _current.y = GraphicsEngine::clampedTilePoint(point.y, _toolRuntimeContext.screenWidth);
        
        if(_toolRuntimeContext.preview->getPixel(_current.x, _current.y) != startColorHEX) continue;

        _toolRuntimeContext.drawingSession->putMirroredPixel(_current.x, _current.y, _drawingContext->color);

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
    _cursorContext.scale = _drawingContext->size;
    return &_cursorContext;
}