#include "EditorManager.h"

EditorManager::EditorManager(){

}

void EditorManager::createProject(int width, int height){
    std::unique_ptr<Editor> editor = std::make_unique<Editor>(width, height);
    editor->setID(Guid::generateUUID());
    
    auto frame = std::make_unique<Frame>();
    auto layer = std::make_unique<Layer>("layer 1", editor->getWidth(), editor->getHeight());
    
    frame->addLayer(std::move(layer), 0);
    editor->addFrame(std::move(frame), 0);
    _listEditor.push_back(std::move(editor));

    for (auto* obs : observers) {
        obs->onAddEditor(_listEditor[_listEditor.size()-1].get(), _listEditor.size()-1);
    }

    setActiveEditor(_listEditor.size()-1);
}
Editor* EditorManager::getActiveEditor(){
    return _activeEditor;
}
void EditorManager::setActiveEditor(int index){
    if(index < 0 || index >= _listEditor.size()) return;
    _activeEditor = _listEditor[index].get();
    
    for (auto* obs : observers) {
        obs->onChangeActiveEditor(_activeEditor->getID());
    }
}
size_t EditorManager::getEditorsLength(){
    return _listEditor.size();
}
Editor* EditorManager::getEditorByIndex(size_t index){
    return _listEditor[index].get();
}
Editor* EditorManager::getEditorById(Guid id){
    for (int i = 0; i < _listEditor.size(); i++) {
        if(_listEditor[i]->getID() == id){
            return _listEditor[i].get();
        }
    }
    return nullptr;
}
void EditorManager::registerEvent(IEditorManagerObserver* observer){
    observers.push_back(observer);
}
void EditorManager::unregisterEvent(IEditorManagerObserver* observer){
    observers.erase(
        std::remove(observers.begin(), observers.end(), observer),
        observers.end()
    );
}
