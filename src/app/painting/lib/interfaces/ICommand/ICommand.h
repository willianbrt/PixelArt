#ifndef ICOMMAND
#define ICOMMAND

class ICommand{
    ~ICommand(){};
    virtual void execute() = 0;
    virtual void undo() = 0;
};
#endif