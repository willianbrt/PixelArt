#ifndef BRUSHDTO_H
#define BRUSHDTO_H

#include <emscripten/val.h>
#include <emscripten/bind.h>
#include "../SurfaceDTO/SurfaceDTO.h"

class BrushDTO {
private:
    
public:
    std::string id;
    SurfaceDTO pattern;
    SurfaceDTO shape;
    int fillMode;
    int alignMode;
    int r, g, b;
    unsigned int color;
    float hardness;
    float smoothness;
    int spacing;
    int size;
    float stabilizer;
    bool pixelPerfect;
};
#endif