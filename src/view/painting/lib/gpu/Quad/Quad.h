#ifndef QUAD_H
#define QUAD_H

#include "../Buffer/Buffer.h"

class Quad {
private:
    Buffer vbo;
    
public:
Quad();
~Quad();
void create();
void bind();
void draw();
};
#endif