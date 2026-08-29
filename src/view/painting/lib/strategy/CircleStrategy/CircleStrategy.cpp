#include "./CircleStrategy.h"

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
    _drawingContext->size = 3;
    _drawingContext->hardness = 1.0f;

    _symmetryContext->isMirrorX =false;
    _symmetryContext->isMirrorY =false;
    _circleContext->isFilled = false;
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

    _toolRuntimeContext.drawingSession->clear();


    Point center = {
        (pivotPoint.x + to.x ) >> 1,
        (pivotPoint.y + to.y ) >> 1
    };
    
    const Point diameter = { 
        std::abs(to.x - pivotPoint.x), 
        std::abs(to.y - pivotPoint.y)
        // std::max(std::abs(to.x - pivotPoint.x), _drawingContext->size-1), 
        // std::max(std::abs(to.y - pivotPoint.y), _drawingContext->size-1)
    };
    const int offsetX = diameter.x %2;
    const int offsetY = diameter.y %2;
    
    const Point radius = { 
        diameter.x >> 1, 
        diameter.y >> 1
    };
    CircleRasterize tcircle(diameter);
    CircleRasterize circle(radius);
    CircleRasterize icircle({ 
       ( radius.x - (_drawingContext->size-1)), 
       ( radius.y - (_drawingContext->size-1))
    });

    
    // unsigned int color = 0xFF0000aa;
    const int gap = 3;
    const int linelength = 3;
    const int patternLength = gap + linelength;

    // auto draw4 = [&](Point p){
    //     int x = p.x; int y = p.y;
    //     _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x + x, center.y + y, color, _symmetryContext);
    //     _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x + x, center.y - y, color, _symmetryContext);
    //     _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x - x, center.y + y, color, _symmetryContext);
    //     _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x - x, center.y - y, color, _symmetryContext);
    // };

    auto draw4 = [&](Point p, unsigned int color){

        _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x + p.x, center.y + p.y, color, _symmetryContext);

        // int x = p.x; int y = p.y;
        // if(!(x == 0 && offsetX == 0)){
        //     if(!(y == 0 && offsetY == 0))
        //         _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x + x + offsetX, center.y + y + offsetY, color, _symmetryContext);
        //     _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x + x + offsetX, center.y - y, color, _symmetryContext);
        // }
        // if(!(y == 0 && offsetY == 0)){
        //     _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x - x, center.y + y + offsetY, color, _symmetryContext);
        // }
        // _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x - x, center.y - y, color, _symmetryContext);

    };

    tcircle.draw(draw4);

/*
    Point flag ={INT_MIN,INT_MIN};
    Point fstart, fend;
    while(circle.hasNext()){
        Point p = circle.next();
        Point ip = icircle.next();
                
        Point externo = {
            p.x,
            p.y,
        },
        interno = {
            (p.x < _drawingContext->size) ? 0 : externo.x-(_drawingContext->size-1),
            (p.y < _drawingContext->size) ? 0 : externo.y-(_drawingContext->size-1),
        };

        if(_circleContext->isFilled){
            if(flag.x != p.x) {
                for(int y = 0; y <= p.y; y++){
                    if(!(p.x == 0 && offsetX == 0)){
                        if(!(y == 0 && offsetY == 0))
                            _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x + p.x + offsetX, center.y + y + offsetY, color, _symmetryContext);
                        
                        _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x + p.x + offsetX, center.y - y, color, _symmetryContext);
                    }
                    if(!(y == 0 && offsetY == 0)){
                        _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x - p.x, center.y + y + offsetY, color, _symmetryContext);
                    }
                    _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x - p.x, center.y - y, color, _symmetryContext);
                }
            }
        } else {
            auto draw4 = [&](int x, int y){
                if(!(x == 0 && offsetX == 0)){
                    if(!(y == 0 && offsetY == 0))
                        _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x + x + offsetX, center.y + y + offsetY, color, _symmetryContext);
                    _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x + x + offsetX, center.y - y, color, _symmetryContext);
                }
                if(!(y == 0 && offsetY == 0)){
                    _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x - x, center.y + y + offsetY, color, _symmetryContext);
                }
                _toolRuntimeContext.drawingSession->blendMirroredPixel(center.x - x, center.y - y, color, _symmetryContext);
            };

            // draw4(ip.x, ip.y);
            // draw4(interno.x, interno.y);
            if(flag.x != p.x){
                for(int y = ip.y; y <= p.y; y++){
                    // if((externo.x <= fend.x && externo.x >= fstart.x) && (y < fend.y && y >= fstart.y)) continue;
                    
                    draw4(p.x, y);
                }
            }
            if(flag.y != ip.y){
                for(int x = ip.x; x <= p.x; x++){
                    // if((x <= fend.x && x >= fstart.x) && (interno.y < fend.y && interno.y >= fstart.y)) continue;
                    draw4(x, ip.y);
                }
            }
            // for(int y = interno.y; y <= externo.y; y++){
            //     if((externo.x <= fend.x && externo.x >= fstart.x) && (y < fend.y && y >= fstart.y)) continue;
                
            //     draw4(externo.x, y);
            // }
            // for(int x = interno.x; x < externo.x; x++){
            //     if((x <= fend.x && x >= fstart.x) && (interno.y < fend.y && interno.y >= fstart.y)) continue;
            //     draw4(x, interno.y);
            // }
        }
        fstart = ip;
        fend = p;
        // fstart = interno;
        // fend = externo;

        flag = p;
    }
            
    
*/
    _flagPoint = to;
}
void CircleStrategy::onRelease(){ done(); }

