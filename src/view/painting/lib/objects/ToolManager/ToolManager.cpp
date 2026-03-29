#include "ToolManager.h"
ToolManager::ToolManager(){}

void ToolManager::changeToolPressed(IPressedStrategy* toolPressed){
    _toolPressed = toolPressed;
}

IPressedStrategy* ToolManager::getToolPressed(){
    return _toolPressed;
}