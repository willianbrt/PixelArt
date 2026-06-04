#ifndef VIEWPORTCONTEXT_H
#define VIEWPORTCONTEXT_H

#include "../CursorContext/CursorContext.h"

class ViewportContext {
private:
    
public:
    ViewportContext();
    ~ViewportContext();
    int width, height;
    int cursorX, cursorY;
};
#endif