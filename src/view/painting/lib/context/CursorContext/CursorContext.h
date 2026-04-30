#ifndef CURSORCONTEXT_H
#define CURSORCONTEXT_H
#include "../../context/BrushContext/BrushContext.h"

struct CursorContext {
    Pattern* pattern;
    std::string cursor;
    float scale = 1.0f;
    bool enable = true;
};

#endif