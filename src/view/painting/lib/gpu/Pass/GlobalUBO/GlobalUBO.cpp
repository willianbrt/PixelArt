#include "./GlobalUBO.h"

GlobalUBO::GlobalUBO(){}
GlobalUBO::~GlobalUBO(){}

GLuint GlobalUBO::id(){
return globalUBO;
}
void GlobalUBO::create(){

    glGenBuffers(1, &globalUBO);

    glBindBuffer(GL_UNIFORM_BUFFER, globalUBO);

    glBufferData(GL_UNIFORM_BUFFER,  sizeof(GlobalData), nullptr, GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, globalUBO);
    
}