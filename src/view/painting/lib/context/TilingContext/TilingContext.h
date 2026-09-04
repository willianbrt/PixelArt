#ifndef TILINGCONTEXT_H
#define TILINGCONTEXT_H
#include "../../interfaces/IToolContext/IToolContext.h"

class TilingContext : public IToolContext{
public:
    bool isTilingX = false;
    bool isTilingY = false;
    static const int N_TILE_X = 3;
    static const int N_TILE_Y = 3;
};

#endif
