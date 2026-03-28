#ifndef APPCONTEXT_H
#define APPCONTEXT_H

#include "../../objects/EditorManager/EditorManager.h"

#include <emscripten.h>
#include "../../objects/Renderer/Renderer.h"
#include "../../objects/Viewport/Viewport.h"
#include "../../context/BrushContext/BrushContext.h"
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

class AppContext{
private:
    std::unique_ptr<EditorManager> _editorManager;
    
    GLFWwindow* _window;

    Renderer* _renderer;
    Viewport* _viewport;
    int flagWidth;
    int flagHeight;
    Pattern pattern;

    public:
    AppContext();
    
    void build(int width, int height);
    void resize(int width, int height);
    static void loop(void* arg);
    void render();
    
    EditorManager* getEditorManager();
    Viewport* getViewport();
    Renderer* getRenderer();
    GLFWwindow* getWindow();

    static AppContext& instance();
};
#endif
