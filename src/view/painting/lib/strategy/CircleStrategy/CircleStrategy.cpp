#include "./CircleStrategy.h"

void CircleStrategy::fillEllipseStrokeDistance(
    int cx, int cy,
    float rx, float ry,
    float thickness,
    float dashLength,   // tamanho do traço
    float gapLength     // espaço
) {
    _toolRuntimeContext.drawingSession->clear();
    if (rx <= 0 || ry <= 0) return;

    int rxOut = rx;
    int ryOut = ry;

    int rxIn = std::max(0.0f, rx - thickness);
    int ryIn = std::max(0.0f, ry - thickness);

    long long rxOut2 = (long long)rxOut * rxOut;
    long long ryOut2 = (long long)ryOut * ryOut;

    long long rxIn2  = (long long)rxIn * rxIn;
    long long ryIn2  = (long long)ryIn * ryIn;

    float acc = 0.0f;
    float period = dashLength + gapLength;

    const float SQRT2 = 1.41421356f;

    int prevX = 0;
    int prevY = ryOut;

    for (int y = -ryOut; y <= ryOut; y++) {
        int xOut = rxOut;

        while ((long long)xOut * xOut * ryOut2 +
            (long long)y * y * rxOut2 >
            rxOut2 * ryOut2) {
            xOut--;
        }

        int xIn = rxIn;

        while (xIn > 0 &&
            (long long)xIn * xIn * ryIn2 +
            (long long)y * y * rxIn2 >
            rxIn2 * ryIn2) {
            xIn--;
        }
        

        // percorre a borda externa (lado direito)
        int px = xOut;
        int py = y;

        int dx = px - prevX;
        int dy = py - prevY;

        if (dx != 0 || dy != 0) {
            acc += (dx != 0 && dy != 0) ? SQRT2 : 1.0f;
        }

        float m = fmod(acc, period);
        bool draw = (m < dashLength);

        if (draw) {
            for (int x = xIn + 1; x <= xOut; x++) {

                _toolRuntimeContext.drawingSession->putMirroredPixel(
                    cx + x, cy + y,
                    _drawingContext->color,
                    _symmetryContext
                );

                _toolRuntimeContext.drawingSession->putMirroredPixel(
                    cx - x, cy + y,
                    _drawingContext->color,
                    _symmetryContext
                );
            }
        }

        prevX = px;
        prevY = py;
    }
}
inline bool insideArc(
    float x,
    float y,
    float rx,
    float ry,
    float start,
    float end)
{
    // Ângulo paramétrico da elipse
    float t = atan2(y / ry, x / rx);

    if (t < 0)
        t += 2.0f * M_PI;

    if (start <= end)
        return t >= start && t <= end;

    return t >= start || t <= end;
}
inline int cross(const Point& A, const Point& B, const Point& P) {
    return (B.x - A.x)*(P.y - A.y) - (B.y - A.y)*(P.x - A.x);
}
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

    // pivotPoint = {5,5};
    _flagPoint = pivotPoint;
}
void CircleStrategy::onTracking(int x, int y){
    Point to = _toolRuntimeContext.canvasSettings->cursorToCanvas(x, y);
    if (to == _flagPoint) return;


    // to = { 22,22 };
    // fillEllipseStrokeDistance(
    //         (pivotPoint.x + to.x ) >> 1,
    //         (pivotPoint.y + to.y ) >> 1,
    //         std::abs(to.x - pivotPoint.x)*0.5f,
    //         std::abs(to.y - pivotPoint.y)*0.5f,
    //         5,
    //         3,
    //         3
    //     );
    draw({
        (pivotPoint.x + to.x ) >> 1,
        (pivotPoint.y + to.y ) >> 1
    });

    _flagPoint = to;
}
void CircleStrategy::onRelease(){ done(); }


