#ifndef FRAMEDTO_H
#define FRAMEDTO_H

#include "../LayerDTO/LayerDTO.h"

#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <vector>

struct FrameDTO{
    unsigned int timeDuration;
    std::vector<LayerDTO> layers;
    bool isActive;
    std::string id;
    emscripten::val buffer;
    int width;
    int height;
};

#endif