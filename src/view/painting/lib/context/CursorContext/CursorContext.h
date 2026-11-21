#ifndef CURSORCONTEXT_H
#define CURSORCONTEXT_H
#include "../../interfaces/ISurface/ISurface.h"
#include <string>

struct CursorContext {
    ISurface* pattern = nullptr;
    std::string cursor;
    float scale = 1.0f;
    bool enable = true;
};

#endif