
#include <emscripten/html5.h>

#include "ViewModel/EditorViewModel/EditorViewModel.h"
// #include "ViewModel/FramesViewModel/FramesViewModel.h"
// #include "View/FramesView/FramesView.h"

EditorViewModel createProject(int width, int height){
    Editor* editor = new Editor(width, height);

    auto frame = std::make_unique<Frame>();
    auto layer = std::make_unique<Layer>("layer 1", editor->getWidth(), editor->getHeight());
    frame->addLayer(std::move(layer), 0);

    editor->addFrame(std::move(frame), 0);

    return EditorViewModel(*editor);
}
int main(){
    return 0;
}


#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    emscripten::function("createProject", &createProject);
};