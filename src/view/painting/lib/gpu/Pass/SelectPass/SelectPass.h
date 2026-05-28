#ifndef SELECTPASS_H
#define SELECTPASS_H

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

class SelectPass : public IRenderPass {
private:
    Surface* surface;
    Viewport* viewport;
    CursorContext* hover;
    Editor* editor;
    
    GLuint programSelect;
    GLuint canvasSelect;
    GLuint texture;
    GLint posSelect;
    GLint positionLocationSelect;
    GLint cursorWorldLocation;
    GLint scaleLocationSelect;
    GLint texSizeLocationSelect;
    GLint resolutionLocationSelect;
    GLint selectSizeLocation;
    bool initializedSelect;
    GLint cursorLocation;
    Quad quad;
    Shader shader;


    const char* fsSelect =
    "precision highp float;"
    "uniform sampler2D tex;"
    "varying vec2 uv;"
    "uniform vec2 texSize;"
    "uniform vec2 cursor;"
    "uniform vec2 cursorWorld;"
    "uniform vec2 selectSize;"
    "uniform vec2 resolution;"
    "uniform float zoom;"
    "uniform vec2 pan;"
    
    "uniform vec2 resizeHandle[4];"
    "uniform vec2 rotateHandle[4];"


    "uniform float time;"

    "float hasEdge(vec2 pixelUV){"
    "   float inside = step(0.0, pixelUV.x) * step(0.0, pixelUV.y) * step(pixelUV.x, 1.0) * step(pixelUV.y, 1.0);"
    "   return step(0.004, texture2D(tex, pixelUV).r)*inside;"
    "}"

    "vec2 hyp(vec2 point, vec2 center){"
    "   vec2 dir = center - point;"
    "   float dist = length(dir);"
    "   return vec2(dir / dist);"
    "}"

    "void drawCornerResize(vec2 pixel, float size, float thickness){"
    "   pixel = pixel - size * 0.5;"

    "   float inner = step(pixel.x, uv.x) * step(pixel.y, uv.y) * step(uv.x, pixel.x + size) * step(uv.y, pixel.y + size);"
    "   float outer = step(pixel.x + thickness, uv.x) * step(pixel.y + thickness, uv.y) * step(uv.x, pixel.x + size - thickness) * step(uv.y, pixel.y + size - thickness);"
    "   float border =  inner - outer;"
    
    "   float hover = step(pixel.x, cursorWorld.x) * step(pixel.y, cursorWorld.y) * step(cursorWorld.x, pixel.x + size) * step(cursorWorld.y, pixel.y + size);"

    "   vec4 background = mix(vec4(0.87,0.411,0.0,1.0), vec4(0.0, 1.0, 0.0,1.0), hover);"
    "   gl_FragColor = mix(gl_FragColor, background, inner);"
    "   gl_FragColor = mix(gl_FragColor, vec4(0.203,0.105,0.015,1.0), border);"
    "}"
    

    "void drawCornerRotate(vec2 pixel, float size, float thickness){"
    "   float radius = size*0.5;"
    "   float r2 = radius*radius;"

    "   float radiusOuter = (radius-thickness);"
    "   float ro2 = radiusOuter*radiusOuter;"
    
    "   float sumOuter = 0.0;"
    "   float sum = 0.0;"

    "    float blur = 0.15;"
    "    vec2 offsets[4];"
    "    offsets[0] = vec2(-blur, -blur);"
    "    offsets[1] = vec2( blur, -blur);"
    "    offsets[2] = vec2(-blur,  blur);"
    "    offsets[3] = vec2( blur,  blur);"
        
    "    vec2 dh = cursorWorld - pixel;"
    "    float hover = step(dot(dh,dh), ro2);"

    "    for(int i = 0; i < 4; i++){"
    "        vec2 d = (uv + offsets[i]) - pixel;"
    "        sum += step(dot(d,d), r2);"
    "        sumOuter += step(dot(d,d), ro2);"
    "    }"
    "   float inside = sum / 4.0;"
    "   float outside = sumOuter / 4.0;"
    "   float border = inside - outside;"
    
    "   vec4 background = mix(vec4(0.87,0.411,0.0,1.0), vec4(0.0, 1.0, 0.0,1.0), hover);"
    "   gl_FragColor = mix(gl_FragColor, background, inside);"
    "   gl_FragColor = mix(gl_FragColor, vec4(0.203,0.105,0.015,1.0), border);"
    "}"
    "void computeCorner(){"
    "   float sizeResize = zoom*0.5;"
    "   float sizeRotate = zoom*0.5;"
    "   float thinkness = 2.0;"


    "   for(int i = 0; i < 4; i++){"
    "       drawCornerResize(resizeHandle[i], sizeResize, thinkness);"
    "   }"

    "   for(int i = 0; i < 4; i++){"
    "       drawCornerRotate(rotateHandle[i], sizeRotate, thinkness);"
    "   }"

    "}"
    
    "void main(){"
    "   vec2 pixel = (uv - pan) / zoom;"
    "   vec2 transformedUV = pixel / texSize;"

    "   vec2 lineSize = 1.0/(zoom*selectSize);"

    "   float mask = hasEdge(transformedUV);"
    "   gl_FragColor = mix(vec4(0.0), vec4(0.32,0.21,0.36,0.5), mask);"

    "   vec2 px = vec2(lineSize.x, 0.0);"
    "   vec2 py = vec2(0.0, lineSize.y);"

    "   float left  = hasEdge(transformedUV - px);"
    "   float down  = hasEdge(transformedUV - py);"

    "   float edge =  abs(mask - down) + abs(mask - left);"
    "   edge = clamp(edge, 0.0, 1.0);"

    "   float coord = uv.x + uv.y;"
    "   float speed = 1.0;"
    "   float pattern = step(0.5, fract(coord / zoom + time * speed));"
    "   vec3 antsColor = mix(vec3(1.0), vec3(0.0), pattern);"

    "   gl_FragColor = mix(gl_FragColor, vec4(antsColor, edge), edge);"

    "   computeCorner();"
    "}";
public:
    SelectPass();
    ~SelectPass();

    void init() override;
    void draw() override;
    void upload(Bounding area) override;
};
#endif