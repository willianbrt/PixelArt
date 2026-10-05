#include "FrameVM.h"


FrameVM::FrameVM(){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
   _frame = _editor->getActiveFrame();

    _frame->registerEvent(this);
}
FrameVM::~FrameVM(){
}
Frame* FrameVM::getActiveFrame(){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();

   return _editor->getActiveFrame();
}
void FrameVM::registerEvent(string eventType, emscripten::val callback){
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
    getActiveFrame()->changeActiveLayer(Guid(id));
}
void FrameVM::createLayer(){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();

    auto layer = std::make_unique<Layer>("Layer 1", _editor->getWidth(), _editor->getHeight());
    size_t activeLayerIndex = _frame->getLayerIndex(_frame->getActiveLayer()->getID()); 
    
    AddLayerCommand command(*_frame, std::move(layer), activeLayerIndex+1);
    command.execute();
}
void FrameVM::removeActiveLayer(){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();

    Layer* layer = _frame->getActiveLayer();
    RemoveLayerCommand command(*_frame, layer->getID());
    command.execute();
}
void FrameVM::cloneActiveLayer(){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();

    Layer* layer = _frame->getActiveLayer();
    CloneLayerCommand command(layer->getID(), *_frame);
    command.execute();
}

void FrameVM::moveLayerTo(std::string id, std::string afterId){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();

    MoveLayerToCommand command(*_frame, Guid(id), _frame->getLayerIndex(Guid(afterId)));
    command.execute();
}
void FrameVM::moveDownActiveLayer(){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();

    Layer* layer = _frame->getActiveLayer();
    size_t index = _frame->getLayerIndex(layer->getID());
    if(index < 0) return;

    MoveLayerToCommand command(*_frame, layer->getID(), index - 1);
    command.execute();
}
void FrameVM::moveUpActiveLayer(){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();

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
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();
    Layer* _layer = _frame->getActiveLayer();
    _initialOpacity = _layer->getOpacity();
    
}

void FrameVM::onChangeActiveLayerOpacity(float opacity){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();
    Layer* _layer = _frame->getActiveLayer();
    _layer->setOpacity(opacity);
}

void FrameVM::endChangeActiveLayerOpacity(){
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    Frame* _frame = _editor->getActiveFrame();
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
    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
    _editor->getDirtyManager()->markDirty({{0,0},{_editor->getWidth()-1, _editor->getHeight()-1}});
}
void FrameVM::onMoveLayerTo(Guid id, int index){
    auto it = observable.find(FRAME_EVENT_TYPE::MOVE_LAYER_TO);
    if (it != observable.end()) {
        it->second(id, index);
    }

    EditorManager*  _manager = AppContext::instance().getEditorManager();
    Editor* _editor = _manager->getActiveEditor();
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
        .constructor<>()
        .function("getNumberLayers", &FrameVM::getNumberLayers)
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