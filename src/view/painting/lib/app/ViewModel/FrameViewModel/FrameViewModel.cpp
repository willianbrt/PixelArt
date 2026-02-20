#include "FrameViewModel.h"


FrameViewModel::FrameViewModel(Frame& frame) : _frame(frame){
    Layer* layer = _frame.getActiveLayer();

}
FrameViewModel::~FrameViewModel(){
}
vector<Layer*> FrameViewModel::getAllLayers(){
    return _frame.getAllLayers();
}

void FrameViewModel::registerEvent(FRAME_EVENT_TYPE eventType, std::function<void(FrameEvent)> callback){
    _frame.registerEvent(eventType, callback);
}
void FrameViewModel::changeActiveLayer(Guid id){
    _frame.changeActiveLayer(id);
}
void FrameViewModel::addActiveLayer(){
    Layer layer("Layer ", 10,10);
    AddLayerCommand command(layer, _frame);
    command.execute();
}
void FrameViewModel::cloneActiveLayer(){
    Layer* layer = _frame.getActiveLayer();
    CloneLayerCommand command(layer->getID(), _frame);
    command.execute();
}
void FrameViewModel::moveLayerTo(Guid id, int index){
    MoveLayerToCommand command(_frame, id, index);
    command.execute();
}
void FrameViewModel::moveDownActiveLayer(){
    Layer* layer = _frame.getActiveLayer();
    size_t index = _frame.getLayerIndex(layer->getID());
    if(index > 0) return;

    MoveLayerToCommand command(_frame, layer->getID(), index - 1);
    command.execute();
}
void FrameViewModel::moveUpActiveLayer(){
    Layer* layer = _frame.getActiveLayer();
    size_t index = _frame.getLayerIndex(layer->getID());

    if(index >= _frame.getAllLayers().size()) return;

    MoveLayerToCommand command(_frame, layer->getID(), index + 1);
    command.execute();
}
void FrameViewModel::onRemoveLayer(){
    Layer* layer = _frame.getActiveLayer();
    size_t index = _frame.getLayerIndex(layer->getID());

    RemoveLayerCommand command(_frame, layer->getID());
    command.execute();
}

/*

function activeFrameContainLayer(layerID){
    let activeFrame = editor.getActiveFrame();
    const strIdLayer = layerID.toString();

    let frames = activeFrame.getAllLayers();
    for(let i = 0; i < frames.size(); i++){
        if(frames.get(i).getID().toString() == strIdLayer){
            return true;
        }
    }
    return false;
}
function findTitle(find) {
    let name = find;
    let cntr = 1;
    while(hasLayerWithName(name)){
        name = `${name.replace(/\(\d+\)$/, '')}(${cntr})`;
        cntr++;
    }

    return name;
}
function hasLayerWithName(name){
    let layers = editor.getActiveFrame().getAllLayers();
    for(let i = 0; i < layers.size(); i++){
        if(layers.get(i).getName() === name){
            return true;
        }
    }
    return false;
}
*/