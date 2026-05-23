#include "EditorManagerViewModel.h"


EditorManagerViewModel::EditorManagerViewModel(){
    _manager = AppContext::instance().getEditorManager();
}
EditorManagerViewModel::~EditorManagerViewModel(){
}

EditorManager* EditorManagerViewModel::getEditorManager(){
    return  AppContext::instance().getEditorManager();
}

EditorDTO EditorManagerViewModel::getEditorByIndex(size_t index){
    Editor* _activeEditor = _manager->getActiveEditor();
    EditorDTO editorDTO;

    return editorDTO;
}
size_t EditorManagerViewModel::getNumberEditors(){
    return _manager->getEditorsLength();
}

void EditorManagerViewModel::registerEvent(string eventType, emscripten::val callback){
    if(eventType == "ADD_EDITOR"){
        observable[EDITOR_MANAGER_EVENT_TYPE::ADD_EDITOR] = callback;
        return;
    }
}

void EditorManagerViewModel::changeActiveEditor(int id){
    _manager->setActiveEditor(id);
}
void EditorManagerViewModel::createProject(int width, int height){
    _manager->createProject(width,height);
}
void EditorManagerViewModel::render(){
    // _manager->getActiveEditor()->render();
}
// void EditorManagerViewModel::resizeWindow(int width, int height){
//     _manager->getActiveEditor()->resize(width, height);
// }
void EditorManagerViewModel::resize(int width, int height){
    _manager->getActiveEditor()->resize(width, height);
}
void EditorManagerViewModel::onChangeActiveEditor(Guid id){
    auto it = observable.find(EDITOR_MANAGER_EVENT_TYPE::CHANGE_ACTIVE_EDITOR);
    if (it != observable.end()) {
        it->second(id.toString());
    }
}
void EditorManagerViewModel::onAddEditor(Editor* editor, size_t index){
    EditorDTO editorDTO;

    auto it = observable.find(EDITOR_MANAGER_EVENT_TYPE::ADD_EDITOR);
    if (it != observable.end()) {
        it->second(editorDTO, index);
    }
}
void EditorManagerViewModel::onRemoveEditor(Guid id){
    auto it = observable.find(EDITOR_MANAGER_EVENT_TYPE::REMOVE_EDITOR);
    if (it != observable.end()) {
        it->second(id.toString());
    }
}
void EditorManagerViewModel::onMoveEditorTo(Guid id, int index){
    auto it = observable.find(EDITOR_MANAGER_EVENT_TYPE::MOVE_EDITOR_TO);
    if (it != observable.end()) {
        it->second(id, index);
    }
}

SurfaceDTO EditorManagerViewModel::copy(){
    Surface* surface = _manager->copy();
    
    SurfaceDTO surfaceDTO;
    surfaceDTO.width = surface->getWidth();
    surfaceDTO.height = surface->getHeight();
    surfaceDTO.buffer = emscripten::val(emscripten::typed_memory_view(surface->getLength()*4, reinterpret_cast<uint8_t*>(surface->getBuffer())));

    return surfaceDTO;
}
void EditorManagerViewModel::paste(SurfaceDTO surfaceDTO){
    size_t byteOffset = surfaceDTO.buffer["byteOffset"].as<size_t>();
    uint8_t* bytes = reinterpret_cast<uint8_t*>(byteOffset);
    unsigned int* data = reinterpret_cast<unsigned int*>(bytes);
    
    Surface* surface = new Surface(data, surfaceDTO.width, surfaceDTO.height);
    // Surface surface = Surface(surfaceDTO.width, surfaceDTO.height);
    _manager->paste(surface);
}


#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<EditorManagerViewModel>("EditorManagerViewModel")
        .constructor<>()
        .function("getNumberEditors", &EditorManagerViewModel::getNumberEditors)
        .function("getEditorByIndex", &EditorManagerViewModel::getEditorByIndex)
        .function("registerEvent", &EditorManagerViewModel::registerEvent)
        .function("changeActiveEditor", &EditorManagerViewModel::changeActiveEditor)
        .function("createProject", &EditorManagerViewModel::createProject)
        .function("render", &EditorManagerViewModel::render)
        .function("resize", &EditorManagerViewModel::resize)
        .function("copy", &EditorManagerViewModel::copy)
        .function("paste", &EditorManagerViewModel::paste)
        ;
};