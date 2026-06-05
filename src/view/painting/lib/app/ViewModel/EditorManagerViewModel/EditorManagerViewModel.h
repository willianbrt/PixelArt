
#ifndef FRAMESVIEWMODEL_H
#define FRAMESVIEWMODEL_H

#include <GLES3/gl3.h>

#include <emscripten/val.h>

#include "../../AppContext/AppContext.h"

#include "../../../objects/Editor/Editor.h"
#include "../../../objects/ToolManager/ToolManager.h"
#include "../../dto/EditorDTO/EditorDTO.h"
#include "../../dto/SurfaceDTO/SurfaceDTO.h"


#include "../../../interfaces/IEditorManagerObserver/IEditorManagerObserver.h"
// #include "../../../commands/HistoryCommand/HistoryCommand.h"
#include "../../../commands/CopyCommand/CopyCommand.h"
#include "../../../commands/PasteCommand/PasteCommand.h"
// #include "../../../commands/AddEditorCommand/AddEditorCommand.h"
// #include "../../../commands/CloneEditorCommand/CloneEditorCommand.h"
// #include "../../../commands/MoveEditorToCommand/MoveEditorToCommand.h"
// #include "../../../commands/RemoveEditorCommand/RemoveEditorCommand.h"


class EditorManagerViewModel : IEditorManagerObserver {
private:
    unordered_map<EDITOR_MANAGER_EVENT_TYPE, emscripten::val> observable;
    
    void onChangeActiveEditor(Guid id) override;
    void onAddEditor(Editor* editor, size_t index) override;
    void onRemoveEditor(Guid id) override;
    void onMoveEditorTo(Guid id, int index) override;
    
    EditorManager* getEditorManager();
    EditorManager* _manager;
    ToolManager* _toolManager;
    
public:
    EditorManagerViewModel();
    ~EditorManagerViewModel();

    EditorDTO getEditorByIndex(size_t index);
    size_t getNumberEditors();

    void changeActiveEditor(int id);
    void createProject(int width, int height);
    void render();
    void resize(int width, int height);
    
    void registerEvent(string eventType, emscripten::val callback);
    
    SurfaceDTO copy();
    void paste(SurfaceDTO surfaceDTO);
};

#endif