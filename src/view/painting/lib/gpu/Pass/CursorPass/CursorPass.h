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
#include "../../Texture/Texture.h"

class CursorPass : public IRenderPass {
private:
    Surface* surface;
    ViewportContext* _viewport;
    Editor* editor;
    EditorManager* _manager;
    ToolManager* _toolManager;
    
    GLint brushSizeLocation;
    GLint texSizeLocationH;
    
    Quad quad;
    Shader shader;
    Texture texture;

public:
    CursorPass(EditorManager* manager, ToolManager* toolManager, ViewportContext* viewport);
    ~CursorPass();

    void render();
};
#endif