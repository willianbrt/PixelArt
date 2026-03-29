
#ifndef RENDERER_H
#define RENDERER_H

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
// #include "../../objects/Editor/Editor.h"
#include "../../graphics/surface/Surface.h"
// #include "../../objects/CanvasSettings/CanvasSettings.h"
#include "../../objects/Viewport/Viewport.h"
#include "../../context/BrushContext/BrushContext.h"
#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"

class Renderer
{
public:
    Renderer();

    void init(Surface* surface, Viewport* viewport);
    void initCursorHover(HoverPreview* hover);

    void uploadSurface(Bounding area, Surface* surface);
    void uploadCursorHover(HoverPreview* hover);

    void draw(Surface* surface, Viewport* viewport);
    void drawCursorHover(Surface* surface, HoverPreview* hover, Viewport* viewport);

private:
    int _height, _width;
    GLuint program;
    GLuint vbo;
    
    GLuint programHover;
    GLuint canvasCursorHover;
    GLuint texLocationH;
    GLint cursorLocation;
    GLint brushSizeLocation;
    GLint posH;
    GLint positionLocationH;
    GLint scaleLocationH;
    GLint texSizeLocationH;
    GLint resolutionLocationH;

    GLuint canvasTexture;
    GLuint texLocation;
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
    GLuint compile(GLenum type,const char* src);
};
#endif