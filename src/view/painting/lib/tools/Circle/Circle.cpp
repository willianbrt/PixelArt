#include "Circle.h"


Circle::Circle(
    int from_x, int from_y,
    int to_x, int to_y,
    bool isFilled, int thickness,
    unsigned int newColorHex,
    bool isMirrorX, bool isMirrorY, 
    int nRows, int nCols) : IGraphic(isMirrorX, isMirrorY, nRows, nCols)
{
    _fromPoint = Point(from_x,from_y);
    _toPoint = Point(to_x,to_y);

    if(_toPoint.x < _fromPoint.x) std::swap(_fromPoint.x, _toPoint.x);
    if(_toPoint.y < _fromPoint.y) std::swap(_fromPoint.y, _toPoint.y);

    _newColorHex = newColorHex;
    _thickness = thickness-1;
    _isFilled = isFilled;

    cx = (_toPoint.x + _fromPoint.x) / 2.0f + 0.5f;
    cy = (_toPoint.y + _fromPoint.y) / 2.0f + 0.5f;
}
void Circle::draw(Layer& layer) {
    const int screenWidth = layer.getWidth()*_nRows;
    const int screenHeight = layer.getHeight()*_nCols;
    
// EXTERNO
    const int rx = ((_toPoint.x - _fromPoint.x) >> 1);
    const int ry = ((_toPoint.y - _fromPoint.y) >> 1);

    const int rx2 = rx * rx;
    const int ry2 = ry * ry;
    const int twoRx2 = rx2 << 1;
    const int twoRy2 = ry2 << 1;

    int x = 0;
    int y = ry;

    int p = ry2 - (rx2 * ry) + (rx2 >> 2);

    int px = 0;
    int py = twoRx2 * y;

// INTERNO
    const int rix = std::max(rx - _thickness, 0);
    const int riy = std::max(ry - _thickness, 0);

    const int rix2 = rix * rix;
    const int riy2 = riy * riy;
    const int twoRix2 = rix2 << 1;
    const int twoRiy2 = riy2 << 1;

    int pi = riy2 - (rix2 * riy) + (rix2 >> 2);

    int xi = 0;
    int yi = riy;

    int pix = 0;
    int piy = twoRix2 * yi;
    
    while(px < py){
        traceSymetricOutline(layer, x, y, xi, yi, _newColorHex, _thickness, screenWidth, screenHeight);
        if(_isFilled) traceFilled(layer, xi, yi, _newColorHex, screenWidth, screenHeight);
        
        x++;
        px += twoRy2;

        if (p < 0) {
            p += ry2 + px;
        } else {
            y--;
            py -= twoRx2;
            p += ry2 + px - py;
        }

        if(pix >= piy) continue;

        xi++;

        pix += twoRiy2;
        if (pi < 0) {
            pi += riy2 + pix;
        } else {
            yi--;
            piy -= twoRix2;
            pi += riy2 + pix - piy;
        }

    }
    
    p  = ry2 * x*x + rx2 * y*(y  - 1) + (rx2  >> 2) - rx2*ry2;
    pi = riy2 * xi*xi + rix2 * yi*(yi - 1) + (rix2 >> 2) - rix2*riy2;

    while (y >= 0){
        traceSymetricOutline(layer, x, y, xi, yi, _newColorHex, _thickness, screenWidth, screenHeight);
        if(_isFilled) traceFilled(layer, xi, yi, _newColorHex, screenWidth, screenHeight);

        y--;
        py -= twoRx2;

        if (p > 0) {
            p += rx2 - py;
        } else {
            x++;
            px += twoRy2;
            p += rx2 - py + px;
        }

        if(yi <= 0) continue;

        yi--;
        piy -= twoRix2;

        if (pi > 0) {
            pi += rix2 - piy;
        } else {
            xi++;
            pix += twoRiy2;
            pi += rix2 - piy + pix;
        }
    }
}


void Circle::traceFilled(Layer& layer, int x, int y, unsigned int color, int screenWidth, int screenHeight) {
    if(x<=0) return;
    for (int xi = std::floor(cx - x + 1) ; xi < cx + x; xi++) {
        putPixel(layer, xi, std::floor(cy + y), color, screenWidth, screenHeight);
        putPixel(layer, xi, std::floor(cy - y), color, screenWidth, screenHeight);
    }
}

void Circle::traceSymetricOutline(Layer& layer,
    int toX, int toY,
    int fromX, int fromY,
    unsigned int color, int thickness, 
    int screenWidth, int screenHeight) {

    if (fromX > toX) std::swap(fromX, toX);
    if (fromY > toY) std::swap(fromY, toY);

    for(int y = fromY; y <= toY; y++){
        for(int x = fromX; x <= toX; x++){
            putPixel(layer, std::floor(cx + fromX), std::floor(cy + fromY), color, screenWidth, screenHeight);
            putPixel(layer, std::floor(cx - fromX), std::floor(cy + fromY), color, screenWidth, screenHeight);
            putPixel(layer, std::floor(cx + fromX), std::floor(cy - fromY), color, screenWidth, screenHeight);
            putPixel(layer, std::floor(cx - fromX), std::floor(cy - fromY), color, screenWidth, screenHeight);
        }
    }
}
void Circle::putPixel(Layer& layer, int x, int y, unsigned int color, int screenWidth, int screenHeight){
    if(x >= screenWidth || y >= screenHeight || x < 0 || y < 0) return;

    Point p;
    p.x = GraphicsEngine::clampedTilePoint(x, layer.getWidth());
    p.y = GraphicsEngine::clampedTilePoint(y, layer.getHeight());

    layer.putPixel(p.x, p.y, color);
    
    int pointMirrorX = GraphicsEngine::pointMirrored(p.x, layer.getWidth());
    int pointMirrorY = GraphicsEngine::pointMirrored(p.y, layer.getHeight());

    if(_isMirrorX){
        layer.putPixel(pointMirrorX, p.y, color);
    }            
    if(_isMirrorY){
        layer.putPixel(p.x, pointMirrorY, color);
    }
    if(_isMirrorX && _isMirrorY){
        layer.putPixel(pointMirrorX, pointMirrorY, color);
    }
}

using namespace emscripten;

EMSCRIPTEN_BINDINGS(circle_module){
    class_<Circle, base<IGraphic>>("Circle")
        .constructor<int, int, int, int, bool,int, unsigned int, bool, bool, int, int>()
        .smart_ptr<std::shared_ptr<Circle>>("shared_ptr<Circle>")
        .function("draw", &Circle::draw);
};
