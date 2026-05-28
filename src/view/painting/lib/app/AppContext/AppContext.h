#ifndef APPCONTEXT_H
#define APPCONTEXT_H

#include "../../objects/EditorManager/EditorManager.h"

#include <emscripten.h>
#include "../../objects/Renderer/Renderer.h"
#include "../../objects/Viewport/Viewport.h"
#include "../../context/CursorContext/CursorContext.h"
#include "../../context/ViewportContext/ViewportContext.h"
#include "../../objects/ToolManager/ToolManager.h"
#include "../../gpu/Pass/CursorPass/CursorPass.h"
#include "../../gpu/Pass/SelectPass/SelectPass.h"
#include "../../gpu/RendererPipeline/RendererPipeline.h"

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

class AppContext{
private:
    std::unique_ptr<EditorManager> _editorManager;
    
    GLFWwindow* _window;

    Viewport* _viewport;
    ViewportContext* _viewportContext;
    Editor* _activeEditor;
    ToolManager* _toolManager;
    CursorContext* _cursorContext;
    RendererPipeline* _renderer;
    
public:
    AppContext();
    
    void build(int width, int height);
    void resize(int width, int height);
    static void loop(void* arg);
    void render();
    
    EditorManager* getEditorManager();
    Viewport* getViewport();
    GLFWwindow* getWindow();
    ToolManager* getToolManager();

    static AppContext& instance();
};
#endif
