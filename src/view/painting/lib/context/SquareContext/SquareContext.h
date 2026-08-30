#ifndef SQUARECONTEXT_H
#define SQUARECONTEXT_H
#include "../../interfaces/IToolContext/IToolContext.h"

class SquareContext : IToolContext{
public:
    bool isFilled;
    
    int gap = 0;
    int linelength = 3;
    int patternLength = gap + linelength;
};

#endif
