#ifndef SURFACEDTO_H
#define SURFACEDTO_H

#include <emscripten/val.h>
#include <emscripten/bind.h>

struct SurfaceDTO{
    int width;
    int height;
    emscripten::val buffer;
};

#endif