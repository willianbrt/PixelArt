
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

    bool getMirrorX();
    bool getMirrorY();
    bool isTilingX();
    bool isTilingY();

    void setMirrorX(bool active);
    void setMirrorY(bool active);
    void enabledTilingX(bool enabled);
    void enabledTilingY(bool enabled);
    
};

#endif