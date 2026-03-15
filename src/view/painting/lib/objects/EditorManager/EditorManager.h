#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include "../../objects/Editor/Editor.h"
#include <vector>


#include "../../objects/Renderer/Renderer.h"
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

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

    // int _width = 0;
    // int _height = 0;
    // GLFWwindow* window;
    // Render* renderer;

public:
    EditorManager();
    
    void createProject(int width, int height);
    Editor* getActiveEditor();
    void setActiveEditor(int index);

    size_t getEditorsLength();
    Editor* getEditorByIndex(size_t index);

};
#endif
