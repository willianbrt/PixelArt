#include "AppContext.h"

AppContext::AppContext(){

}
void AppContext::build(int width, int height){
    _viewport = new Viewport(width, height);
    _viewport->getCanvasSettings()->setGridDivisionsX(32);
    _viewport->getCanvasSettings()->setGridDivisionsY(32);
    _viewport->getCanvasSettings()->setSketchPosition(0,0);
    _viewport->getCanvasSettings()->setScale(15.0f);

    glfwInit();
    _window = glfwCreateWindow(_viewport->getWidth(), _viewport->getHeight(),"drawing area",NULL,NULL);
    glfwMakeContextCurrent(_window);
    
    _editorManager = std::make_unique<EditorManager>();
    _renderer = new Renderer();

    _toolManager = new ToolManager(_editorManager.get(), _viewport);
    _cursorContext = _toolManager->getToolPressed()->getCursorContext();
    _renderer->initCursorHover(_cursorContext);
}

void AppContext::resize(int width, int height){
    glfwSetWindowSize(_window, width, height);
    _viewport->resize(width, height);
}

void AppContext::loop(void* arg){
    static_cast<AppContext*>(arg)->render();
}

void AppContext::render(){
    glfwPollEvents();
    // glClear (GL_COLOR_BUFFER_BIT);
    
    glViewport(0,0, _viewport->getWidth(), _viewport->getHeight());
    
    double xpos, ypos;
    glfwGetCursorPos(_window, &xpos, &ypos);
    _viewport->setCursor(xpos, ypos);
    
    if (!_editorManager) { return; }

    Editor* editor = _editorManager->getActiveEditor();

    if (editor != _activeEditor) { 
        _activeEditor = editor;
    
        _activeEditor->compose();
        _renderer->init(_activeEditor->getSurface(), _viewport);

        Bounding dirtArea = {
            Point(0,0),
            Point(_activeEditor->getWidth(), _activeEditor->getHeight())
        };
        _renderer->uploadSurface(dirtArea, _activeEditor->getSurface());
    }else{
        Preview* preview = _activeEditor->preview();
        Bounding dirtArea = preview->getDirtyArea();
        if(dirtArea.getWidth() > 0 && dirtArea.getHeight() > 0){
            _activeEditor->compose(dirtArea);
            _renderer->uploadSurface(dirtArea, _activeEditor->getSurface());
        }
    }
    
    _renderer->draw(_activeEditor->getSurface(), _viewport);

    IPressedStrategy* tool = _toolManager->getToolPressed();
    if(!tool) return;

    CursorContext* cursorContext = tool->getCursorContext();
    if(!cursorContext) return;
    if(_cursorContext->pattern->height != cursorContext->pattern->height || _cursorContext->pattern->width != cursorContext->pattern->width){
        _cursorContext = cursorContext;
        _renderer->initCursorHover(_cursorContext);
    }
    if(cursorContext->enable)
        _renderer->drawCursorHover(_activeEditor->getSurface(), _cursorContext, _viewport);


    glfwSwapBuffers(_window);
}
EditorManager* AppContext::getEditorManager(){
    return _editorManager.get();
}
Viewport* AppContext::getViewport(){
    return _viewport;
}
Renderer* AppContext::getRenderer(){
    return _renderer;
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