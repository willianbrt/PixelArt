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
    ToolSettings* _toolSettings;
    ISurface* _shape = nullptr;
    ISurface* _pattern = nullptr;
    
    GLint brushSizeLocation;
    GLint fillModeLocation;
    GLint alignModeLocation;
    GLint maskAlignLocation;
    GLint brushColorLocation;
    GLint hardnessLocation;
    GLint editorSizeLocation;
    GLint texPatternSizeLocationH;
    GLint texShapeSizeLocationH;
    
    
    Quad quad;
    Shader shader;
    Texture texturePattern;
    Texture textureShape;

public:
    CursorPass(EditorManager* manager, ToolManager* toolManager, ToolSettings* toolSettings, ViewportContext* viewport);
    ~CursorPass();

    void render();
};
#endif