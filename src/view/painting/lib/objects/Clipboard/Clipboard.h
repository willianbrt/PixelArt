#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include "../../graphics/surface/Surface.h"
#include "../../context/SelectContext/SelectContext.h"

class Clipboard{
public:
    Clipboard();
    Surface* copy(SelectContext* select);
    void paste(Surface* surface, SelectContext* select);
private:
    
};
#endif