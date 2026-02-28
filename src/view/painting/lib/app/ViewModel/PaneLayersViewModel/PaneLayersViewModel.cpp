#include "PaneLayersViewModel.h"


PaneLayersViewModel::PaneLayersViewModel(){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
   _frame = _editor->getActiveFrame();

    _frame->registerEvent(this);
}
PaneLayersViewModel::~PaneLayersViewModel(){
}
Frame* PaneLayersViewModel::getActiveFrame(){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();

   return _editor->getActiveFrame();
}
void PaneLayersViewModel::registerEvent(string eventType, emscripten::val callback){
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

LayerDTO PaneLayersViewModel::getLayerByIndex(size_t index){
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
size_t PaneLayersViewModel::getNumberLayers(){
    return _frame->getLayersLength();
}



void PaneLayersViewModel::changeActiveLayer(std::string id){
    getActiveFrame()->changeActiveLayer(Guid(id));
}
void PaneLayersViewModel::createLayer(){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();

    auto layer = std::make_unique<Layer>("Layer 1", _editor->getWidth(), _editor->getHeight());
    size_t activeLayerIndex = _frame->getLayerIndex(_frame->getActiveLayer()->getID()); 
    
    AddLayerCommand command(*_frame, std::move(layer), activeLayerIndex+1);
    command.execute();
}
void PaneLayersViewModel::removeActiveLayer(){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();

    Layer* layer = _frame->getActiveLayer();
    RemoveLayerCommand command(*_frame, layer->getID());
    command.execute();
}
void PaneLayersViewModel::cloneActiveLayer(){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();

    Layer* layer = _frame->getActiveLayer();
    CloneLayerCommand command(layer->getID(), *_frame);
    command.execute();
}

void PaneLayersViewModel::moveLayerTo(std::string id, std::string afterId){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();

    MoveLayerToCommand command(*_frame, Guid(id), _frame->getLayerIndex(Guid(afterId)));
    command.execute();
}
void PaneLayersViewModel::moveDownActiveLayer(){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();

    Layer* layer = _frame->getActiveLayer();
    size_t index = _frame->getLayerIndex(layer->getID());
    if(index < 0) return;

    MoveLayerToCommand command(*_frame, layer->getID(), index - 1);
    command.execute();
}
void PaneLayersViewModel::moveUpActiveLayer(){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();

    Layer* layer = _frame->getActiveLayer();
    size_t index = _frame->getLayerIndex(layer->getID());

    if(index > _frame->getLayersLength()) return;

    MoveLayerToCommand command(*_frame, layer->getID(), index + 1);
    command.execute();
}
void PaneLayersViewModel::flipXLayer(){
}
void PaneLayersViewModel::flipYLayer(){
}

void PaneLayersViewModel::beginChangeActiveLayerOpacity(){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();
    Layer* _layer = _frame->getActiveLayer();
    _initialOpacity = _layer->getOpacity();
    
}

void PaneLayersViewModel::onChangeActiveLayerOpacity(float opacity){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();
    Layer* _layer = _frame->getActiveLayer();
    _layer->setOpacity(opacity);
}

void PaneLayersViewModel::endChangeActiveLayerOpacity(){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();
    Layer* _layer = _frame->getActiveLayer();

    if(_initialOpacity ==  _layer->getOpacity() || _initialOpacity < 0) return;
    
    LayerOpacityCommand command(*_layer, _initialOpacity, _layer->getOpacity());
    command.execute();

    _initialOpacity = -1;
}


void PaneLayersViewModel::onChangeActiveLayer(Guid id){
    endChangeActiveLayerOpacity();

    auto it = observable.find(FRAME_EVENT_TYPE::CHANGE_ACTIVE_LAYER);
    if (it != observable.end()) {
        it->second(id.toString());
    }
}
void PaneLayersViewModel::onAddLayer(Layer* layer, size_t index){
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
void PaneLayersViewModel::onRemoveLayer(Guid id){
    auto it = observable.find(FRAME_EVENT_TYPE::REMOVE_LAYER);
    if (it != observable.end()) {
        it->second(id.toString());
    }
}
void PaneLayersViewModel::onMoveLayerTo(Guid id, int index){
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
    class_<PaneLayersViewModel>("PaneLayersViewModel")
        .constructor<>()
        .function("getNumberLayers", &PaneLayersViewModel::getNumberLayers)
        .function("getLayerByIndex", &PaneLayersViewModel::getLayerByIndex)
        .function("registerEvent", &PaneLayersViewModel::registerEvent)
        .function("changeActiveLayer", &PaneLayersViewModel::changeActiveLayer)
        .function("createLayer", &PaneLayersViewModel::createLayer)
        .function("cloneActiveLayer", &PaneLayersViewModel::cloneActiveLayer)
        .function("moveLayerTo", &PaneLayersViewModel::moveLayerTo)
        .function("moveDownActiveLayer", &PaneLayersViewModel::moveDownActiveLayer)
        .function("moveUpActiveLayer", &PaneLayersViewModel::moveUpActiveLayer)
        .function("removeActiveLayer", &PaneLayersViewModel::removeActiveLayer)
        .function("flipXLayer", &PaneLayersViewModel::flipXLayer)
        .function("flipYLayer", &PaneLayersViewModel::flipYLayer)
        .function("beginChangeActiveLayerOpacity", &PaneLayersViewModel::beginChangeActiveLayerOpacity)
        .function("onChangeActiveLayerOpacity", &PaneLayersViewModel::onChangeActiveLayerOpacity)
        .function("endChangeActiveLayerOpacity", &PaneLayersViewModel::endChangeActiveLayerOpacity)
        ;
};