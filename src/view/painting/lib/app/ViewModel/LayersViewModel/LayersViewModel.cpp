#include "LayersViewModel.h"


LayersViewModel::LayersViewModel(Frame& frame) : _frame(frame){
    Layer* layer = _frame.getActiveLayer();

}
LayersViewModel::~LayersViewModel(){
}
vector<Layer*> LayersViewModel::getAllLayers(){
    return _frame.getAllLayers();
}

void LayersViewModel::registerEvent(FRAME_EVENT_TYPE eventType, std::function<void(FrameEvent)> callback){
    _frame.registerEvent(eventType, callback);
}
void LayersViewModel::onChangeActiveLayer(Guid id){
    _frame.changeActiveLayer(id);
}
void LayersViewModel::onAddLayer(){
    Layer layer("Layer ", 10,10);
    AddLayerCommand command(layer, _frame);
    command.execute();
}
void LayersViewModel::onDuplicateLayer(){
    Layer* layer = _frame.getActiveLayer();
    CloneLayerCommand command(layer->getID(), _frame);
    command.execute();
}
void LayersViewModel::onMoveLayerTo(Guid id, int index){
    MoveLayerToCommand command(_frame, id, index);
    command.execute();
}
void LayersViewModel::onMoveDownLayer(){
    Layer* layer = _frame.getActiveLayer();
    size_t index = _frame.getLayerIndex(layer->getID());
    if(index > 0) return;

    MoveLayerToCommand command(_frame, layer->getID(), index - 1);
    command.execute();
}
void LayersViewModel::onMoveUpLayer(){
    Layer* layer = _frame.getActiveLayer();
    size_t index = _frame.getLayerIndex(layer->getID());

    if(index >= _frame.getAllLayers().size()) return;

    MoveLayerToCommand command(_frame, layer->getID(), index + 1);
    command.execute();
}
void LayersViewModel::onRemoveLayer(){
    Layer* layer = _frame.getActiveLayer();
    size_t index = _frame.getLayerIndex(layer->getID());

    RemoveLayerCommand command(_frame, layer->getID());
    command.execute();
}
void LayersViewModel::onFlipXLayer(){
    Layer* layer = _frame.getActiveLayer();
    // AddLayerCommand command(_frame, layer->getID());
    // command.execute();
}
void LayersViewModel::onFlipYLayer(){
    Layer* layer = _frame.getActiveLayer();
    // AddLayerCommand command(_frame, layer->getID());
    // command.execute();
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