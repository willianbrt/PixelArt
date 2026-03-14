#include "AppContext/AppContext.h"


void initApp(){
    AppContext::instance().build();
}

int main()
{
    return 0;
}

#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    emscripten::function("initApp", &initApp);
};