#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include "../../objects/Editor/Editor.h"
#include "../../interfaces/IEditorManagerObserver/IEditorManagerObserver.h"
#include <vector>

enum EDITOR_MANAGER_EVENT_TYPE{
    ADD_EDITOR,
    REMOVE_EDITOR,
    MOVE_EDITOR_TO,
    CHANGE_ACTIVE_EDITOR
};

class EditorManager{
private:
    std::vector<std::unique_ptr<Editor>> _listEditor;
    Editor* _activeEditor;
    vector<IEditorManagerObserver*> observers;

public:
    EditorManager();
    
    void createProject(int width, int height);
    Editor* getActiveEditor();
    void setActiveEditor(int index);

    size_t getEditorsLength();
    Editor* getEditorByIndex(size_t index);
    Editor* getEditorById(Guid id);

    void registerEvent(IEditorManagerObserver* observer);
    void unregisterEvent(IEditorManagerObserver* observer);
};
#endif
