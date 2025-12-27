#include "Pixel.h"

#include <emscripten.h>
#include <emscripten/bind.h>


using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_module){
    value_object<Point>("Point")
        .field("x", &Point::x)
        .field("y", &Point::y);

    value_object<Bounding>("Bounding")
        .field("start", &Bounding::start)
        .field("end", &Bounding::end);

    value_object<Corners>("Corners")
        .field("topLeft", &Corners::topLeft)
        .field("bottomLeft", &Corners::bottomLeft)
        .field("topRight", &Corners::topRight)
        .field("bottomRight", &Corners::bottomRight);
}