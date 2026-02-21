#ifndef LAYERDTO_H
#define LAYERDTO_H

#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <vector>

struct LayerDTO{
    std::string id;
    bool isVisible;
    bool isLock;
    float opacity;
    std::string name;
    int width;
    int height;
    bool isActive;
    emscripten::val buffer;
};

#endif