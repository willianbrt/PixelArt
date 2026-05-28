#include "AppContext.h"

AppContext::AppContext(){

}
void AppContext::build(int width, int height){
    _viewport = new Viewport(width, height);
    _viewportContext =new ViewportContext();
    _viewportContext->width = width;
    _viewportContext->height = height;
    
    glfwInit();
    _window = glfwCreateWindow(width, height,"drawing area",NULL,NULL);
    glfwMakeContextCurrent(_window);
    
    
    _editorManager = std::make_unique<EditorManager>();
    
    _toolManager = new ToolManager(_editorManager.get(), _viewport);


    _cursorContext = _toolManager->getToolPressed()->getCursorContext();
    
    _renderer = new RendererPipeline();
    _renderer->registerPass(new SketchPass(_editorManager.get(), _viewportContext));
}

void AppContext::resize(int width, int height){
    glfwSetWindowSize(_window, width, height);
    _viewport->resize(width, height);
    
    _viewportContext->width = width;
    _viewportContext->height = height;
}

void AppContext::loop(void* arg){
    static_cast<AppContext*>(arg)->render();
}

void AppContext::render(){
    glfwPollEvents();
    glViewport(0,0, _viewport->getWidth(), _viewport->getHeight());
    glViewport(0,0, _viewportContext->width, _viewportContext->height);
    
    double xpos, ypos;
    glfwGetCursorPos(_window, &xpos, &ypos);
    _viewport->setCursor(xpos, ypos);
    _viewportContext->cursorX = xpos;
    _viewportContext->cursorY = ypos;
    
    _renderer->render();

    glfwSwapBuffers(_window);
}
EditorManager* AppContext::getEditorManager(){
    return _editorManager.get();
}
Viewport* AppContext::getViewport(){
    return _viewport;
}
GLFWwindow* AppContext::getWindow(){
    return _window;
}
ToolManager* AppContext::getToolManager(){
    return _toolManager;
}
AppContext& AppContext::instance(){
    static AppContext appContext;
    return appContext;
}