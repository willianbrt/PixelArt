
#ifndef RENDERER_H
#define RENDERER_H

#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>
// #include <GLFW/glfw3.h>
#include "../../graphics/surface/Surface.h"

class Renderer
{
public:
    Renderer(int width, int height);
    void render(Bounding area, Surface* surface);
private:
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context;
    double _screenWidth, _screenHeight;
    int _height, _width;
    GLuint program;
    GLuint vbo;
    GLuint texture;
    void createContext();
    void createTexture();
    void createShader();
    void createQuad();
    void draw();
    GLuint compile(GLenum type,const char* src);
    void uploadSurface(Bounding area, Surface* surface);
};
#endif