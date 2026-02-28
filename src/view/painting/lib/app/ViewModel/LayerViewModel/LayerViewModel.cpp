#include "LayerViewModel.h"


LayerViewModel::LayerViewModel(std::string layerID) {
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();
    _layer = _frame->getLayerByID(Guid(layerID));
    _layer->registerEvent(this);
}
LayerViewModel::~LayerViewModel(){
}

void LayerViewModel::setOpacity(float opacity){
    LayerOpacityCommand command(*_layer, _layer->getOpacity(), opacity);
    command.execute();
}
void LayerViewModel::setIsVisible(bool isVisible){
    LayerVisibilityCommand command(*_layer, isVisible);
    command.execute();
}
void LayerViewModel::setIsLock(bool isLock){
    LayerLockCommand command(*_layer, isLock);
    command.execute();
}
void LayerViewModel::setName(string name){
    LayerRenameCommand command(*_layer, name);
    command.execute();
}
void LayerViewModel::registerEvent(string eventType, emscripten::val callback){
    if(eventType == "RENAME_LAYER"){
        observable[LAYER_EVENT_TYPE::RENAME_LAYER] = callback;
        return;
    }
    if(eventType == "IS_LOCK_LAYER"){
        observable[LAYER_EVENT_TYPE::IS_LOCK_LAYER] = callback;
        return;
    }
    if(eventType == "IS_VISIBLE_LAYER"){
        observable[LAYER_EVENT_TYPE::IS_VISIBLE_LAYER] = callback;
        return;
    }
    if(eventType == "OPACITY_LAYER"){
        observable[LAYER_EVENT_TYPE::OPACITY_LAYER] = callback;
        return;
    }
}
void LayerViewModel::onIsVisibleLayer(){
    auto it = observable.find(LAYER_EVENT_TYPE::IS_VISIBLE_LAYER);
    if (it != observable.end()) {
        it->second(_layer->isVisible());
    }
}
void LayerViewModel::onIsLockLayer(){
    auto it = observable.find(LAYER_EVENT_TYPE::IS_LOCK_LAYER);
    if (it != observable.end()) {
        it->second(_layer->isLock());
    }
}
void LayerViewModel::onOpacityLayer(){
    auto it = observable.find(LAYER_EVENT_TYPE::OPACITY_LAYER);
    if (it != observable.end()) {
        it->second(_layer->getOpacity());
    }
}
void LayerViewModel::onRenameLayer(){
    auto it = observable.find(LAYER_EVENT_TYPE::RENAME_LAYER);
    if (it != observable.end()) {
        it->second(_layer->getName());
    }
}


#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<LayerViewModel>("LayerViewModel")
        .constructor<std::string>()
        .function("registerEvent", &LayerViewModel::registerEvent)
        .function("setName", &LayerViewModel::setName)
        .function("setOpacity", &LayerViewModel::setOpacity)
        .function("setIsLock", &LayerViewModel::setIsLock)
        .function("setIsVisible", &LayerViewModel::setIsVisible)
        ;
};