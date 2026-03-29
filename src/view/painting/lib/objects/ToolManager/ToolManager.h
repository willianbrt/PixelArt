#ifndef TOOLMANAGER_H
#define TOOLMANAGER_H

#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"
#include "../../interfaces/IToolContext/IToolContext.h"

class ToolManager {
    IPressedStrategy* _toolPressed;
public:
    ToolManager();
    
    // void changeToolPressed(std::string toolName);
    void changeToolPressed(IPressedStrategy* toolPressed);
    IPressedStrategy* getToolPressed();
};
#endif