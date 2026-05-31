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
    Surface* _surface;
    ViewportContext* _viewport;
    CursorContext* hover;
    Editor* editor;
    EditorManager* _manager;
    
    GLuint programSelect;
    GLuint canvasSelect;
    GLuint texture;
    GLint posSelect;
    GLint positionLocation;
    GLint cursorWorldLocation;
    GLint scaleLocation;
    GLint texSizeLocation;
    GLint resolutionLocation;
    GLint selectSizeLocation;
    bool initializedSelect;
    GLint cursorLocation;
    Quad quad;
    Shader shader;


public:
    SelectPass(EditorManager* manager, ViewportContext* viewport);
    ~SelectPass();

    void init() override;
    void draw() override;
    void upload(Bounding area) override;
};
#endif