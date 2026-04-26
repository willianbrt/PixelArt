#ifndef SYMETRYCONTEXT_H
#define SYMETRYCONTEXT_H
#include "../../interfaces/IToolContext/IToolContext.h"

class SymmetryContext : public IToolContext{
public:
    bool isMirrorX;
    bool isMirrorY;
    int nTileX = 1;
    int nTileY = 1;
};

#endif
