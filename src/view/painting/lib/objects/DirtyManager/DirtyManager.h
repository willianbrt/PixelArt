#ifndef DIRTYMANAGER_H
#define DIRTYMANAGER_H

#include "../../graphics/Pixel/Pixel.h"

class DirtyManager {
private:
    Bounding _dirty;
    bool _hasDirty;
public:
    DirtyManager();
    ~DirtyManager();

    void markDirty(Bounding dirtyArea);
    Bounding dirty();
    bool hasDirty();
};
#endif