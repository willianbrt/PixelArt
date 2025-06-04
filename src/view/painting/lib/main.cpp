
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/html5.h>
#include <memory>

extern "C" {
    void add_layer();
}

class Editor
{
public:
    Editor(){
        printf("ola \n");
        add_layer();
    }
    void teste(){

    }
};

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<Editor>("Editor")
        .constructor<>()
        .smart_ptr<std::shared_ptr<Editor>>("shared_ptr<Editor>")
        .function("teste", &Editor::teste);
};
