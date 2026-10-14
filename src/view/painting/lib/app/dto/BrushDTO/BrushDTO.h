#ifndef BRUSHDTO_H
#define BRUSHDTO_H

#include <emscripten/val.h>
#include <emscripten/bind.h>

class BrushDTO {
private:
    
public:
    std::string id;
    emscripten::val buffer;
    int width;
    int height;
};
#endif