#include "AppContext/AppContext.h"
#include "../objects/Renderer/Renderer.h"
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

int _width = 0;
int _height = 0;
GLFWwindow* window;
Renderer* renderer;

void loop(){
    glfwPollEvents();
    Editor* editor = AppContext::instance().getEditorManager()->getActiveEditor();
    int w,h;
    glfwGetFramebufferSize(window,&w,&h);

    glViewport(0,0,w,h);


    // glClearColor(0.1f,0.1f,0.1f,1.0f);
    // glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
}

void initApp(int width, int height){
    AppContext::instance().build();

    _width = width;
    _height = height;

    glfwInit();
    window = glfwCreateWindow(_width,_height,"drawing area",NULL,NULL);
    glfwMakeContextCurrent(window);

    renderer = new Renderer(_width, _height);


    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(loop,0,false);
    #else
        while(!glfwWindowShouldClose(window))
        {
            loop();
        }
    #endif
}

void resize(int width, int height){
    glfwSetWindowSize(window, width, height);
}
int main()
{

    return 0;
}
#ifdef __EMSCRIPTEN__
    #include <emscripten/bind.h>

    using namespace emscripten;

    EMSCRIPTEN_BINDINGS(pixel_editor_module){
        emscripten::function("initApp", &initApp);
        emscripten::function("resize", &resize);
    };
#endif