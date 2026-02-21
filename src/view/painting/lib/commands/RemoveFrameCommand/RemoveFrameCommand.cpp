#include "RemoveFrameCommand.h"

RemoveFrameCommand::RemoveFrameCommand(Editor& editor, Guid frameId) : _editor(editor), _frameId(frameId){}
RemoveFrameCommand::~RemoveFrameCommand(){}
void RemoveFrameCommand::execute(){
    _index = _editor.getFrameIndex(_frameId);

    if(_editor.getFramesLength() == 1){
        std::unique_ptr<Frame> _frameAdded = make_unique<Frame>();
        auto layer = std::make_unique<Layer>("layer 1", _editor.getWidth(), _editor.getHeight());
        _frameAdded.get()->addLayer(std::move(layer), 0);
        _frameAddedID = _frameAdded->getID();

        _editor.addFrame(std::move(_frameAdded), 1);
    }
    if(_editor.getFrameByIndex(_index) == _editor.getActiveFrame()){
        size_t activeIndex = (_index < _editor.getFramesLength()-1) ? _index+1 : _index-1;
        
       _editor.changeActiveFrame(_editor.getFrameByIndex(activeIndex)->getID());
    }
    
    _frameRemoved = _editor.removeFrame(_index);
}
void RemoveFrameCommand::undo(){
    _editor.addFrame(std::move(_frameRemoved), _index);
    
    _index = _editor.getFrameIndex(_frameAddedID);
    _frameRemoved = _editor.removeFrame(_index);
}