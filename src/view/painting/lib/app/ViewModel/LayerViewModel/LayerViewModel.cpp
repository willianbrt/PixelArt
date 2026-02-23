#include "LayerViewModel.h"


LayerViewModel::LayerViewModel(std::string layerID) {
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();
    _layer = _frame->getLayerByID(Guid(layerID));
}
LayerViewModel::~LayerViewModel(){
}

void LayerViewModel::setOpacity(float opacity){
    // LayerOpacityCommand command(_layer, opacity);
    // command.execute();
}
void LayerViewModel::setIsVisible(bool isVisible){
    // LayerVisibilityCommand command(_layer, isVisible);
    // command.execute();
}
void LayerViewModel::setIsLock(bool isLock){
    // LayerLockCommand command(_layer, isLock);
    // command.execute();
}
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<LayerViewModel>("LayerViewModel")
        .constructor<std::string>()
        .function("setOpacity", &LayerViewModel::setOpacity)
        .function("setIsLock", &LayerViewModel::setIsLock)
        .function("setIsVisible", &LayerViewModel::setIsVisible)
        // .function("registerEvent", &LayerViewModel::registerEvent);
        ;
};