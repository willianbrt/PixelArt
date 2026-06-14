#ifndef CURSORPASS_H
#define CURSORPASS_H

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

#include "../../../interfaces/IRenderPass/IRenderPass.h"


#include "../../../objects/Editor/Editor.h"
#include "../../../graphics/surface/Surface.h"
#include "../../../graphics/surface/Surface.h"
#include "../../../objects/EditorManager/EditorManager.h"
#include "../../../objects/ToolManager/ToolManager.h"
#include "../../../context/CursorContext/CursorContext.h"
#include "../../../context/ViewportContext/ViewportContext.h"
#include "../../Quad/Quad.h"
#include "../../Shader/Shader.h"

class CursorPass : public IRenderPass {
private:
    Surface* surface;
    ViewportContext* _viewport;
    CursorContext* hover;
    Editor* editor;
    EditorManager* _manager;
    ToolManager* _toolManager;
    
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
    
    bool initialized;
    Quad quad;
    Shader shader;

public:
    CursorPass(EditorManager* manager, ToolManager* toolManager, ViewportContext* viewport);
    ~CursorPass();

    void init();
    void render() override;
    void upload(Bounding area);
};
#endif