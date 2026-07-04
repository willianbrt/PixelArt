#include "./CircleStrategy.h"

CircleStrategy::~CircleStrategy(){}


CircleStrategy::CircleStrategy(CircleContext* circleContext,  DrawingContext* drawingContext, SymmetryContext* symmetryContext) :
_circleContext(circleContext),
_drawingContext(drawingContext),
_symmetryContext(symmetryContext)
{
    _drawingContext->color = 0xff0000ff;
    _drawingContext->size = 4;
    _drawingContext->hardness = 1.0f;

    _symmetryContext->isMirrorX =false;
    _symmetryContext->isMirrorY =false;
}
void CircleStrategy::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
    _cursorContext.enable = false;
    _initialized = true;
    _toolRuntimeContext.drawingSession->begin(_toolRuntimeContext.layer);

    pivotPoint = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);

    _flagPoint = pivotPoint;
}
void CircleStrategy::onTracking(int x, int y){
    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    if (to == _flagPoint) return;


    draw({
        (pivotPoint.x + to.x ) >> 1,
        (pivotPoint.y + to.y ) >> 1
    });

    _flagPoint = to;
}
void CircleStrategy::onRelease(){ done(); }

void CircleStrategy::draw(const Point& pixel){
    _toolRuntimeContext.drawingSession->clear();

    const int gap = 4;
    const int linelength = 4;
    const int patternLength = gap + linelength;
    int acc;

    
    // EXTERNO
    const Point radius = { 
        std::abs(pixel.x - pivotPoint.x + 1), 
        std::abs(pixel.y - pivotPoint.y + 1)
    };
    const Point radius2 = { radius.x * radius.x, radius.y * radius.y };

    const int twoRx2 = radius2.x << 1;
    const int twoRy2 = radius2.y << 1;

    Point point = { 0, radius.y};
    int decisionParam = radius2.y - (radius2.x * radius.y) + (radius2.x >> 2);
    Point delta = {0, twoRx2 * point.y};

// INTERNO
    const Point radiusInner = { std::max(radius.x - _drawingContext->size + 1, 0), std::max(radius.y - _drawingContext->size + 1, 0) };
    const Point radiusInner2 = { radiusInner.x * radiusInner.x, radiusInner.y * radiusInner.y };

    const int twoRix2 = radiusInner2.x << 1;
    const int twoRiy2 = radiusInner2.y << 1;

    int innerDecisionParam = radiusInner2.y - (radiusInner2.x * radiusInner.y) + (radiusInner2.x >> 2);
    Point inner = { 0, radiusInner.y};
    Point innerDelta = { 0, twoRix2 * inner.y};
    
    // traceSymetricOutline(inner, point, pixel);

    while(delta.x < delta.y){
        if(acc > patternLength) { acc = 0; }

        if(acc <= linelength){
            for (int y = inner.y; y <= point.y; y++) {
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + point.x, pixel.y + y, _drawingContext->color, _symmetryContext);
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - point.x, pixel.y + y, _drawingContext->color, _symmetryContext);
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + point.x, pixel.y - y, _drawingContext->color, _symmetryContext);
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - point.x, pixel.y - y, _drawingContext->color, _symmetryContext);
            }

            traceSymetricOutline(inner, point, pixel);
        }
        // if(_isFilled) traceFilled(layer, inner.x, inner.y, _drawingContext->color,  _toolRuntimeContext.screenWidth,  _toolRuntimeContext.screenHeight);
        

        point.x++;
        delta.x += twoRy2;
        acc++;
        
        if (decisionParam < 0) {
            decisionParam += radius2.y + delta.x;
        } else {
            acc++;
            point.y--;
            delta.y -= twoRx2;
            decisionParam += radius2.y + delta.x - delta.y;
        }

        if(innerDelta.x >= innerDelta.y) continue;

        inner.x++;

        innerDelta.x += twoRiy2;
        if (innerDecisionParam < 0) {
            innerDecisionParam += radiusInner2.y + innerDelta.x;
        } else {
            inner.y--;
            innerDelta.y -= twoRix2;
            innerDecisionParam += radiusInner2.y + innerDelta.x - innerDelta.y;
        }

    }
    
    decisionParam  = radius2.y * point.x*point.x + radius2.x * point.y*(point.y  - 1) + (radius2.x  >> 2) - radius2.x*radius2.y;
    innerDecisionParam = radiusInner2.y * inner.x*inner.x + radiusInner2.x * inner.y*(inner.y - 1) + (radiusInner2.x >> 2) - radiusInner2.x*radiusInner2.y;

    while (point.y >= 0){
        

        if(acc > patternLength) { acc = 0; }

        if(acc <= linelength){
            for (int x = inner.x; x <= point.x; x++) {
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + x, pixel.y + point.y, _drawingContext->color, _symmetryContext);
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - x, pixel.y + point.y, _drawingContext->color, _symmetryContext);
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + x, pixel.y - point.y, _drawingContext->color, _symmetryContext);
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - x, pixel.y - point.y, _drawingContext->color, _symmetryContext);
            }
            traceSymetricOutline(inner, point, pixel);
        }

        // if(_isFilled) traceFilled(layer, inner.x, inner.y, _drawingContext->color,  _toolRuntimeContext.screenWidth,  _toolRuntimeContext.screenHeight);

        point.y--;
        delta.y -= twoRx2;
        acc++;

        if (decisionParam > 0) {
            decisionParam += radius2.x - delta.y;
        } else {
            point.x++;
            delta.x += twoRy2;
            decisionParam += radius2.x - delta.y + delta.x;
            acc++;
        }

        if(inner.y <= 0) continue;

        inner.y--;
        innerDelta.y -= twoRix2;

        if (innerDecisionParam > 0) {
            innerDecisionParam += radiusInner2.x - innerDelta.y;
        } else {
            inner.x++;
            innerDelta.x += twoRiy2;
            innerDecisionParam += radiusInner2.x - innerDelta.y + innerDelta.x;
        }
    }
}


