#include "FrameVM.h"


FrameVM::FrameVM(std::string editorId, std::string id){
    _manager = AppContext::instance().getEditorManager();
    _editor = _manager->getEditorById(Guid(editorId));
    _frame = _editor->getFrameById(Guid(id));

    _frame->registerEvent(this);
}
FrameVM::~FrameVM(){
    _frame->unregisterEvent(this);
}
void FrameVM::registerEvent(std::string eventType, emscripten::val callback){
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

LayerDTO FrameVM::getActiveLayer(){
    Layer* layer = _frame->getActiveLayer();

    LayerDTO layerDTO;
    layerDTO.id = layer->getID().toString();
    layerDTO.name = layer->getName();
    layerDTO.opacity = layer->getOpacity();
    layerDTO.isLock = layer->isLock();
    layerDTO.isVisible = layer->isVisible();
    layerDTO.buffer = emscripten::val(emscripten::typed_memory_view(layer->getWidth()* layer->getHeight()*4, reinterpret_cast<uint8_t*>(layer->getBuffer())));
    layerDTO.width = layer->getWidth();
    layerDTO.height = layer->getHeight();
    layerDTO.isActive = _frame->getActiveLayer() == layer;
    
    return layerDTO;
}
LayerDTO FrameVM::getLayerByIndex(size_t index){
    Layer* layer = _frame->getLayerByIndex(index);

    LayerDTO layerDTO;
    layerDTO.id = layer->getID().toString();
    layerDTO.name = layer->getName();
    layerDTO.opacity = layer->getOpacity();
    layerDTO.isLock = layer->isLock();
    layerDTO.isVisible = layer->isVisible();
    layerDTO.buffer = emscripten::val(emscripten::typed_memory_view(layer->getWidth()* layer->getHeight()*4, reinterpret_cast<uint8_t*>(layer->getBuffer())));
    layerDTO.width = layer->getWidth();
    layerDTO.height = layer->getHeight();
    layerDTO.isActive = _frame->getActiveLayer() == layer;
    
    return layerDTO;
}
size_t FrameVM::getNumberLayers(){
    return _frame->getLayersLength();
}



void FrameVM::changeActiveLayer(std::string id){
    _frame->changeActiveLayer(Guid(id));
}
void FrameVM::createLayer(){
    auto layer = std::make_unique<Layer>("Layer 1", _editor->getWidth(), _editor->getHeight());
    size_t activeLayerIndex = _frame->getLayerIndex(_frame->getActiveLayer()->getID()); 
    
    AddLayerCommand command(*_frame, std::move(layer), activeLayerIndex+1);
    command.execute();
}
void FrameVM::removeActiveLayer(){
    Layer* layer = _frame->getActiveLayer();
    RemoveLayerCommand command(*_frame, layer->getID());
    command.execute();
}
void FrameVM::cloneActiveLayer(){
    Layer* layer = _frame->getActiveLayer();
    CloneLayerCommand command(layer->getID(), *_frame);
    command.execute();
}

void FrameVM::moveLayerTo(std::string id, std::string afterId){
    MoveLayerToCommand command(*_frame, Guid(id), _frame->getLayerIndex(Guid(afterId)));
    command.execute();
}
void FrameVM::moveDownActiveLayer(){
    Layer* layer = _frame->getActiveLayer();
    size_t index = _frame->getLayerIndex(layer->getID());
    if(index < 0) return;

    MoveLayerToCommand command(*_frame, layer->getID(), index - 1);
    command.execute();
}
void FrameVM::moveUpActiveLayer(){
    Layer* layer = _frame->getActiveLayer();
    size_t index = _frame->getLayerIndex(layer->getID());

    if(index > _frame->getLayersLength()) return;

    MoveLayerToCommand command(*_frame, layer->getID(), index + 1);
    command.execute();
}
void FrameVM::flipXLayer(){
}
void FrameVM::flipYLayer(){
}

void FrameVM::beginChangeActiveLayerOpacity(){
    Layer* _layer = _frame->getActiveLayer();
    _initialOpacity = _layer->getOpacity();
}
void FrameVM::onChangeActiveLayerOpacity(float opacity){
    Layer* _layer = _frame->getActiveLayer();
    _layer->setOpacity(opacity);
}
void FrameVM::endChangeActiveLayerOpacity(){
    Layer* _layer = _frame->getActiveLayer();

    if(_initialOpacity ==  _layer->getOpacity() || _initialOpacity < 0) return;
    
    LayerOpacityCommand command(*_layer, _initialOpacity, _layer->getOpacity());
    command.execute();

    _initialOpacity = -1;
}

void FrameVM::onChangeActiveLayer(Guid id){
    endChangeActiveLayerOpacity();

    auto it = observable.find(FRAME_EVENT_TYPE::CHANGE_ACTIVE_LAYER);
    if (it != observable.end()) {
        it->second(id.toString());
    }
}
void FrameVM::onAddLayer(Layer* layer, size_t index){
    LayerDTO layerDTO;
    layerDTO.id = layer->getID().toString();
    layerDTO.name = layer->getName();
    layerDTO.opacity = layer->getOpacity();
    layerDTO.isLock = layer->isLock();
    layerDTO.isVisible = layer->isVisible();
    layerDTO.buffer = emscripten::val(emscripten::typed_memory_view(layer->getWidth()* layer->getHeight()*4, reinterpret_cast<uint8_t*>(layer->getBuffer())));
    layerDTO.width = layer->getWidth();
    layerDTO.height = layer->getHeight();
    layerDTO.isActive = _frame->getActiveLayer() == layer;

    auto it = observable.find(FRAME_EVENT_TYPE::ADD_LAYER);
    if (it != observable.end()) {
        it->second(layerDTO, index);
    }
}
void FrameVM::onRemoveLayer(Guid id){
    auto it = observable.find(FRAME_EVENT_TYPE::REMOVE_LAYER);
    if (it != observable.end()) {
        it->second(id.toString());
    }
    _editor->getDirtyManager()->markDirty({{0,0},{_editor->getWidth()-1, _editor->getHeight()-1}});
}
void FrameVM::onMoveLayerTo(Guid id, int index){
    auto it = observable.find(FRAME_EVENT_TYPE::MOVE_LAYER_TO);
    if (it != observable.end()) {
        it->second(id, index);
    }
    _editor->getDirtyManager()->markDirty({{0,0},{_editor->getWidth()-1, _editor->getHeight()-1}});
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
    class_<FrameVM>("FrameVM")
        .constructor<std::string, std::string>()
        .function("getNumberLayers", &FrameVM::getNumberLayers)
        .function("getActiveLayer", &FrameVM::getActiveLayer)
        .function("getLayerByIndex", &FrameVM::getLayerByIndex)
        .function("registerEvent", &FrameVM::registerEvent)
        .function("changeActiveLayer", &FrameVM::changeActiveLayer)
        .function("createLayer", &FrameVM::createLayer)
        .function("cloneActiveLayer", &FrameVM::cloneActiveLayer)
        .function("moveLayerTo", &FrameVM::moveLayerTo)
        .function("moveDownActiveLayer", &FrameVM::moveDownActiveLayer)
        .function("moveUpActiveLayer", &FrameVM::moveUpActiveLayer)
        .function("removeActiveLayer", &FrameVM::removeActiveLayer)
        .function("flipXLayer", &FrameVM::flipXLayer)
        .function("flipYLayer", &FrameVM::flipYLayer)
        .function("beginChangeActiveLayerOpacity", &FrameVM::beginChangeActiveLayerOpacity)
        .function("onChangeActiveLayerOpacity", &FrameVM::onChangeActiveLayerOpacity)
        .function("endChangeActiveLayerOpacity", &FrameVM::endChangeActiveLayerOpacity)
        ;
};