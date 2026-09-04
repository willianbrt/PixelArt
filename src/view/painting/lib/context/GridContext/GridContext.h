#ifndef GRIDCONTEXT_H
#define GRIDCONTEXT_H
#include "../../interfaces/IToolContext/IToolContext.h"

class GridContext : public IToolContext{
public:
    bool enableGridX = false;
    bool enableGridY = false;
    int divisionsX = 1;
    int divisionsY = 1;
};

#endif
