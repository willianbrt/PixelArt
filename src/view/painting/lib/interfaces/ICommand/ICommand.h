#ifndef ICOMMAND_H
#define ICOMMAND_H

#include "../../app/Editor/Editor.h"

class ICommand{
protected:
public:
    // ICommand(Editor& editor);
    virtual ~ICommand() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

#endif
