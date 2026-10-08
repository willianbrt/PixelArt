#include "EditorManagerViewModel.h"


EditorManagerViewModel::EditorManagerViewModel(){
    _manager = AppContext::instance().getEditorManager();
    _toolManager = AppContext::instance().getToolManager();
    _manager->registerEvent(this);
}
EditorManagerViewModel::~EditorManagerViewModel(){
    _manager->unregisterEvent(this);
}

EditorManager* EditorManagerViewModel::getEditorManager(){
    return  AppContext::instance().getEditorManager();
}

EditorDTO EditorManagerViewModel::getActiveEditor(){
    Editor* editor = _manager->getActiveEditor();
    Frame* frame = editor->getActiveFrame();

    EditorDTO editorDTO;
    editorDTO.id = editor->getID().toString();
    editorDTO.activeFrameId = frame->getID().toString();
    editorDTO.width = editor->getWidth();
    editorDTO.height = editor->getHeight();

    return editorDTO;
}
EditorDTO EditorManagerViewModel::getEditorByIndex(size_t index){
    Editor* _activeEditor = _manager->getActiveEditor();
    EditorDTO editorDTO;

    return editorDTO;
}
size_t EditorManagerViewModel::getNumberEditors(){
    return _manager->getEditorsLength();
}

void EditorManagerViewModel::registerEvent(std::string eventType, emscripten::val callback){
    if(eventType == "ADD_EDITOR"){
        observable[EDITOR_MANAGER_EVENT_TYPE::ADD_EDITOR] = callback;
        return;
    }
    if(eventType == "CHANGE_ACTIVE_EDITOR"){
        observable[EDITOR_MANAGER_EVENT_TYPE::CHANGE_ACTIVE_EDITOR] = callback;
        return;
    }
    if(eventType == "REMOVE_EDITOR"){
        observable[EDITOR_MANAGER_EVENT_TYPE::REMOVE_EDITOR] = callback;
        return;
    }
    if(eventType == "MOVE_EDITOR_TO"){
        observable[EDITOR_MANAGER_EVENT_TYPE::MOVE_EDITOR_TO] = callback;
        return;
    }
}

void EditorManagerViewModel::changeActiveEditor(int id){
    _manager->setActiveEditor(id);
}
void EditorManagerViewModel::createProject(int width, int height){
    _manager->createProject(width,height);

    Surface* sketch = _manager->getActiveEditor()->getSurface();
    ViewportContext* viewport =  AppContext::instance().getViewport();
    CanvasSettings* canvas = _manager->getActiveEditor()->getCanvasSettings();
    canvas->zoom(canvas->getFitScale(viewport, sketch),{0,0}, viewport, sketch);
    canvas->pan(canvas->getFitPosition(viewport, sketch), viewport, sketch);
}
void EditorManagerViewModel::resize(int width, int height){
    _manager->getActiveEditor()->resize(width, height);
    
    Surface* sketch = _manager->getActiveEditor()->getSurface();
    ViewportContext* viewport =  AppContext::instance().getViewport();
    CanvasSettings* canvas = _manager->getActiveEditor()->getCanvasSettings();
    canvas->zoom(canvas->getFitScale(viewport, sketch),{0,0}, viewport, sketch);
    canvas->pan(canvas->getFitPosition(viewport, sketch), viewport, sketch);
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
    Editor* editor = AppContext::instance().getEditorManager()->getActiveEditor();
    Surface* surface = AppContext::instance().getClipboard()->copy(editor->getSelectContext());
    
    SurfaceDTO surfaceDTO;
    surfaceDTO.width = surface->getWidth();
    surfaceDTO.height = surface->getHeight();
    surfaceDTO.buffer = emscripten::val(emscripten::typed_memory_view(surface->getLength()*4, reinterpret_cast<uint8_t*>(surface->getBuffer())));

    return surfaceDTO;
}
void EditorManagerViewModel::paste(){
    PasteCommand pasteCommand = PasteCommand(_manager->getActiveEditor(), AppContext::instance().getClipboard(), AppContext::instance().getToolManager());
    pasteCommand.execute();
}


#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<EditorManagerViewModel>("EditorManagerViewModel")
        .constructor<>()
        .function("getNumberEditors", &EditorManagerViewModel::getNumberEditors)
        .function("getActiveEditor", &EditorManagerViewModel::getActiveEditor)
        .function("getEditorByIndex", &EditorManagerViewModel::getEditorByIndex)
        .function("registerEvent", &EditorManagerViewModel::registerEvent)
        .function("changeActiveEditor", &EditorManagerViewModel::changeActiveEditor)
        .function("createProject", &EditorManagerViewModel::createProject)
        .function("resize", &EditorManagerViewModel::resize)
        .function("copy", &EditorManagerViewModel::copy)
        .function("paste", &EditorManagerViewModel::paste)
        ;
};