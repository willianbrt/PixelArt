#include "HistoryCommand.h"

HistoryCommand::HistoryCommand(){}
HistoryCommand::~HistoryCommand(){}

void HistoryCommand::execute(ICommand* command){
    if(history.size() == HITORY_SIZE){
        history.erase(history.begin());
    }
    command->execute();

    listToRedo.clear();
    history.emplace_back(command);
};
void HistoryCommand::undo(){
    ICommand* command = history.back();
    command->undo();

    listToRedo.emplace_back(command);
    history.pop_back();
};
void HistoryCommand::redo(){
    ICommand* command = listToRedo.back();
    command->execute();

    history.emplace_back(command);
    listToRedo.pop_back();
};
