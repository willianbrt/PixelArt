#ifndef SHAPECONTEXT_H
#define SHAPECONTEXT_H
#include "../../interfaces/IToolContext/IToolContext.h"

class CircleContext : IToolContext{
public:
    bool isFilled;
    
    int gap = 0;
    int linelength = 3;
    int patternLength = gap + linelength;
};

#endif
