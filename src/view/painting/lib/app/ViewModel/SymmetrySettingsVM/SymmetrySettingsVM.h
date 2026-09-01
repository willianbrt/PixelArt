
#ifndef SYMMETRYSETTINGSVM_H
#define SYMMETRYSETTINGSVM_H

#include "../../AppContext/AppContext.h"
#include "../../../objects/ToolManager/ToolManager.h"

class SymmetrySettingsVM  {
private:
    ToolManager* _toolManager;
    ToolSettings* _toolSettings;

public:
    SymmetrySettingsVM();
    ~SymmetrySettingsVM();

    void setMirrorX(bool active);
    void setMirrorY(bool active);
    void setNTileX(int size);
    void setNTileY(int size);
    
};

#endif