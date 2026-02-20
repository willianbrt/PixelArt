
#ifndef FRAMESVIEWMODEL_H
#define FRAMESVIEWMODEL_H

#include <emscripten/val.h>

#include "../../dto/FrameDTO/FrameDTO.h"

#include "../../../objects/Editor/Editor.h"
#include "../../../objects/frame/Frame.h"
#include "../../../interfaces/IEditorObserver/IEditorObserver.h"
// #include "../../../commands/HistoryCommand/HistoryCommand.h"
#include "../../../commands/AddFrameCommand/AddFrameCommand.h"
#include "../../../commands/CloneFrameCommand/CloneFrameCommand.h"
#include "../../../commands/MoveFrameToCommand/MoveFrameToCommand.h"
#include "../../../commands/RemoveFrameCommand/RemoveFrameCommand.h"


class EditorViewModel : IEditorObserver{
private:
    unordered_map<EDITOR_EVENT_TYPE, emscripten::val> observable;
    
    void onChangeActiveFrame(Guid id) override;
    void onAddFrame(Frame* frame, size_t index) override;
    void onRemoveFrame(Guid id) override;
    void onMoveFrameTo(Guid id, int index) override;

public:
    Editor& _editor;
    EditorViewModel(Editor& editor);
    ~EditorViewModel();
    FrameDTO getFrameByIndex(size_t index);
    size_t getNumberFrames();

    // void registerEvent(EDITOR_EVENT_TYPE eventType, std::function<void(EditorEvent)> callback);
    
    void changeActiveFrame(std::string id);
    void createFrame();
    void cloneActiveFrame();
    void moveFrameTo(std::string id, int index);
    void moveDownActiveFrame();
    void moveUpActiveFrame();
    void removeActiveFrame();
    void flipXFrame();
    void flipYFrame();
    
    void registerEvent(string eventType, emscripten::val callback);
};

#endif