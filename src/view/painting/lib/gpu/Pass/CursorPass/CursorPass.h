#ifndef CURSORPASS_H
#define CURSORPASS_H

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

#include "../../../interfaces/IRenderPass/IRenderPass.h"


#include "../../../objects/Editor/Editor.h"
#include "../../../graphics/surface/Surface.h"
#include "../../../graphics/surface/Surface.h"
#include "../../../objects/Viewport/Viewport.h"
#include "../../../context/CursorContext/CursorContext.h"
#include "../../Quad/Quad.h"
#include "../../Shader/Shader.h"

class CursorPass : public IRenderPass {
private:
    Surface* surface;
    Viewport* viewport;
    CursorContext* hover;
    Editor* editor;
    
    GLuint programHover;
    GLuint canvasCursorHover;
    GLuint texture;
    GLint cursorLocation;
    GLint brushSizeLocation;
    GLint posH;
    GLint positionLocationH;
    GLint scaleLocationH;
    GLint texSizeLocationH;
    GLint resolutionLocationH;
    Quad quad;
    Shader shader;

    const char* fsHover =
    "precision highp float;"
    "uniform sampler2D tex;"
    "varying vec2 uv;"
    "uniform vec2 texSize;"
    "uniform vec2 cursor;"
    "uniform vec2 brushSize;"
    "uniform float zoom;"
    "uniform vec2 pan;"
    "uniform vec2 resolution;"
    "void main(){"
    "   vec2 pixel = (uv - pan) / zoom;"
    "   vec2 transformedUV = pixel / texSize;"
    "   if(transformedUV.x < 0.0 || transformedUV.x > 1.0 || transformedUV.y < 0.0 || transformedUV.y > 1.0) discard;"

    "   vec2 pixelHover = (pixel-cursor) + floor(brushSize*0.5);"
    "   vec2 hoverUV = pixelHover / brushSize;"
    "   if(hoverUV.x < 0.0 || hoverUV.x >= 1.0 || hoverUV.y < 0.0 || hoverUV.y >= 1.0) discard;"

    "   vec4 color = texture2D(tex, hoverUV).abgr;"
    "   gl_FragColor = color;"
    "}";
public:
    CursorPass();
    ~CursorPass();

    void init() override;
    void draw() override;
    void upload(Bounding area) override;
};
#endif