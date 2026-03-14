#include "PaneToolbarViewModel.h"


PaneToolbarViewModel::PaneToolbarViewModel(){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
   _frame = _editor->getActiveFrame();
}
PaneToolbarViewModel::~PaneToolbarViewModel(){
}
void PaneToolbarViewModel::change(){
}
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<PaneToolbarViewModel>("PaneToolbarViewModel")
        .constructor<>()
        // .function("getNumberLayers", &PaneToolbarViewModel::getNumberLayers)
        ;
};