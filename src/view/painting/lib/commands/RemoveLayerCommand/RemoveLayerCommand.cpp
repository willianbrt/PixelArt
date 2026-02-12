#include "RemoveLayerCommand.h"

RemoveLayerCommand::RemoveLayerCommand(Frame& frame, Guid frameId) : _frame(frame){}
RemoveLayerCommand::~RemoveLayerCommand(){}
void RemoveLayerCommand::execute(){}
void RemoveLayerCommand::undo(){}