void CircleStrategy::draw(const Point& pixel){
    _toolRuntimeContext.drawingSession->clear();

    auto plot = [&](int sx, int sy, unsigned int color){
        _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x  + sx, pixel.y + sy, color, _symmetryContext);
    };
        plot(0 , 0, 0xFFFF00FF);

    //OUTTER
    const Point radius = { 
        std::abs(pixel.x - pivotPoint.x), 
        std::abs(pixel.y - pivotPoint.y)
    };
    plot(radius.x , radius.y, 0xFFFF00FF);
    const Point radius2 = { radius.x * radius.x, radius.y * radius.y };
    const int twoRx2 = radius2.x << 1;
    const int twoRy2 = radius2.y << 1;
    Point outter = { 0, radius.y};
    int decisionParam = radius2.y - (radius2.x * radius.y) + (radius2.x >> 2);
    Point delta = {0, twoRx2 * outter.y};

    // INNER
    const Point radiusInner = { std::max(radius.x - _drawingContext->size + 1, 0), std::max(radius.y - _drawingContext->size + 1, 0) };
    const Point radiusInner2 = { radiusInner.x * radiusInner.x, radiusInner.y * radiusInner.y };
    const int twoRIx2 = radiusInner2.x << 1;
    const int twoRIy2 = radiusInner2.y << 1;
    Point inner = { 0, radiusInner.y};
    int dpInner = radiusInner2.y - (radiusInner2.x * radiusInner.y) + (radiusInner2.x >> 2);
    Point deltaInner = {0, twoRIx2 * inner.y};


    const int gap = 0;
    const int linelength = 3;
    const int patternLength = gap + linelength;
    int acc;
    float m = 0;
    Point start = outter;

    
    auto cinner = [&](Point& p){
        float nx = (float)p.x/ (float)radius2.x;
        float ny = (float)p.y/ (float)radius2.y;

        float length = sqrt(nx*nx + ny*ny);
        if (length < 1e-6f) length = 1.0f;
        nx /= length;
        ny /= length;

        Point inner;
        inner.x = std::max((int)(p.x - nx * (_drawingContext->size-1)), 0);
        inner.y = std::max((int)(p.y - ny * (_drawingContext->size-1)), 0);
        return inner;

    };
    auto circleTest = [&](const Point& center, int radius, const Point& p) {
        int dx = p.x - center.x;
        int dy = p.y - center.y;
        return dx*dx + dy*dy - radius*radius;
    };
    auto drawell = [&](const Point& start, const Point& outter, const Point& inners, const Point& innere) {
        
        int minX = std::min(std::min(std::min(start.x, outter.x), inners.x), innere.x);
        int maxX = std::max(std::max(std::max(start.x, outter.x), inners.x), innere.x);
        int minY = std::min(std::min(std::min(start.y, outter.y), inners.y), innere.y);
        int maxY = std::max(std::max(std::max(start.y, outter.y), inners.y), innere.y);

        int rxy = radius2.x * radius2.y;
        for(int y = minY; y <= maxY; y++){
            for(int x = minX; x <= maxX; x++){


                int v = x*x*radius2.y + y*y*radius2.x;
                int limit = radius2.x * radius2.y + (radius2.x + radius2.y);

                if (v > limit) continue;

                int v2 = x*x*radiusInner2.y + y*y*radiusInner2.x;
                int limit2 = radiusInner2.x * radiusInner2.y - (radiusInner2.x + radiusInner2.y);
                if (v2 <= limit2) continue;


                int cs = cross({0,0}, inners,{x,y});
                int ce = cross({0,0}, innere,{x,y});
                if(ce < 0 || cs > 0) continue;
                plot(+ x , + y, 0x0000ffFF);
                plot(- x , + y, 0x0000ffFF);
                plot(+ x , - y, 0x0000ffFF);
                plot(- x , - y, 0x0000ffFF);
            }
        }
        // plot(+ inners.x , + inners.y, 0x00FF00FF);
        // plot(- inners.x , + inners.y, 0x00FF00FF);
        // plot(+ inners.x , - inners.y, 0x00FF00FF);
        // plot(- inners.x , - inners.y, 0x00FF00FF);

        // plot(+ innere.x , + innere.y, 0x00ffffFF);
        // plot(- innere.x , + innere.y, 0x00ffffFF);
        // plot(+ innere.x , - innere.y, 0x00ffffFF);
        // plot(- innere.x , - innere.y, 0x00ffffFF);

        // plot(+ start.x , + start.y, 0xFFFF00FF);
        // plot(- start.x , + start.y, 0xFFFF00FF);
        // plot(+ start.x , - start.y, 0xFFFF00FF);
        // plot(- start.x , - start.y, 0xFFFF00FF);

        // plot(+ outter.x , + outter.y, 0xaa00aaFF);
        // plot(- outter.x , + outter.y, 0xaa00aaFF);
        // plot(+ outter.x , - outter.y, 0xaa00aaFF);
        // plot(- outter.x , - outter.y, 0xaa00aaFF);
            
        
    };

    Point dl;
    float dp;
    int prevYOuter = outter.y;
    int prevYInner = inner.y;
    int prevXOuter = outter.x;
    int prevXInner = inner.x;
    while(delta.x < delta.y){        
        m = fmod(acc, patternLength);

        if(m==0.0f){
            start = outter;
            dl = delta;
            dp = decisionParam;
        }
        if(m < linelength ){
            int tx =  std::max(outter.x, inner.x);
            for(int ty = outter.y; ty >= inner.y; ty--){
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + tx, pixel.y + ty, 0xffaa00ff, _symmetryContext);
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - tx, pixel.y + ty, 0xffaa00ff, _symmetryContext);
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + tx, pixel.y - ty, 0xffaa00ff, _symmetryContext);
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - tx, pixel.y - ty, 0xffaa00ff, _symmetryContext);
            }


            _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + outter.x, pixel.y + outter.y, _drawingContext->color, _symmetryContext);
            _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - outter.x, pixel.y + outter.y, _drawingContext->color, _symmetryContext);
            _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + outter.x, pixel.y - outter.y, _drawingContext->color, _symmetryContext);
            _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - outter.x, pixel.y - outter.y, _drawingContext->color, _symmetryContext);
            
            _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + inner.x, pixel.y + inner.y, _drawingContext->color, _symmetryContext);
            _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - inner.x, pixel.y + inner.y, _drawingContext->color, _symmetryContext);
            _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + inner.x, pixel.y - inner.y, _drawingContext->color, _symmetryContext);
            _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - inner.x, pixel.y - inner.y, _drawingContext->color, _symmetryContext);
                    
        }
        prevYOuter = outter.y;
        prevYInner = inner.y;
        prevXOuter = outter.x;
        prevXInner = inner.x;


        if(m == (linelength-1) && gap > 0 && false){
            Point inners = cinner(start);
            Point innere = cinner(outter);
            Point i = start;
            while(i.x <= outter.x){
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + i.x, pixel.y + i.y, _drawingContext->color, _symmetryContext);
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - i.x, pixel.y + i.y, _drawingContext->color, _symmetryContext);
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + i.x, pixel.y - i.y, _drawingContext->color, _symmetryContext);
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - i.x, pixel.y - i.y, _drawingContext->color, _symmetryContext);
                i.x++;
                dl.x += twoRy2;
                
                if (dp < 0) {
                    dp += radius2.y + dl.x;
                } else {
                    i.y--;
                    dl.y -= twoRx2;
                    dp += radius2.y + dl.x - dl.y;
                }
            }
            // drawell(start, outter, inners, innere);
        }

        

        outter.x++;
        delta.x += twoRy2;
        acc++;
        
        if (decisionParam < 0) {
            decisionParam += radius2.y + delta.x;
        } else {
            outter.y--;
            delta.y -= twoRx2;
            decisionParam += radius2.y + delta.x - delta.y;
            acc+=0.41421356f;
        }


        if(deltaInner.x >= deltaInner.y) continue;
        inner.x++;
        deltaInner.x += twoRIy2;
        
        if (dpInner < 0) {
            dpInner += radiusInner2.y + deltaInner.x;
        } else {
            inner.y--;
            deltaInner.y -= twoRIx2;
            dpInner += radiusInner2.y + deltaInner.x - deltaInner.y;
        }

    }
    
    decisionParam  = radius2.y * outter.x*outter.x + radius2.x * outter.y*(outter.y  - 1) + (radius2.x  >> 2) - radius2.x*radius2.y;
    dpInner  = radiusInner2.y * inner.x*inner.x + radiusInner2.x * inner.y*(inner.y  - 1) + (radiusInner2.x  >> 2) - radiusInner2.x*radiusInner2.y;
  
    while (outter.y >= 0){
        m = fmod(acc, patternLength);


        if(m==0.0f){
            start = outter;
            dl = delta;
            dp = decisionParam;
        }

        if(m == (linelength-1)&& gap >0 && false){
            Point inners = cinner(start);
            Point innere = cinner(outter);
            
            Point i = start;
            while(i.y >= outter.y){
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + i.x, pixel.y + i.y, _drawingContext->color, _symmetryContext);
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - i.x, pixel.y + i.y, _drawingContext->color, _symmetryContext);
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + i.x, pixel.y - i.y, _drawingContext->color, _symmetryContext);
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - i.x, pixel.y - i.y, _drawingContext->color, _symmetryContext);
                        
                i.y--;
                dl.y -= twoRx2;

                if (dp > 0) {
                    dp += radius2.x - dl.y;
                } else {
                    i.x++;
                    dl.x += twoRy2;
                    dp += radius2.x - dl.y + dl.x;
                }
            }
            // drawell(start, outter, inners, innere);
        }

        if(m < linelength){
            int ty = std::max(outter.y, inner.y);
            for(int tx = outter.x; tx >= inner.x; tx--){
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + tx, pixel.y + ty, 0xffaaaaff, _symmetryContext);
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - tx, pixel.y + ty, 0xffaaaaff, _symmetryContext);
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + tx, pixel.y - ty, 0xffaaaaff, _symmetryContext);
                _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - tx, pixel.y - ty, 0xffaaaaff, _symmetryContext);
            }
            
            _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + outter.x, pixel.y + outter.y, 0xff00ffff, _symmetryContext);
            _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - outter.x, pixel.y + outter.y, 0xff00ffff, _symmetryContext);
            _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + outter.x, pixel.y - outter.y, 0xff00ffff, _symmetryContext);
            _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - outter.x, pixel.y - outter.y, 0xff00ffff, _symmetryContext);
            
            _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + inner.x, pixel.y + inner.y, 0xff00ffff, _symmetryContext);
            _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - inner.x, pixel.y + inner.y, 0xff00ffff, _symmetryContext);
            _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x + inner.x, pixel.y - inner.y, 0xff00ffff, _symmetryContext);
            _toolRuntimeContext.drawingSession->putMirroredPixel(pixel.x - inner.x, pixel.y - inner.y, 0xff00ffff, _symmetryContext);
        }


        prevYOuter = outter.y;
        prevYInner = inner.y;
        prevXOuter = outter.x;
        prevXInner = inner.x;


        outter.y--;
        delta.y -= twoRx2;
        acc++;

        if (decisionParam > 0) {
            decisionParam += radius2.x - delta.y;
        } else {
            outter.x++;
            delta.x += twoRy2;
            decisionParam += radius2.x - delta.y + delta.x;
            acc+=0.41421356f;
        }

        if(inner.y < 0) continue;
        inner.y--;
        deltaInner.y -= twoRIx2;
        
        if (dpInner > 0) {
            dpInner += radiusInner2.x - deltaInner.y;
        } else {
            inner.x++;
            deltaInner.x += twoRIy2;
            dpInner += radiusInner2.x - deltaInner.y + deltaInner.x;
        }
    }

    
    if(m < linelength-1 && gap >0&& false){
        Point inners = cinner(start);
        Point innere = cinner(outter);
        drawell(start, outter, inners, innere);
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

/*
float dashAcc = 0.0f;
bool drawingDash = false;

Point startOuter;
Point startInner;
Point endOuter;
Point endInner;

Point prev = outter;

while(delta.x < delta.y)
{
    // distância incremental do Bresenham
    float step = 0.0f;

    int dx = outter.x - prev.x;
    int dy = outter.y - prev.y;

    if (dx || dy)
        step = (dx && dy) ? 1.41421356f : 1.0f;

    float oldAcc = dashAcc;
    dashAcc += step;


    // calcula normal da elipse neste ponto
    float nx = outter.x / float(radius.x * radius.x);
    float ny = outter.y / float(radius.y * radius.y);

    float nlen = sqrt(nx*nx + ny*ny);

    nx /= nlen;
    ny /= nlen;

    float half = _drawingContext->size * 0.5f;


    Point outerPoint =
    {
        (int)round(outter.x + nx * half),
        (int)round(outter.y + ny * half)
    };

    Point innerPoint =
    {
        (int)round(outter.x - nx * half),
        (int)round(outter.y - ny * half)
    };


    float phase = fmod(oldAcc, patternLength);


    // entrou no dash
    if (!drawingDash && phase < linelength)
    {
        drawingDash = true;

        startOuter = outerPoint;
        startInner = innerPoint;
    }


    // continua dentro do dash
    if (drawingDash)
    {
        endOuter = outerPoint;
        endInner = innerPoint;
    }


    // saiu do dash
    if (drawingDash && fmod(dashAcc, patternLength) >= linelength)
    {
        drawingDash = false;

        fillQuad(
            startOuter,
            endOuter,
            endInner,
            startInner
        );
    }


    prev = outter;


    // ===== seu código original do Midpoint =====

    outter.x++;
    delta.x += twoRx2;

    if (decisionParam < 0)
    {
        decisionParam += radius2.y + delta.x;
    }
    else
    {
        outter.y--;
        delta.y -= twoRx2;
        decisionParam += radius2.y + delta.x - delta.y;
    }
}
*/