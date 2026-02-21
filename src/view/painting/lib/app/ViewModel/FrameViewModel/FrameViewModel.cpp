#include "FrameViewModel.h"


FrameViewModel::FrameViewModel(Frame& frame) : _frame(frame){
    Layer* layer = _frame.getActiveLayer();

    _frame.registerEvent(this);
}
FrameViewModel::~FrameViewModel(){
}
void FrameViewModel::registerEvent(string eventType, emscripten::val callback){
    if(eventType == "ADD_LAYER"){
        observable[FRAME_EVENT_TYPE::ADD_LAYER] = callback;
        return;
    }
    if(eventType == "REMOVE_LAYER"){
         observable[FRAME_EVENT_TYPE::REMOVE_LAYER] = callback;
        return;
    }
    if(eventType == "MOVE_LAYER_TO"){
        observable[FRAME_EVENT_TYPE::MOVE_LAYER_TO] = callback;
        return;
    }
    if(eventType == "CHANGE_ACTIVE_LAYER"){
        observable[FRAME_EVENT_TYPE::CHANGE_ACTIVE_LAYER] = callback;
        return;
    }
}

LayerDTO FrameViewModel::getLayerByIndex(size_t index){
    Layer* layer = _frame.getLayerByIndex(index);

    LayerDTO layerDTO;
    layerDTO.id = layer->getID().toString();
    layerDTO.buffer = emscripten::val(emscripten::typed_memory_view(layer->getWidth()* layer->getHeight()*4, reinterpret_cast<uint8_t*>(layer->getBuffer())));
    layerDTO.width = layer->getWidth();
    layerDTO.height = layer->getHeight();
    layerDTO.isActive = _frame.getActiveLayer() == layer;
    
    return layerDTO;
}
size_t FrameViewModel::getNumberLayers(){
    return _frame.getLayersLength();
}



void FrameViewModel::changeActiveLayer(Guid id){
    _frame.changeActiveLayer(id);
}
void FrameViewModel::createLayer(){
    // Layer layer("Layer ", 10,10);
    // AddLayerCommand command(layer, _frame);
    // command.execute();
}
void FrameViewModel::removeActiveLayer(){

}
void FrameViewModel::cloneActiveLayer(){
    // Layer* layer = _frame.getActiveLayer();
    // CloneLayerCommand command(layer->getID(), _frame);
    // command.execute();
}
void FrameViewModel::moveLayerTo(Guid id, int index){
    // MoveLayerToCommand command(_frame, id, index);
    // command.execute();
}
void FrameViewModel::moveDownActiveLayer(){
    // Layer* layer = _frame.getActiveLayer();
    // size_t index = _frame.getLayerIndex(layer->getID());
    // if(index > 0) return;

    // MoveLayerToCommand command(_frame, layer->getID(), index - 1);
    // command.execute();
}
void FrameViewModel::moveUpActiveLayer(){
    // Layer* layer = _frame.getActiveLayer();
    // size_t index = _frame.getLayerIndex(layer->getID());

    // if(index >= _frame.getLayersLength()) return;

    // MoveLayerToCommand command(_frame, layer->getID(), index + 1);
    // command.execute();
}
void FrameViewModel::flipXLayer(){
}
void FrameViewModel::flipYLayer(){
}



void FrameViewModel::onChangeActiveLayer(Guid id){
    auto it = observable.find(FRAME_EVENT_TYPE::CHANGE_ACTIVE_LAYER);
    if (it != observable.end()) {
        it->second(id.toString());
    }
}
void FrameViewModel::onAddLayer(Layer* layer, size_t index){
    LayerDTO layerDTO;
    layerDTO.id = layer->getID().toString();
    layerDTO.buffer = emscripten::val(emscripten::typed_memory_view(layer->getWidth()* layer->getHeight()*4, reinterpret_cast<uint8_t*>(layer->getBuffer())));
    layerDTO.width = layer->getWidth();
    layerDTO.height = layer->getHeight();
    layerDTO.isActive = _frame.getActiveLayer() == layer;

    auto it = observable.find(FRAME_EVENT_TYPE::ADD_LAYER);
    if (it != observable.end()) {
        it->second(layerDTO, index);
    }
}
void FrameViewModel::onRemoveLayer(Guid id){
    auto it = observable.find(FRAME_EVENT_TYPE::REMOVE_LAYER);
    if (it != observable.end()) {
        it->second(id.toString());
    }
}
void FrameViewModel::onMoveLayerTo(Guid id, int index){
    auto it = observable.find(FRAME_EVENT_TYPE::MOVE_LAYER_TO);
    if (it != observable.end()) {
        it->second(id, index);
    }
}

/*

function activeLayerContainLayer(layerID){
    let activeLayer = editor.getActiveLayer();
    const strIdLayer = layerID.toString();

    let frames = activeLayer.getAllLayers();
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
    let layers = editor.getActiveLayer().getAllLayers();
    for(let i = 0; i < layers.size(); i++){
        if(layers.get(i).getName() === name){
            return true;
        }
    }
    return false;
}
*/

#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<FrameViewModel>("FrameViewModel")
        .constructor<Frame&>()
        // .smart_ptr<std::shared_ptr<FrameViewModel>>("shared_ptr<FrameViewModel>")
        .function("getNumberLayers", &FrameViewModel::getNumberLayers)
        .function("getLayerByIndex", &FrameViewModel::getLayerByIndex)
        .function("registerEvent", &FrameViewModel::registerEvent)
        .function("changeActiveLayer", &FrameViewModel::changeActiveLayer)
        .function("createLayer", &FrameViewModel::createLayer)
        .function("cloneActiveLayer", &FrameViewModel::cloneActiveLayer)
        .function("moveLayerTo", &FrameViewModel::moveLayerTo)
        .function("moveDownActiveLayer", &FrameViewModel::moveDownActiveLayer)
        .function("moveUpActiveLayer", &FrameViewModel::moveUpActiveLayer)
        .function("removeActiveLayer", &FrameViewModel::removeActiveLayer)
        .function("flipXLayer", &FrameViewModel::flipXLayer)
        .function("flipYLayer", &FrameViewModel::flipYLayer);
};