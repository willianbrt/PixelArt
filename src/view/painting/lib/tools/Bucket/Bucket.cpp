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

    // bounds.end = _point;
    // bounds.start = _point;
    
    const unsigned int _width = 32;
    const unsigned int _height = 32;
    while(queue.size() != 0) {
        Point point = queue.back();
        queue.pop_back();
        const unsigned int currentColorHEX = _layer.getPixel(point.x, point.y);

        const bool isItOutsideTheAxisX = point.x < 0 || point.x > _width;
        const bool isItOutsideTheAxisY =  point.y < 0 || point.y > _height;

        if(isItOutsideTheAxisX || isItOutsideTheAxisY || currentColorHEX != startColorHEX)
            continue;

        // modifyPixel(layer, point, _newColorHex);
        _layer.putPixel(point.x,point.y, _newColorHex);
        queue.push_back(Point(point.x+1, point.y));
        queue.push_back(Point(point.x-1, point.y));
        queue.push_back(Point(point.x,   point.y+1));
        queue.push_back(Point(point.x,   point.y-1));
    }
}


using namespace emscripten;

EMSCRIPTEN_BINDINGS(bucket_module){
    // register_vector<Pixel>("VectorPixel");
    // emscripten::register_vector<float>("VectorFloat");
    // emscripten::register_vector<std::vector<float>>("VectorVectorFloat");

    class_<Bucket>("Bucket")
        .constructor<Layer&, unsigned int,unsigned int,unsigned int>()
        .smart_ptr<std::shared_ptr<Bucket>>("shared_ptr<Bucket>")
        .function("draw", &Bucket::draw);
};
