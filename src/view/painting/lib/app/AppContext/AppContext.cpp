#include "AppContext.h"

AppContext::AppContext(){

}
void AppContext::build(int width, int height){
    _editorManager = std::make_unique<EditorManager>();
    
    glfwInit();
    _window = glfwCreateWindow(width,height,"drawing area",NULL,NULL);
    glfwMakeContextCurrent(_window);
    
    _viewport = new Viewport(width, height);
    _renderer = new Renderer();
}

void AppContext::resize(int width, int height){
    glfwSetWindowSize(_window, width, height);
    _viewport->resize(width, height);
}

void AppContext::loop(void* arg){
    static_cast<AppContext*>(arg)->render();
}

void AppContext::render(){
    // glfwPollEvents();

    int w,h;
    glfwGetFramebufferSize(_window,&w,&h);
    glViewport(0,0,w,h);

    Editor* editor = _editorManager->getActiveEditor();
    if (!editor) { return; }
    Surface* surface = editor->getSurface();
    if (!surface) { return; }

    Preview* preview = editor->preview();

    
    _viewport->getCanvasSettings()->setGridDivisionsX(32);
    _viewport->getCanvasSettings()->setGridDivisionsY(32);
    _viewport->getCanvasSettings()->setSketchPosition(0,0);
    _viewport->getCanvasSettings()->setScale(15.0f);

    if(editor->getHeight() != flagHeight || editor->getWidth() != flagWidth){
        editor->compose();
        _renderer->init(surface, _viewport);
        _renderer->render(Bounding(Point(0,0), Point(editor->getWidth(), editor->getHeight())),
                            editor->getSurface(),
                            _viewport);
        
        flagHeight = editor->getHeight();
        flagWidth = editor->getWidth();
    }else{
        Bounding dirtArea = preview->getDirtyArea();
        if(dirtArea.getWidth() > 0 && dirtArea.getHeight() > 0){
            editor->compose(dirtArea);
            _renderer->render(dirtArea, editor->getSurface(), _viewport);
        }
    }

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
AppContext& AppContext::instance(){
    static AppContext appContext;
    return appContext;
}