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

class SketchPass : public IRenderPass {
private:
    Surface* _surface;
    ViewportContext* _viewport;
    CursorContext* hover;
    Editor* editor;
    EditorManager* _manager;

    GLuint program;
    GLuint texture;
    GLint pos;
    GLint texSizeLocation;
    GLint gridDivisionsLocation;
    GLint lightColorLocation;
    GLint darkColorLocation;

    Quad quad;
    Shader shader;
    GlobalUBO globalUBO;
    GLuint blockIndex;

public:
    SketchPass();
    SketchPass(EditorManager* manager, ViewportContext* viewport);
    ~SketchPass();

    void init() override;
    void draw() override;
    void upload(Bounding area) override;
};
#endif