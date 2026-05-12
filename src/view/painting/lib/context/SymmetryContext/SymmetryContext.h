#ifndef SYMETRYCONTEXT_H
#define SYMETRYCONTEXT_H
#include "../../interfaces/IToolContext/IToolContext.h"

class SymmetryContext : public IToolContext{
public:
    bool isMirrorX = false;
    bool isMirrorY = false;
    int nTileX = 1;
    int nTileY = 1;

    int pointMirrored(float point, float comprimento);
    int pointMirrored(int point, int center, int comprimento);
};

#endif