void CircleStrategy::traceSymetricOutline(const Point& from, const Point& to, const Point& center) {
    // for(int y = from.y; y <= to.y; y++){
    //     for(int x = from.x; x <= to.x; x++){
    //         _toolRuntimeContext.drawingSession->putMirroredPixel(center.x + x, center.y + y,  _drawingContext->color, _symmetryContext);
    //         _toolRuntimeContext.drawingSession->putMirroredPixel(center.x - x, center.y + y,  _drawingContext->color, _symmetryContext);
    //         _toolRuntimeContext.drawingSession->putMirroredPixel(center.x + x, center.y - y,  _drawingContext->color, _symmetryContext);
    //         _toolRuntimeContext.drawingSession->putMirroredPixel(center.x - x, center.y - y,  _drawingContext->color, _symmetryContext);
    //     }
    // }
    // for(int y = (to.y - (_drawingContext->size>>1)); y < (to.y + (_drawingContext->size>>1)); y++){
    //     for(int x = (to.x - (_drawingContext->size>>1)); x < (to.x + (_drawingContext->size>>1)); x++){
    //         _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x + x, center.y + y, 0xFFFF00aa, _symmetryContext);
    //         _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x - x, center.y + y, 0xFFFF00aa, _symmetryContext);
    //         _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x + x, center.y - y, 0xFFFF00aa, _symmetryContext);
    //         _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x - x, center.y - y, 0xFFFF00aa, _symmetryContext);
    //     }
    // }

    _toolRuntimeContext.drawingSession->putMirroredPixel(center.x + from.x, center.y + from.y, 0x00FFFFFF, _symmetryContext);
    _toolRuntimeContext.drawingSession->putMirroredPixel(center.x - from.x, center.y + from.y, 0x00FFFFFF, _symmetryContext);
    _toolRuntimeContext.drawingSession->putMirroredPixel(center.x + from.x, center.y - from.y, 0x00FFFFFF, _symmetryContext);
    _toolRuntimeContext.drawingSession->putMirroredPixel(center.x - from.x, center.y - from.y, 0x00FFFFFF, _symmetryContext);

    _toolRuntimeContext.drawingSession->putMirroredPixel(center.x + to.x, center.y + to.y, 0x00FF00FF, _symmetryContext);
    _toolRuntimeContext.drawingSession->putMirroredPixel(center.x - to.x, center.y + to.y, 0x00FF00FF, _symmetryContext);
    _toolRuntimeContext.drawingSession->putMirroredPixel(center.x + to.x, center.y - to.y, 0x00FF00FF, _symmetryContext);
    _toolRuntimeContext.drawingSession->putMirroredPixel(center.x - to.x, center.y - to.y, 0x00FF00FF, _symmetryContext);
}

void CircleStrategy::traceFilled(const Point& from, const Point& center) {
    if(from.x<=0) return;
    for (int xi = std::floor(center.x - from.x + 1) ; xi < center.x + from.x; xi++) {
        _toolRuntimeContext.drawingSession->putMirroredPixel(xi, std::floor(center.y + from.y), _drawingContext->color, _symmetryContext);
        _toolRuntimeContext.drawingSession->putMirroredPixel(xi, std::floor(center.y - from.y), _drawingContext->color, _symmetryContext);
    }
}
bool CircleStrategy::isInitialized(){
    return _initialized;
}
void CircleStrategy::done() {
    _toolRuntimeContext.drawingSession->commit();
    _initialized = false;
    _cursorContext.enable = true;
}
void CircleStrategy::abort(){
    _toolRuntimeContext.drawingSession->clear();
    _initialized = false;
    _cursorContext.enable = true;
}
CursorContext* CircleStrategy::getCursorContext(){
    _cursorContext.scale = _drawingContext->size;
    return &_cursorContext;
}