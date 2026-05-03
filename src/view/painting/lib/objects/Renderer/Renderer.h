
#ifndef RENDERER_H
#define RENDERER_H

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include "../../objects/Editor/Editor.h"
#include "../../graphics/surface/Surface.h"
// #include "../../objects/CanvasSettings/CanvasSettings.h"
#include "../../objects/Viewport/Viewport.h"
#include "../../context/CursorContext/CursorContext.h"

class Renderer
{
public:
    Renderer();

    void init(Surface* surface, Viewport* viewport);
    void initCursorHover(CursorContext* hover);
    void initSelect(Editor* editor);

    void uploadSurface(Bounding area, Surface* surface);
    void uploadCursorHover(CursorContext* hover);
    void uploadSelect(Editor* editor);

    void draw(Surface* surface, Viewport* viewport);
    void drawCursorHover(Surface* surface, CursorContext* hover, Viewport* viewport);
    void drawSelect(Surface* surface, Editor* editor, Viewport* viewport);

private:
    int _height, _width;
    GLuint program;
    GLuint vbo;
    
    GLuint programSelect;
    GLuint canvasSelect;
    GLuint texLocationSelect;
    GLint posSelect;
    GLint positionLocationSelect;
    GLint cursorWorldLocation;
    GLint scaleLocationSelect;
    GLint texSizeLocationSelect;
    GLint resolutionLocationSelect;
    GLint selectSizeLocation;
    bool initializedSelect;
    
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