void CircleStrategy::draw(const Point& pixel){
    _toolRuntimeContext.drawingSession->clear();

    auto plot = [&](int sx, int sy, unsigned int color){
        _toolRuntimeContext.drawingSession->blendMirroredPixel(pixel.x  +  sx, pixel.y + sy, color, _symmetryContext);
        _toolRuntimeContext.drawingSession->blendMirroredPixel(pixel.x  -  sx, pixel.y + sy, color, _symmetryContext);
        _toolRuntimeContext.drawingSession->blendMirroredPixel(pixel.x  +  sx, pixel.y - sy, color, _symmetryContext);
        _toolRuntimeContext.drawingSession->blendMirroredPixel(pixel.x  -  sx, pixel.y - sy, color, _symmetryContext);
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
    Point start = outter, sinner = inner;

    Point dl,dli;
    float dp,dpi;
    while(delta.x < delta.y){        
        m = fmod(acc, patternLength);

        if(m==0.0f){
            start = outter;
            dl = delta;
            dp = decisionParam;

            sinner = inner;
            dli = deltaInner;
            dpi = dpInner;
        }
        if(m < linelength && false){
            int tx =  std::max(outter.x, inner.x);
            for(int ty = outter.y; ty >= inner.y; ty--){
                plot(tx, ty, 0xffaa00dd);
            }

            traceSymetricOutline(inner, outter, pixel);       
        }


        if(m == (linelength-1)){
            Point i = start, ii = sinner;
            while(i.x <= outter.x){
                
                // int tx =  std::max(i.x, ii.x);
                // for(int ty = i.y; ty >= ii.y; ty--){
                    
                int minX = std::min(i.x, ii.x);
                int minY = std::min(i.y, ii.y);
                int maxX = std::max(i.x, ii.x);
                int maxY = std::max(i.y, ii.y);
                for(int ty = minY; ty <= maxY; ty++){
                    for(int tx = minX; tx <= maxX; tx++){
                        Point p = {tx, ty};
                        // int cs = std::abs(cross({0,0}, start,p));
                        // int ce = std::abs(cross({0,0}, outter,p));
                        // if(ce > 1 || cs < 1) continue;


                        // if (!insideSector(p, start, outter)) continue;
                        int cs = cross(sinner, start,p);
                        int ce = cross(inner, outter,p);
                        if(ce < 0 || cs > 0) continue;

                        plot(tx, ty, 0xffaa00dd);
                    }
                }

                // traceSymetricOutline(ii, i, pixel);
                
                i.x++;
                dl.x += twoRy2;
                
                if (dp < 0) {
                    dp += radius2.y + dl.x;
                } else {
                    i.y--;
                    dl.y -= twoRx2;
                    dp += radius2.y + dl.x - dl.y;
                }
                ii.x++;
                dli.x += twoRIy2;

                if(dli.x >= dli.y) continue;
                
                if (dpi < 0) {
                    dpi += radiusInner2.y + dli.x;
                } else {
                    ii.y--;
                    dli.y -= twoRIx2;
                    dpi += radiusInner2.y + dli.x - dli.y;
                }
            }
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
    // return;
    
    decisionParam  = radius2.y * outter.x*outter.x + radius2.x * outter.y*(outter.y  - 1) + (radius2.x  >> 2) - radius2.x*radius2.y;
    dpInner  = radiusInner2.y * inner.x*inner.x + radiusInner2.x * inner.y*(inner.y  - 1) + (radiusInner2.x  >> 2) - radiusInner2.x*radiusInner2.y;

    while (outter.y >= 0){
        m = fmod(acc, patternLength);


        if(m==0.0f){
            start = outter;
            dl = delta;
            dp = decisionParam;

            sinner = inner;
            dli = deltaInner;
            dpi = dpInner;
        }

        if(m == (linelength-1)){
            Point i = start, ii =sinner;
            while(i.y >= outter.y){
                int minX = std::min(i.x, ii.x);
                int minY = std::min(i.y, ii.y);
                int maxX = std::max(i.x, ii.x);
                int maxY = std::max(i.y, ii.y);
                for(int ty = minY; ty <= maxY; ty++){
                    for(int tx = minX; tx <= maxX; tx++){
                        Point p = {tx,ty};
                        int cs = cross(sinner, start,p);
                        int ce = cross(inner, outter,p);
                        if(ce < 0 || cs > 0) continue;

                        plot(tx, ty, 0xffaa00dd);
                    }
                }
                // traceSymetricOutline(ii, i, pixel);
                
                i.y--;
                dl.y -= twoRx2;

                if (dp > 0) {
                    dp += radius2.x - dl.y;
                } else {
                    i.x++;
                    dl.x += twoRy2;
                    dp += radius2.x - dl.y + dl.x;
                }

                ii.y--;
                dli.y -= twoRIx2;

                if (dpi > 0) {
                    dpi += radiusInner2.x - dli.y;
                } else {
                    ii.x++;
                    dli.x += twoRIy2;
                    dpi += radiusInner2.x - dli.y + dli.x;
                }
            }
        }

        if(m < linelength && false){

            int minX = std::min(outter.x, inner.x);
            int minY = std::min(outter.y, inner.y);
            int maxX = std::max(outter.x, inner.x);
            int maxY = std::max(outter.y, inner.y);
            for(int ty = minY; ty <= maxY; ty++){
                for(int tx = minX; tx <= maxX; tx++){
                    plot(tx, ty, 0xffaa00dd);
                }
            }
        }


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

    
    if(m < linelength-1){
        Point i = start, ii =sinner;
        while(i.y >= outter.y){
            int minX = std::min(i.x, ii.x);
            int minY = std::min(i.y, ii.y);
            int maxX = std::max(i.x, ii.x);
            int maxY = std::max(i.y, ii.y);
            for(int ty = minY; ty <= maxY; ty++){
                for(int tx = minX; tx <= maxX; tx++){
                    Point p = {tx,ty};
                    int cs = cross(sinner, start,p);
                    int ce = cross(inner, outter,p);
                    if(ce < 0 || cs > 0) continue;

                    plot(tx, ty, 0xffaa00dd);
                }
            }
            // traceSymetricOutline(ii, i, pixel);
            
            i.y--;
            dl.y -= twoRx2;

            if (dp > 0) {
                dp += radius2.x - dl.y;
            } else {
                i.x++;
                dl.x += twoRy2;
                dp += radius2.x - dl.y + dl.x;
            }

            ii.y--;
            dli.y -= twoRIx2;

            if (dpi > 0) {
                dpi += radiusInner2.x - dli.y;
            } else {
                ii.x++;
                dli.x += twoRIy2;
                dpi += radiusInner2.x - dli.y + dli.x;
            }
        }
    }
}

void CircleStrategy::traceSymetricOutline(const Point& from, const Point& to, const Point& center) {
    
    for(int y = from.y; y <= to.y; y++){
        for(int x = from.x; x <= to.x; x++){
            
            // Point p = {tx,ty};
            // int cs = cross({0,0}, start,p);
            // int ce = cross({0,0}, outter,p);
            // if(ce < 0 || cs > 0) continue;

            // _toolRuntimeContext.drawingSession->putMirroredPixel(center.x + x, center.y + y,  _drawingContext->color, _symmetryContext);
            // _toolRuntimeContext.drawingSession->putMirroredPixel(center.x - x, center.y + y,  _drawingContext->color, _symmetryContext);
            // _toolRuntimeContext.drawingSession->putMirroredPixel(center.x + x, center.y - y,  _drawingContext->color, _symmetryContext);
            // _toolRuntimeContext.drawingSession->putMirroredPixel(center.x - x, center.y - y,  _drawingContext->color, _symmetryContext);
        }
    }

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