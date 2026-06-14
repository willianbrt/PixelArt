#ifndef SKETCHPASS_H
#define SKETCHPASS_H

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include "../../../interfaces/IRenderPass/IRenderPass.h"

#include "../../../objects/Editor/Editor.h"
#include "../../../graphics/surface/Surface.h"

#include "../../../objects/EditorManager/EditorManager.h"
#include "../../../context/CursorContext/CursorContext.h"
#include "../../../context/ViewportContext/ViewportContext.h"
#include "../../Quad/Quad.h"
#include "../../Shader/Shader.h"
#include "../GlobalUBO/GlobalUBO.h"
#include "../../Texture/Texture.h"

class SketchPass : public IRenderPass {
private:
    Surface* _surface;
    ViewportContext* _viewport;
    CursorContext* hover;
    Editor* editor;
    EditorManager* _manager;

    GLuint program;
    GLint pos;
    GLint texSizeLocation;
    GLint gridDivisionsLocation;
    GLint lightColorLocation;
    GLint darkColorLocation;
    
    Texture texture;
    Quad quad;
    Shader shader;
    GlobalUBO globalUBO;

public:
    SketchPass();
    SketchPass(EditorManager* manager, ViewportContext* viewport);
    ~SketchPass();

    void render() override;
};
#endif