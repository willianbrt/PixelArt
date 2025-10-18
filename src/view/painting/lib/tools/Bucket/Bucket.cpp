#include "Bucket.h"

Bucket::Bucket(Layer& layer, unsigned int x, unsigned int y, unsigned int newColorHex) : _layer(layer){
    _point = Point(x,y);
    _newColorHex = newColorHex;
}
void Bucket::draw(){
    const unsigned int startColorHEX = _layer.getPixel(_point.x, _point.y);

    if(startColorHEX == _newColorHex)
        return;

    vector<Point> queue;
    queue.push_back(_point);

    while(queue.size() != 0) {
        Point point = queue.back();
        queue.pop_back();
        const unsigned int currentColorHEX = _layer.getPixel(point.x, point.y);

        if(!_layer.isInsideSkecth(point.x, point.y) || currentColorHEX != startColorHEX)
            continue;

        _layer.putPixel(point.x, point.y, _newColorHex);
        queue.push_back(Point(point.x+1, point.y));
        queue.push_back(Point(point.x-1, point.y));
        queue.push_back(Point(point.x,   point.y+1));
        queue.push_back(Point(point.x,   point.y-1));
    }
}


using namespace emscripten;

EMSCRIPTEN_BINDINGS(bucket_module){
    class_<Bucket>("Bucket")
        .constructor<Layer&, unsigned int,unsigned int,unsigned int>()
        .smart_ptr<std::shared_ptr<Bucket>>("shared_ptr<Bucket>")
        .function("draw", &Bucket::draw);
};
