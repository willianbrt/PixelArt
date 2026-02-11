
#ifndef FRAMESVIEWMODEL_H
#define FRAMESVIEWMODEL_H

#include "../../Editor/Editor.h"
#include "../../../objects/frame/Frame.h"
// #include "../../../interfaces/IObservableList/IObservableList.h"
// #include "../../../commands/HistoryCommand/HistoryCommand.h"
#include "../../../commands/AddFrameCommand/AddFrameCommand.h"
#include "../../../commands/CloneFrameCommand/CloneFrameCommand.h"
#include "../../../commands/MoveFrameToCommand/MoveFrameToCommand.h"
#include "../../../commands/RemoveFrameCommand/RemoveFrameCommand.h"


class FramesViewModel{
private:

public:
    Editor& _editor;
    FramesViewModel(Editor& editor);
    ~FramesViewModel();
    
    vector<Frame*>& getAllFrames();
    void registerEvent(EDITOR_EVENT_TYPE eventType, std::function<void(EditorEvent)> callback);

    void onChangeActiveFrame(Guid id);
    void onAddFrame();
    void onDuplicateFrame();
    void onMoveFrameTo(Guid id, int index);
    void onMoveDownFrame();
    void onMoveUpFrame();
    void onRemoveFrame();
    void onFlipXFrame();
    void onFlipYFrame();

};

#endif