#ifndef EDITORDTO_H
#define EDITORDTO_H

#include "../FrameDTO/FrameDTO.h"

#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <vector>

struct EditorDTO{
    std::vector<FrameDTO> frames;
    FrameDTO activeFrame;
    int width;
    int height;
    int n_tiles_x;
    int n_tiles_y;
    emscripten::val buffer;
};

#endif