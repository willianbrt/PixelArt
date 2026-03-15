
#ifndef RENDERER_H
#define RENDERER_H

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
// #include "../../objects/Editor/Editor.h"
#include "../../graphics/surface/Surface.h"

class Renderer
{
public:
    Renderer(int width, int height);
    void render(Bounding area, Surface* surface);
    // void resize(int width, int height);
private:
    int _height, _width;
    GLuint program;
    GLuint vbo;
    GLuint texture;
    GLint resolutionLocation;
    GLint positionLocation;
    GLint scaleLocation;
    GLint texSizeLocation;
    GLint repeatLocation;

    void createTexture();
    void createShader();
    void createQuad();
    void draw();
    GLuint compile(GLenum type,const char* src);
    void uploadSurface(Bounding area, Surface* surface);
};
#endif