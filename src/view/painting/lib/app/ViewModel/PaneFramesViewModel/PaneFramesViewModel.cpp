#include "PaneFramesViewModel.h"


PaneFramesViewModel::PaneFramesViewModel(){
    getActiveEditor()->registerEvent(this);
}
Editor* PaneFramesViewModel::getActiveEditor(){
    EditorManager* _manager = AppContext::instance().getEditorManager();

    return _manager->getActiveEditor();
}
PaneFramesViewModel::~PaneFramesViewModel(){
}
void PaneFramesViewModel::registerEvent(string eventType, emscripten::val callback){
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

void PaneFramesViewModel::onChangeActiveFrame(Guid id){
    printf("active\n");
    auto it = observable.find(EDITOR_EVENT_TYPE::CHANGE_ACTIVE_FRAME);
    if (it != observable.end()) {
        it->second(id.toString());
    }
}
void PaneFramesViewModel::onAddFrame(Frame* frame, size_t index){
    printf("add\n");
    Editor* _editor = getActiveEditor();

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
}
void PaneFramesViewModel::onRemoveFrame(Guid id){
    printf("remove\n");
    auto it = observable.find(EDITOR_EVENT_TYPE::REMOVE_FRAME);
    if (it != observable.end()) {
        it->second(id.toString());
    }
}
void PaneFramesViewModel::onMoveFrameTo(Guid id, int index){
    printf("move\n");
    auto it = observable.find(EDITOR_EVENT_TYPE::MOVE_FRAME_TO);
    if (it != observable.end()) {
        it->second(id, index);
    }
}

FrameDTO PaneFramesViewModel::getFrameByIndex(size_t index){
    Editor* activeEditor = getActiveEditor();
    Frame* frame = activeEditor->getFrameByIndex(index);

    FrameDTO frameDTO;
    frameDTO.id = frame->getID().toString();
    frameDTO.timeDuration = frame->getFrameDuration();
    frameDTO.buffer = emscripten::val(emscripten::typed_memory_view(activeEditor->getWidth()* activeEditor->getHeight()*4, reinterpret_cast<uint8_t*>(frame->getBuffer())));
    frameDTO.width = activeEditor->getWidth();
    frameDTO.height = activeEditor->getHeight();
    frameDTO.isActive = activeEditor->getActiveFrame() == frame;
    
    return frameDTO;
}
size_t PaneFramesViewModel::getNumberFrames(){
    return getActiveEditor()->getFramesLength();
}

void PaneFramesViewModel::changeActiveFrame(std::string id){
    Editor* _editor = getActiveEditor();
    _editor->changeActiveFrame(Guid(id));
}
void PaneFramesViewModel::createFrame(){
    Editor* _editor = getActiveEditor();
    auto frame = std::make_unique<Frame>();
    auto layer = std::make_unique<Layer>("layer 1", _editor->getWidth(), _editor->getHeight());
    frame.get()->addLayer(std::move(layer), 0);

    size_t index = (_editor->getActiveFrame() == nullptr) ? 0 : _editor->getFrameIndex(_editor->getActiveFrame()->getID())+1;

    AddFrameCommand command(*_editor, std::move(frame), index);
    command.execute();
}
void PaneFramesViewModel::cloneActiveFrame(){
    Editor* _editor = getActiveEditor();
    Frame* frame = _editor->getActiveFrame();
    CloneFrameCommand command(*_editor, frame->getID());
    command.execute();
}
void PaneFramesViewModel::moveFrameTo(std::string id, int index){
    Editor* _editor = getActiveEditor();
    MoveFrameToCommand command(*_editor, Guid(id), index);
    command.execute();
}
void PaneFramesViewModel::moveDownActiveFrame(){
    Editor* _editor = getActiveEditor();
    Frame* frame = _editor->getActiveFrame();
    size_t index = _editor->getFrameIndex(frame->getID());
    if(index < 0) return;

    MoveFrameToCommand command(*_editor, frame->getID(), index - 1);
    command.execute();
}
void PaneFramesViewModel::moveUpActiveFrame(){
    Editor* _editor = getActiveEditor();
    Frame* frame = _editor->getActiveFrame();
    size_t index = _editor->getFrameIndex(frame->getID());

    if(index >= _editor->getFramesLength()) return;

    MoveFrameToCommand command(*_editor, frame->getID(), index + 1);
    command.execute();
}
void PaneFramesViewModel::removeActiveFrame(){
    Editor* _editor = getActiveEditor();
    Frame* frame = _editor->getActiveFrame();

    RemoveFrameCommand command(*_editor, frame->getID());
    command.execute();
}
void PaneFramesViewModel::flipXFrame(){
    Editor* _editor = getActiveEditor();
    Frame* frame = _editor->getActiveFrame();
}
void PaneFramesViewModel::flipYFrame(){
    Editor* _editor = getActiveEditor();
    Frame* frame = _editor->getActiveFrame();
}


#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<PaneFramesViewModel>("PaneFramesViewModel")
        .constructor<>()
        .function("getNumberFrames", &PaneFramesViewModel::getNumberFrames)
        .function("getFrameByIndex", &PaneFramesViewModel::getFrameByIndex)
        .function("registerEvent", &PaneFramesViewModel::registerEvent)
        .function("changeActiveFrame", &PaneFramesViewModel::changeActiveFrame)
        .function("createFrame", &PaneFramesViewModel::createFrame)
        .function("cloneActiveFrame", &PaneFramesViewModel::cloneActiveFrame)
        .function("moveFrameTo", &PaneFramesViewModel::moveFrameTo)
        .function("moveDownActiveFrame", &PaneFramesViewModel::moveDownActiveFrame)
        .function("moveUpActiveFrame", &PaneFramesViewModel::moveUpActiveFrame)
        .function("removeActiveFrame", &PaneFramesViewModel::removeActiveFrame)
        .function("flipXFrame", &PaneFramesViewModel::flipXFrame)
        .function("flipYFrame", &PaneFramesViewModel::flipYFrame);
};