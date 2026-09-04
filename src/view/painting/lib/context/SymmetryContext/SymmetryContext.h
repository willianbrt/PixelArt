#ifndef SYMETRYCONTEXT_H
#define SYMETRYCONTEXT_H
#include "../../interfaces/IToolContext/IToolContext.h"

class SymmetryContext : public IToolContext{
public:
    bool isMirrorX = false;
    bool isMirrorY = false;

    int pointMirrored(float point, float comprimento);
    int pointMirrored(int point, int center, int comprimento);
};

#endif
