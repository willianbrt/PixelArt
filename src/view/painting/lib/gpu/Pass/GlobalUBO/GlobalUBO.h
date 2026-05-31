#ifndef GLOBALUBO_H
#define GLOBALUBO_H

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
struct GlobalData {
    float resolution[2];
    float cursor[2];
    float cursorLocation[2];
    float pan[2];
    float zoom[2];
    float repeat[2];
    float time;
    
    float padding[3]; // alinhamento std140
};

class GlobalUBO {
private:
GLuint globalUBO;
    
public:
GlobalUBO();
~GlobalUBO();
GLuint id();
void create();
};
#endif