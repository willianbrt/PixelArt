#ifndef SHADER_H
#define SHADER_H

#include <cstdio>
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

class Shader {
private:
    GLuint _program = 0;
    static const  char* vertex_data;
    static GLuint v;

    GLuint compile(GLenum type, const char* src);
public:
Shader();
~Shader();
    void create(const char* vs, const char* fs);
    void create(const char* fs);
    void use();

    GLuint id();

    // uniforms básicos (expanda conforme precisar)
    void setInt(const char* name, int v);
    void setFloat(const char* name, float v);
    void setVec2(const char* name, float x, float y);
};
#endif