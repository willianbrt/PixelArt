#ifndef TOOLMANAGER_H
#define TOOLMANAGER_H

#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"
#include "../../interfaces/IToolContext/IToolContext.h"

class ToolManager {
    IPressedStrategy& toolPressed;

    ToolManager();
    
    // IPressedStrategy& getToolPressed();
    // void changeToolPressed(std::string toolName);
};
#endif