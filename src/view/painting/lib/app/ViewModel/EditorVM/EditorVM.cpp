#include "EditorVM.h"


EditorVM::EditorVM(std::string id){
    _manager = AppContext::instance().getEditorManager();
    _editor = _manager->getEditorById(Guid(id));
    _editor->registerEvent(this);
}
EditorVM::~EditorVM(){
    _editor->unregisterEvent(this);
}
Editor* EditorVM::getActiveEditor(){
    return _manager->getActiveEditor();
}
void EditorVM::registerEvent(std::string eventType, emscripten::val callback){
    if(eventType == "ADD_FRAME"){
        observable[EDITOR_EVENT_TYPE::ADD_FRAME] = callback;
        return;
    }
    if(eventType == "REMOVE_FRAME"){
         observable[EDITOR_EVENT_TYPE::REMOVE_FRAME] = callback;
        return;
    }
    if(eventType == "MOVE_FRAME_TO"){
        observable[EDITOR_EVENT_TYPE::MOVE_FRAME_TO] = callback;
        return;
    }
    if(eventType == "CHANGE_ACTIVE_FRAME"){
        observable[EDITOR_EVENT_TYPE::CHANGE_ACTIVE_FRAME] = callback;
        return;
    }
}

void EditorVM::onChangeActiveFrame(Guid id){
    auto it = observable.find(EDITOR_EVENT_TYPE::CHANGE_ACTIVE_FRAME);
    if (it != observable.end()) {
        it->second(id.toString());
    }
    _editor->getDirtyManager()->markDirty({{0,0},{_editor->getWidth()-1, _editor->getHeight()-1}});
}
void EditorVM::onAddFrame(Frame* frame, size_t index){
    FrameDTO frameDTO;
    frameDTO.id = frame->getID().toString();
    frameDTO.timeDuration = frame->getFrameDuration();
    frameDTO.buffer = emscripten::val(emscripten::typed_memory_view(_editor->getWidth()* _editor->getHeight()*4, reinterpret_cast<uint8_t*>(frame->getBuffer())));
    frameDTO.width = _editor->getWidth();
    frameDTO.height = _editor->getHeight();
    frameDTO.isActive = _editor->getActiveFrame() == frame;

    auto it = observable.find(EDITOR_EVENT_TYPE::ADD_FRAME);
    if (it != observable.end()) {
        it->second(frameDTO, index);
    }
    _editor->getDirtyManager()->markDirty({{0,0},{_editor->getWidth()-1, _editor->getHeight()-1}});
}
void EditorVM::onRemoveFrame(Guid id){
    auto it = observable.find(EDITOR_EVENT_TYPE::REMOVE_FRAME);
    if (it != observable.end()) {
        it->second(id.toString());
    }
    _editor->getDirtyManager()->markDirty({{0,0},{_editor->getWidth()-1, _editor->getHeight()-1}});
}
void EditorVM::onMoveFrameTo(Guid id, int index){
    auto it = observable.find(EDITOR_EVENT_TYPE::MOVE_FRAME_TO);
    if (it != observable.end()) {
        it->second(id, index);
    }
    _editor->getDirtyManager()->markDirty({{0,0},{_editor->getWidth()-1, _editor->getHeight()-1}});
}

FrameDTO EditorVM::getActiveFrame(){
    Frame* frame = _editor->getActiveFrame();

    FrameDTO frameDTO;
    frameDTO.id = frame->getID().toString();
    frameDTO.timeDuration = frame->getFrameDuration();
    frameDTO.buffer = emscripten::val(emscripten::typed_memory_view(_editor->getWidth()* _editor->getHeight()*4, reinterpret_cast<uint8_t*>(frame->getBuffer())));
    frameDTO.width = _editor->getWidth();
    frameDTO.height = _editor->getHeight();
    frameDTO.isActive = true;
    
    return frameDTO;
}
FrameDTO EditorVM::getFrameByIndex(size_t index){
    Frame* frame = _editor->getFrameByIndex(index);

    FrameDTO frameDTO;
    frameDTO.id = frame->getID().toString();
    frameDTO.timeDuration = frame->getFrameDuration();
    frameDTO.buffer = emscripten::val(emscripten::typed_memory_view(_editor->getWidth()* _editor->getHeight()*4, reinterpret_cast<uint8_t*>(frame->getBuffer())));
    frameDTO.width = _editor->getWidth();
    frameDTO.height = _editor->getHeight();
    frameDTO.isActive = _editor->getActiveFrame() == frame;
    
    return frameDTO;
}
size_t EditorVM::getNumberFrames(){
    return _editor->getFramesLength();
}

void EditorVM::changeActiveFrame(std::string id){
    _editor->changeActiveFrame(Guid(id));
}
void EditorVM::createFrame(){
    auto frame = std::make_unique<Frame>();
    auto layer = std::make_unique<Layer>("layer 1", _editor->getWidth(), _editor->getHeight());
    frame.get()->addLayer(std::move(layer), 0);

    size_t index = (_editor->getActiveFrame() == nullptr) ? 0 : _editor->getFrameIndex(_editor->getActiveFrame()->getID())+1;

    AddFrameCommand command(*_editor, std::move(frame), index);
    command.execute();
}
void EditorVM::cloneActiveFrame(){
    Frame* frame = _editor->getActiveFrame();
    CloneFrameCommand command(*_editor, frame->getID());
    command.execute();
}
void EditorVM::moveFrameTo(std::string id, int index){
    MoveFrameToCommand command(*_editor, Guid(id), index);
    command.execute();
}
void EditorVM::moveDownActiveFrame(){
    Frame* frame = _editor->getActiveFrame();
    size_t index = _editor->getFrameIndex(frame->getID());
    if(index < 0) return;

    MoveFrameToCommand command(*_editor, frame->getID(), index - 1);
    command.execute();
}
void EditorVM::moveUpActiveFrame(){
    Frame* frame = _editor->getActiveFrame();
    size_t index = _editor->getFrameIndex(frame->getID());

    if(index >= _editor->getFramesLength()) return;

    MoveFrameToCommand command(*_editor, frame->getID(), index + 1);
    command.execute();
}
void EditorVM::removeActiveFrame(){
    Frame* frame = _editor->getActiveFrame();

    RemoveFrameCommand command(*_editor, frame->getID());
    command.execute();
}
void EditorVM::flipXFrame(){
    Frame* frame = _editor->getActiveFrame();
}
void EditorVM::flipYFrame(){
    Frame* frame = _editor->getActiveFrame();
}


#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<EditorVM>("EditorVM")
        .constructor<std::string>()
        .function("getNumberFrames", &EditorVM::getNumberFrames)
        .function("getFrameByIndex", &EditorVM::getFrameByIndex)
        .function("getActiveFrame", &EditorVM::getActiveFrame)
        .function("registerEvent", &EditorVM::registerEvent)
        .function("changeActiveFrame", &EditorVM::changeActiveFrame)
        .function("createFrame", &EditorVM::createFrame)
        .function("cloneActiveFrame", &EditorVM::cloneActiveFrame)
        .function("moveFrameTo", &EditorVM::moveFrameTo)
        .function("moveDownActiveFrame", &EditorVM::moveDownActiveFrame)
        .function("moveUpActiveFrame", &EditorVM::moveUpActiveFrame)
        .function("removeActiveFrame", &EditorVM::removeActiveFrame)
        .function("flipXFrame", &EditorVM::flipXFrame)
        .function("flipYFrame", &EditorVM::flipYFrame);
};