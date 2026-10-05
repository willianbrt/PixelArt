#include "EditorVM.h"


EditorVM::EditorVM(){
    _manager = AppContext::instance().getEditorManager();
    _editor = _manager->getActiveEditor();
    _editor->registerEvent(this);
}
Editor* EditorVM::getActiveEditor(){
    EditorManager* _manager = AppContext::instance().getEditorManager();

    return _manager->getActiveEditor();
}
EditorVM::~EditorVM(){
}
void EditorVM::registerEvent(string eventType, emscripten::val callback){
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
    printf("active\n");
    auto it = observable.find(EDITOR_EVENT_TYPE::CHANGE_ACTIVE_FRAME);
    if (it != observable.end()) {
        it->second(id.toString());
    }
    _editor->getDirtyManager()->markDirty({{0,0},{_editor->getWidth()-1, _editor->getHeight()-1}});
}
void EditorVM::onAddFrame(Frame* frame, size_t index){
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
    _editor->getDirtyManager()->markDirty({{0,0},{_editor->getWidth()-1, _editor->getHeight()-1}});
}
void EditorVM::onRemoveFrame(Guid id){
    printf("remove\n");
    auto it = observable.find(EDITOR_EVENT_TYPE::REMOVE_FRAME);
    if (it != observable.end()) {
        it->second(id.toString());
    }
    _editor->getDirtyManager()->markDirty({{0,0},{_editor->getWidth()-1, _editor->getHeight()-1}});
}
void EditorVM::onMoveFrameTo(Guid id, int index){
    printf("move\n");
    auto it = observable.find(EDITOR_EVENT_TYPE::MOVE_FRAME_TO);
    if (it != observable.end()) {
        it->second(id, index);
    }
    _editor->getDirtyManager()->markDirty({{0,0},{_editor->getWidth()-1, _editor->getHeight()-1}});
}

FrameDTO EditorVM::getFrameByIndex(size_t index){
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
size_t EditorVM::getNumberFrames(){
    return getActiveEditor()->getFramesLength();
}

void EditorVM::changeActiveFrame(std::string id){
    Editor* _editor = getActiveEditor();
    _editor->changeActiveFrame(Guid(id));
}
void EditorVM::createFrame(){
    Editor* _editor = getActiveEditor();
    auto frame = std::make_unique<Frame>();
    auto layer = std::make_unique<Layer>("layer 1", _editor->getWidth(), _editor->getHeight());
    frame.get()->addLayer(std::move(layer), 0);

    size_t index = (_editor->getActiveFrame() == nullptr) ? 0 : _editor->getFrameIndex(_editor->getActiveFrame()->getID())+1;

    AddFrameCommand command(*_editor, std::move(frame), index);
    command.execute();
}
void EditorVM::cloneActiveFrame(){
    Editor* _editor = getActiveEditor();
    Frame* frame = _editor->getActiveFrame();
    CloneFrameCommand command(*_editor, frame->getID());
    command.execute();
}
void EditorVM::moveFrameTo(std::string id, int index){
    Editor* _editor = getActiveEditor();
    MoveFrameToCommand command(*_editor, Guid(id), index);
    command.execute();
}
void EditorVM::moveDownActiveFrame(){
    Editor* _editor = getActiveEditor();
    Frame* frame = _editor->getActiveFrame();
    size_t index = _editor->getFrameIndex(frame->getID());
    if(index < 0) return;

    MoveFrameToCommand command(*_editor, frame->getID(), index - 1);
    command.execute();
}
void EditorVM::moveUpActiveFrame(){
    Editor* _editor = getActiveEditor();
    Frame* frame = _editor->getActiveFrame();
    size_t index = _editor->getFrameIndex(frame->getID());

    if(index >= _editor->getFramesLength()) return;

    MoveFrameToCommand command(*_editor, frame->getID(), index + 1);
    command.execute();
}
void EditorVM::removeActiveFrame(){
    Editor* _editor = getActiveEditor();
    Frame* frame = _editor->getActiveFrame();

    RemoveFrameCommand command(*_editor, frame->getID());
    command.execute();
}
void EditorVM::flipXFrame(){
    Editor* _editor = getActiveEditor();
    Frame* frame = _editor->getActiveFrame();
}
void EditorVM::flipYFrame(){
    Editor* _editor = getActiveEditor();
    Frame* frame = _editor->getActiveFrame();
}


#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<EditorVM>("EditorVM")
        .constructor<>()
        .function("getNumberFrames", &EditorVM::getNumberFrames)
        .function("getFrameByIndex", &EditorVM::getFrameByIndex)
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