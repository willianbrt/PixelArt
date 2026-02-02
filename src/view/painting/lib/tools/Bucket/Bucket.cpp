#include "Bucket.h"

Bucket::Bucket(
    int x, int y, 
    unsigned int newColorHex,
    bool isMirrorX, bool isMirrorY, 
    int nRows, int nCols) : IGraphic(isMirrorX, isMirrorY, nRows, nCols)
{
    _point = Point(x,y);
    _newColorHex = newColorHex;
}
void Bucket::draw(Layer& layer){
    const int screenWidth = layer.getWidth()*_nRows;
    const int screenHeight = layer.getHeight()*_nCols;

    if(_point.x >= screenWidth || _point.y >= screenHeight || _point.x < 0 || _point.y < 0)
        return;
    
    Point clampedPoint;
    clampedPoint.x = GraphicsEngine::clampedTilePoint(_point.x, layer.getWidth());
    clampedPoint.y = GraphicsEngine::clampedTilePoint(_point.y, layer.getHeight());

    const unsigned int startColorHEX = layer.getPixel(clampedPoint.x, clampedPoint.y);

    if(startColorHEX == _newColorHex)
        return;

    vector<Point> queue;
    queue.push_back(_point);

    while(queue.size() != 0) {
        Point point = queue.back();
        queue.pop_back();
        
        if(point.x >= screenWidth || point.y >= screenHeight || point.x < 0 || point.y < 0) continue;

        clampedPoint.x = GraphicsEngine::clampedTilePoint(point.x, layer.getWidth());
        clampedPoint.y = GraphicsEngine::clampedTilePoint(point.y, layer.getHeight());

        if(layer.getPixel(clampedPoint.x, clampedPoint.y) != startColorHEX) continue;

        putPixel(layer, clampedPoint.x, clampedPoint.y, _newColorHex);

        queue.push_back(Point(point.x+1, point.y));
        queue.push_back(Point(point.x-1, point.y));
        queue.push_back(Point(point.x,   point.y+1));
        queue.push_back(Point(point.x,   point.y-1));
    }
}


void Bucket::putPixel(Layer& layer, int x, int y, unsigned int color){
    layer.putPixel(x, y, color);
    
    int pointMirrorX = GraphicsEngine::pointMirrored(x, layer.getWidth());
    int pointMirrorY = GraphicsEngine::pointMirrored(x, layer.getHeight());

    if(_isMirrorX){
        layer.putPixel(pointMirrorX, y, color);
    }            
    if(_isMirrorY){
        layer.putPixel(x, pointMirrorY, color);
    }
    if(_isMirrorX && _isMirrorY){
        layer.putPixel(pointMirrorX, pointMirrorY, color);
    }
}

using namespace emscripten;

EMSCRIPTEN_BINDINGS(bucket_module){
    class_<Bucket, base<IGraphic>>("Bucket")
        .constructor<int, int,unsigned int, bool, bool, int, int>()
        .smart_ptr<std::shared_ptr<Bucket>>("shared_ptr<Bucket>")
        .function("draw", &Bucket::draw);
};
