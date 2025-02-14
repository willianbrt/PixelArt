#ifndef LINE_COMMAND
#define LINE_COMMAND

#include "../helpers/pixel.cpp"
#include "../PixelEditor.cpp"

#include "../tools/Line.cpp"
#include "../interfaces/ICommand.cpp"

class LineCommand : ICommand
{
private:
    Point flagPoint;
    PixelEditor editor;
    Line line;
public:
    LineCommand(Line& line){
        line = line;
    }
    ~LineCommand(){

    }
    void execute(){
        
    }
    void undo(){

    }
};



#endif