#ifndef EDITORDTO_H
#define EDITORDTO_H

#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <vector>

struct EditorDTO{
    std::string id;
    std::string activeFrameId;
    int width;
    int height;
};

#endif