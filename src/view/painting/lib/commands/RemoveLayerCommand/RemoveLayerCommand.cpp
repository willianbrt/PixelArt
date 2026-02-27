#include "RemoveLayerCommand.h"

RemoveLayerCommand::RemoveLayerCommand(Frame& frame, Guid layerId) : _frame(frame), _layerId(layerId){}
RemoveLayerCommand::~RemoveLayerCommand(){}
void RemoveLayerCommand::execute(){
    _index = _frame.getLayerIndex(_layerId);

    if(_frame.getLayersLength() == 1){
        auto _layerAdded = std::make_unique<Layer>("layer 1", 32,32);
        _layerAddedID = _layerAdded->getID();

        _frame.addLayer(std::move(_layerAdded), 1);
    }
    if(_frame.getLayerByIndex(_index) == _frame.getActiveLayer()){
        size_t activeIndex = (_index < _frame.getLayersLength()-1) ? _index+1 : _index-1;
        
       _frame.changeActiveLayer(_frame.getLayerByIndex(activeIndex)->getID());
    }
    
    _layerRemoved = _frame.removeLayer(_index);
}
void RemoveLayerCommand::undo(){
    
    _frame.addLayer(std::move(_layerRemoved), _index);
    
    _index = _frame.getLayerIndex(_layerAddedID);
    _layerRemoved = _frame.removeLayer(_index);
}