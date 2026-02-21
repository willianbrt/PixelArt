#include "EditorViewModel.h"


EditorViewModel::EditorViewModel(Editor& editor) : _editor(editor){    
    _editor.registerEvent(this);
}
EditorViewModel::~EditorViewModel(){
}
FrameDTO EditorViewModel::getFrameByIndex(size_t index){
    Frame* frame = _editor.getFrameByIndex(index);

    FrameDTO frameDTO;
    frameDTO.id = frame->getID().toString();
    frameDTO.timeDuration = frame->getFrameDuration();
    frameDTO.buffer = emscripten::val(emscripten::typed_memory_view(_editor.getWidth()* _editor.getHeight()*4, reinterpret_cast<uint8_t*>(frame->getBuffer())));
    frameDTO.width = _editor.getWidth();
    frameDTO.height = _editor.getHeight();
    frameDTO.isActive = _editor.getActiveFrame() == frame;
    
    return frameDTO;
}
size_t EditorViewModel::getNumberFrames(){
    return _editor.getFramesLength();
}

void EditorViewModel::registerEvent(string eventType, emscripten::val callback){
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

FrameViewModel EditorViewModel::getLayersViewModel(){
    
}
void EditorViewModel::changeActiveFrame(std::string id){
    _editor.changeActiveFrame(Guid(id));
}
void EditorViewModel::createFrame(){
    auto frame = std::make_unique<Frame>();
    auto layer = std::make_unique<Layer>("layer 1", _editor.getWidth(), _editor.getHeight());
    frame.get()->addLayer(std::move(layer), 0);

    size_t index = (_editor.getActiveFrame() == nullptr) ? 0 : _editor.getFrameIndex(_editor.getActiveFrame()->getID())+1;

    AddFrameCommand command(std::move(frame), index, _editor);
    command.execute();
}
void EditorViewModel::cloneActiveFrame(){
    Frame* frame = _editor.getActiveFrame();
    CloneFrameCommand command(frame->getID(), _editor);
    command.execute();
}
void EditorViewModel::moveFrameTo(std::string id, int index){
    MoveFrameToCommand command(_editor, Guid(id), index);
    command.execute();
}
void EditorViewModel::moveDownActiveFrame(){
    Frame* frame = _editor.getActiveFrame();
    size_t index = _editor.getFrameIndex(frame->getID());
    if(index < 0) return;

    MoveFrameToCommand command(_editor, frame->getID(), index - 1);
    command.execute();
}
void EditorViewModel::moveUpActiveFrame(){
    Frame* frame = _editor.getActiveFrame();
    size_t index = _editor.getFrameIndex(frame->getID());

    if(index >= _editor.getFramesLength()) return;

    MoveFrameToCommand command(_editor, frame->getID(), index + 1);
    command.execute();
}
void EditorViewModel::removeActiveFrame(){
    Frame* frame = _editor.getActiveFrame();

    RemoveFrameCommand command(_editor, frame->getID());
    command.execute();
}
void EditorViewModel::flipXFrame(){
    Frame* frame = _editor.getActiveFrame();
    // AddFrameCommand command(_editor, frame->getID());
    // command.execute();
}
void EditorViewModel::flipYFrame(){
    Frame* frame = _editor.getActiveFrame();
    // AddFrameCommand command(_editor, frame->getID());
    // command.execute();
}

void EditorViewModel::onChangeActiveFrame(Guid id){
    auto it = observable.find(EDITOR_EVENT_TYPE::CHANGE_ACTIVE_FRAME);
    if (it != observable.end()) {
        it->second(id.toString());
    }
}
void EditorViewModel::onAddFrame(Frame* frame, size_t index){
    unsigned int* buffer = frame->getBuffer();
    FrameDTO frameDTO;
    frameDTO.id = frame->getID().toString();
    frameDTO.timeDuration = frame->getFrameDuration();
    frameDTO.buffer = emscripten::val(emscripten::typed_memory_view(_editor.getWidth()* _editor.getHeight()*4, reinterpret_cast<uint8_t*>(buffer)));
    frameDTO.width = _editor.getWidth();
    frameDTO.height = _editor.getHeight();
    frameDTO.isActive = _editor.getActiveFrame() == frame;

    auto it = observable.find(EDITOR_EVENT_TYPE::ADD_FRAME);
    if (it != observable.end()) {
        it->second(frameDTO, index);
    }
    free(buffer);
}
void EditorViewModel::onRemoveFrame(Guid id){
    auto it = observable.find(EDITOR_EVENT_TYPE::REMOVE_FRAME);
    if (it != observable.end()) {
        it->second(id.toString());
    }
}
void EditorViewModel::onMoveFrameTo(Guid id, int index){
    auto it = observable.find(EDITOR_EVENT_TYPE::MOVE_FRAME_TO);
    if (it != observable.end()) {
        it->second(id, index);
    }
}

#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<EditorViewModel>("EditorViewModel")
        .constructor<Editor&>()
        // .smart_ptr<std::shared_ptr<EditorViewModel>>("shared_ptr<EditorViewModel>")
        .function("getNumberFrames", &EditorViewModel::getNumberFrames)
        .function("getFrameByIndex", &EditorViewModel::getFrameByIndex)
        .function("registerEvent", &EditorViewModel::registerEvent)
        .function("changeActiveFrame", &EditorViewModel::changeActiveFrame)
        .function("createFrame", &EditorViewModel::createFrame)
        .function("cloneActiveFrame", &EditorViewModel::cloneActiveFrame)
        .function("moveFrameTo", &EditorViewModel::moveFrameTo)
        .function("moveDownActiveFrame", &EditorViewModel::moveDownActiveFrame)
        .function("moveUpActiveFrame", &EditorViewModel::moveUpActiveFrame)
        .function("removeActiveFrame", &EditorViewModel::removeActiveFrame)
        .function("flipXFrame", &EditorViewModel::flipXFrame)
        .function("flipYFrame", &EditorViewModel::flipYFrame);
};