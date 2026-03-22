#include "AppContext/AppContext.h"
#include <emscripten.h>
#include <emscripten/html5.h>

void initApp(int width, int height){
    AppContext::instance().build(width, height);
}

void resize(int width, int height){
    AppContext::instance().resize(width, height);
    
}
int main()
{
    emscripten_set_main_loop_arg(AppContext::loop, &AppContext::instance(), 0, true);
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