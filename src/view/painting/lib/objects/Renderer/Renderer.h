
#ifndef RENDERER_H
#define RENDERER_H

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
// #include "../../objects/Editor/Editor.h"
#include "../../graphics/surface/Surface.h"
// #include "../../objects/CanvasSettings/CanvasSettings.h"
#include "../../objects/Viewport/Viewport.h"
#include "../../context/BrushContext/BrushContext.h"

class Renderer
{
public:
    Renderer();

    void render(Bounding area, Surface* surface, Viewport* viewport);
    void renderCursor(Pattern pattern, Viewport* viewport);

    void init(Surface* surface, Viewport* viewport);
    void initCursorHover(Pattern pattern, Viewport* viewport);

private:
    int _height, _width;
    GLuint program;
    GLuint vbo;

    GLuint canvasCursorHover;
    GLint cursorLocation;
    GLint brushSizeLocation;

    GLuint canvasTexture;
    GLint pos;
    GLint positionLocation;
    GLint scaleLocation;
    GLint resolutionLocation;
    GLint texSizeLocation;
    GLint repeatLocation;
    GLint gridDivisionsLocation;
    GLint lightColorLocation;
    GLint darkColorLocation;

    void createShader();
    void createQuad();

    void cache();
    void draw(Viewport* viewport);
    GLuint compile(GLenum type,const char* src);
    void uploadSurface(Bounding area, Surface* surface);
    void uploadCursorHover(Pattern pattern);
};
#endif