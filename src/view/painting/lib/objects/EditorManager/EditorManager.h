#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include "../../objects/Editor/Editor.h"
#include "../../objects/Clipboard/Clipboard.h"
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
    Clipboard _clipboard;

public:
    EditorManager();
    
    void createProject(int width, int height);
    Editor* getActiveEditor();
    void setActiveEditor(int index);

    size_t getEditorsLength();
    Editor* getEditorByIndex(size_t index);

    Surface* copy();
    void paste(Surface* surface);
};
#endif
