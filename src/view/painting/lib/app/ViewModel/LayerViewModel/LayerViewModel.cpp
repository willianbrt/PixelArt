#include "LayerViewModel.h"


LayerViewModel::LayerViewModel(Guid layerID) {
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();
    _layer = _frame->getLayerByID(layerID);
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