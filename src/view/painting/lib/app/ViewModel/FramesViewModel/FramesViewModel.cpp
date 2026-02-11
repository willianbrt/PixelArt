#include "FramesViewModel.h"


FramesViewModel::FramesViewModel(Editor& editor) : _editor(editor){
    Frame* frame = _editor.getActiveFrame();

}
FramesViewModel::~FramesViewModel(){
}
vector<Frame*>& FramesViewModel::getAllFrames(){
    return _editor.getAllFrames();
}

void FramesViewModel::registerEvent(EDITOR_EVENT_TYPE eventType, std::function<void(EditorEvent)> callback){
    _editor.registerEvent(eventType, callback);
}
void FramesViewModel::onChangeActiveFrame(Guid id){
    _editor.changeActiveFrame(id);
}
void FramesViewModel::onAddFrame(){
    Frame frame;
    AddFrameCommand command(frame, _editor);
    command.execute();
}
void FramesViewModel::onDuplicateFrame(){
    Frame* frame = _editor.getActiveFrame();
    CloneFrameCommand command(frame->getID(), _editor);
    command.execute();
}
void FramesViewModel::onMoveFrameTo(Guid id, int index){
    MoveFrameToCommand command(_editor, id, index);
    command.execute();
}
void FramesViewModel::onMoveDownFrame(){
    Frame* frame = _editor.getActiveFrame();
    size_t index = _editor.getFrameIndex(frame->getID());
    if(index > 0) return;

    MoveFrameToCommand command(_editor, frame->getID(), index - 1);
    command.execute();
}
void FramesViewModel::onMoveUpFrame(){
    Frame* frame = _editor.getActiveFrame();
    size_t index = _editor.getFrameIndex(frame->getID());

    if(index >= _editor.getAllFrames().size()) return;

    MoveFrameToCommand command(_editor, frame->getID(), index + 1);
    command.execute();
}
void FramesViewModel::onRemoveFrame(){
    Frame* frame = _editor.getActiveFrame();
    size_t index = _editor.getFrameIndex(frame->getID());

    RemoveFrameCommand command(_editor, frame->getID());
    command.execute();
}
void FramesViewModel::onFlipXFrame(){
    Frame* frame = _editor.getActiveFrame();
    // AddFrameCommand command(_editor, frame->getID());
    // command.execute();
}
void FramesViewModel::onFlipYFrame(){
    Frame* frame = _editor.getActiveFrame();
    // AddFrameCommand command(_editor, frame->getID());
    // command.execute();
}