#include "LayerViewModel.h"


LayerViewModel::LayerViewModel(Layer& layer) : _layer(layer){
}
LayerViewModel::~LayerViewModel(){
}
// void LayerViewModel::registerEvent(EDITOR_EVENT_TYPE eventType, std::function<void(EditorEvent)> callback){
//     _layer.registerEvent(eventType, callback);
// }
void LayerViewModel::setOpacity(float opacity){
    LayerOpacityCommand command(_layer, opacity);
    command.execute();
}
void LayerViewModel::setIsVisible(bool isVisible){
    LayerVisibilityCommand command(_layer, isVisible);
    command.execute();
}
void LayerViewModel::setIsLock(bool isLock){
    LayerLockCommand command(_layer, isLock);
    command.execute();
}