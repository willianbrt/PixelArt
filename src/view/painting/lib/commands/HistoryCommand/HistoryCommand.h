
#ifndef IHISTORYCOMMAND_H
#define IHISTORYCOMMAND_H

#include <vector>
#include "../../interfaces/ICommand/ICommand.h"

class HistoryCommand {
private:
    const int HITORY_SIZE = 26;
    vector<ICommand*> history;
    vector<ICommand*> listToRedo;
public:
    HistoryCommand();
    ~HistoryCommand();
    void execute(ICommand* command);
    void undo();
    void redo();
};

#endif